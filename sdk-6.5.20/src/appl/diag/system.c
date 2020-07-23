/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>

#ifndef __KERNEL__
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <setjmp.h>
#endif

#define SAL_ALLOC(x) sal_alloc(x, __FILE__)

#include <sal/core/libc.h>
#include <appl/diag/sysconf.h>
#include <appl/diag/bslmgmt.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/system.h>

#ifdef BCM_LTSW_SUPPORT
#include <appl/diag/sysconf_ltsw.h>
#endif /* BCM_LTSW_SUPPORT */

#if (defined(BROADCOM_DEBUG) && defined(SW_STATE_MEM_MONITOR))
#include <shared/swstate/sw_state.h>
#else
#define DISPLAY_SW_STATE_MEM
#endif /* (defined(BROADCOM_DEBUG) && defined(SW_STATE_MEM_MONITOR)) */

#if defined(BCM_SAND_SUPPORT)
#include <shared/bslenum.h>
#include <appl/diag/bsldnx.h>
#endif /* BCM_SAND_SUPPORT */

/* #define INCLUDE_NFS 12345678*/

/* Undefine these OS definitions. Only features if they were requested */
#ifdef INCLUDE_NFS
#undef INCLUDE_NFS
#endif /* INCLUDE_NFS */
#ifdef INCLUDE_TELNET
#undef INCLUDE_TELNET
#endif /* INCLUDE_TELNET */

#include <sal/appl/pci.h>
#include <sal/appl/config.h>
#include <sal/core/boot.h>
#include <soc/memtune.h>
#include <soc/devids.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/scache.h>

#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/adapter/adapter_reg_access.h>
#endif
#include <soc/l2x.h>
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#include <soc/mcm/driver.h>
#endif

#include <bcm/error.h>
#include <bcm/init.h>
#include <bcm/stat.h>
#include <bcm/link.h>
#include <bcm/port.h>
#include <bcm/rx.h>
#include <bcm_int/control.h>

#if defined(PLISIM)
#include <bde/pli/verinet.h>
#endif

#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <appl/diag/warmboot.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#include <shared/shr_bprof.h>

#ifdef CANCUN_SUPPORT
#include <soc/esw/cancun.h>
#endif

#ifdef BCM_LTSW_SUPPORT
#include <soc/ltsw/drv.h>
#endif

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
#include <bcm/flowtracker.h>
#endif

/*
 * Global variable for currently attached unit in system.
 */

#ifdef INCLUDE_TCL
static char bootdir[256];
#endif

#ifdef BCM_DNX_SUPPORT
#ifdef ADAPTER_SERVER_MODE
/** In order to support parallel units init,  */
/** need to lock the dnx adapter stage, which do not support multi-threading. */
/** since this mutex is between units, we create it before splitting to units. */
sal_mutex_t dnx_adapter_init_lock = NULL;
#endif
#endif
#ifndef NO_SAL_APPL
static char *soc_init_rc[SOC_MAX_NUM_DEVICES];
#endif

#if defined(BCM_ESW_SUPPORT)
extern void mem_watch_counter_init(int unit);
extern void mem_watch_start(int unit);
#endif

#ifdef BMW
extern int sysIsCFM(void);
extern int sysSlotIdGet(void);
#endif
#ifdef MBZ
extern int sysIsLM();
extern int sysSlotIdGet();
#endif /* MBZ */

/*
 * Routine to set useful global variables for script files to use.
 */
#if ((defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)) && !defined(PLISIM) && defined(BCM_PETRA_SUPPORT))

#include <soc/i2c.h>
#include <soc/dcmn/dcmn_utils_eeprom.h>
#include <appl/diag/dcmn/bsp_cards_consts.h>
extern int cpu_i2c_read_extended(int chip, int addr, CPU_I2C_BUS_LEN alen, int* p_val, int donot_print_errors);

int sys_board_type_mng_get(void) {
#ifndef __KERNEL__
    int val = 0;

    /* Do not report an error if the I2C read fails */
    if (cpu_i2c_read_extended(0x42, 0x11, CPU_I2C_ALEN_LONG_DLEN_LONG, &val, 1)) {
        /* Error: sal_printf("sysSlotIdGet: Failed reading Mng slot_id, Setting to 0\n"); */
        return 0;
    }

    if ((val & 0x3f) == 0x2f) {
        return 1;
    } else {
        return 0;
    }
#endif /* __KERNEL__ */
    sal_printf("This function is unavailable in Kernel mode\n");
    return 0;
}

int sysBoardTypeGfaBiGet(void) {
      int val = 0;

    if (eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE, NEGEV_CHASSIS_EEPROM_BYTE_SIZE_BOARD_TYPE, &val)) {
        /* Error */
        sal_printf("sysBoardTypeGfaBiGet: Failed reading board_type, Setting to 0. val=%d\n", val);
        return 0;
    }
    
      if (val == LINE_CARD_GFA_PETRA_B_INTERLAKEN) {
            return 1;
      } else {
            return 0;
      }
}

int sysBoardTypeGfaBi2Get(void) {
    int val = 0;

    if (eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE, NEGEV_CHASSIS_EEPROM_BYTE_SIZE_BOARD_TYPE, &val)) {
        /* Error */
        /* sal_printf("sysBoardTypeGfaBi2Get: Failed reading board_type, Setting to 0. val=%d\n", val); */
        return 0;
    }
    
    if (val == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2) {
        return 1;
    } else {
        return 0;
    }
}

int sysBoardTypeAradCardGet(void) {
    int val = 0;

    if (eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE, NEGEV_CHASSIS_EEPROM_BYTE_SIZE_BOARD_TYPE, &val)) {
        /* Error */
        /* sal_printf("sysBoardTypeGfaBi2Get: Failed reading board_type, Setting to 0. val=%d\n", val); */
        return 0;
    }
    
    if ((val == LINE_CARD_ARAD) || (LINE_CARD_ARAD_NOACP)) {
        return 1;
    } else {
        return 0;
    }
}

int sysSlotIdGet(void) {
#ifndef __KERNEL__
    int val;

    if (sys_board_type_mng_get()) {
        return -1;
    }else if (sysBoardTypeGfaBiGet()) {
        if (cpu_i2c_read_extended(0x4d, 0x58, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &val, 1)) {
            /* Error: sal_printf("sysSlotIdGet: Failed reading gfa-bi slot_id, Setting to 0\n"); */
            return -1;
        }
        return ((val & 0xf0) == 0x80) ? 0 : 1;
    } else if (sysBoardTypeGfaBi2Get()) {
        if (cpu_i2c_read_extended(0x4d, 0x58, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &val, 1)) {
            /* Error: sal_printf("sysSlotIdGet: Failed reading gfa-bi2 slot_id, Setting to 0\n"); */
            return -1;
        }
        return ((val & 0xc0) == 0x0)  ? 0 : 1;
    } else if (sysBoardTypeAradCardGet()) {
        /* Do not report an error if the I2C read fails */
        if (cpu_i2c_read_extended(0x20, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &val, 1)) {
            /* Error: sal_printf("sysSlotIdGet: Failed reading arad-card slot_id, Setting to 0\n"); */
            return -1;
        }
        return ((val & 0x06) == 0x0)  ? 0 : 1;
    } else {
        return -1;
    }
#endif /* __KERNEL__ */
    sal_printf("This function is unavailable in Kernel mode\n");
    return -1;
}
#endif /* (__DUNE_GTO_BCM_CPU__ || __DUNE_WRX_BCM_CPU__) && !PLISIM && BCM_PETRA_SUPPORT */

static void
gvar_init(void)
{
#ifndef NO_SAL_APPL

    char    path[256];

    /*
     * Path (default is current directory followed by home directory)
     */

    sal_strncpy(path, ". ", 3);
    sal_homedir_get(path + 2, sizeof (path) - 2);
    var_set("path", path, TRUE, FALSE);

#ifdef INCLUDE_TCL
    var_set("bootdir", bootdir, TRUE, FALSE);
#endif

#endif /* NO_SAL_APPL */

    /*
     * Boot flags
     */

    if (SAL_BOOT_QUICKTURN) {
    var_set_integer("quickturn", 1, 0, 0);
    }

    if (SAL_BOOT_PLISIM) {
    var_set_integer("plisim", 1, 0, 0);
    }

    if (SAL_BOOT_XGSSIM) {
    var_set_integer("xgssim", 1, 0, 0);
    }

#ifdef ADAPTER_SERVER_MODE
    var_set_integer("cmodel", 1, 0, 0);
#endif /* ADAPTER_SERVER_MODE */

    /*
     * Included packages
     */

#ifdef INCLUDE_TCL
    var_set_integer("tcl", 1, 0, 0);
#endif

#ifdef INCLUDE_I2C
    var_set_integer("i2c", 1, 0, 0);
#endif

#ifdef INCLUDE_MACSEC
    var_set_integer("macsec", 1, 0, 0);
#endif

#ifdef INCLUDE_FLEXE
    var_set_integer("flexe", 1, 0, 0);
#endif

    /*
     * Host type
     */

#ifdef UNIX
    var_set_integer("unix", 1, 0, 0);
#endif

#ifdef BMW
    var_set_integer("bmw", 1, 0, 0);
    var_set("cpu", "bmw", FALSE, FALSE);
    if (sysIsCFM()) {
        char tmp[8];
        sal_sprintf(tmp, "cfm%d", sysSlotIdGet());
        var_set_integer(tmp, 1, 0, 0);
        var_set_integer("slot", sysSlotIdGet(), 0, 0);
        var_set_integer("cfm", 1, 0, 0);
    }
#endif

#ifdef MOUSSE
    var_set_integer("mousse", 1, 0, 0);
    var_set("cpu", "mousse", FALSE, FALSE);
#endif

#ifdef IDTRP334
    var_set_integer("idtrp334", 1, 0, 0);
    var_set("cpu", "idtrp334", FALSE, FALSE);
#endif

#ifdef IDT438
    var_set_integer("idt438", 1, 0, 0);
    var_set("cpu", "idt438", FALSE, FALSE);
#endif

#ifdef NSX
    var_set_integer("nsx", 1, 0, 0);
    var_set("cpu", "nsx", FALSE, FALSE);
#endif

#ifdef METROCORE
    var_set_integer("metrocore", 1, 0, 0);
    var_set("cpu", "metrocore", FALSE, FALSE);
#endif

#ifdef BCM_ICS
    var_set_integer("ics", 1, 0, 0);
    var_set("cpu", "ics", FALSE, FALSE);
#endif

#ifdef WR8548
    var_set_integer("wrsbc8548", 1, 0, 0);
    var_set("cpu", "wrsbc8548", FALSE, FALSE);
#endif   

#ifdef GTO 
    var_set_integer("gto", 1, 0, 0);
    var_set("cpu", "gto", FALSE, FALSE);
#endif

#if ((defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)) && !defined(PLISIM) && defined(BCM_PETRA_SUPPORT))
    {
        int mng_cpu;
        int slot;
        int val = 0;

        if (eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE,
                        NEGEV_CHASSIS_EEPROM_BYTE_SIZE_BOARD_TYPE, &val)==0){
            slot = sysSlotIdGet();
            if (slot >= 0) {
                var_set_integer("slot", slot, 0, 0);
            }
            var_set_integer("board_type_GFA_BI", sysBoardTypeGfaBiGet(), 0, 0);
            var_set_integer("board_type_GFA_BI_2", sysBoardTypeGfaBi2Get(), 0, 0);

            /* managment card */
            mng_cpu = sys_board_type_mng_get();
            if (mng_cpu) {
                var_set_integer("mng_cpu", mng_cpu, 0, 0);
            }
        }
    }

#endif
#ifdef KEYSTONE 
    var_set_integer("keystone", 1, 0, 0);
    var_set("cpu", "keystone", FALSE, FALSE);
#endif

#ifdef MBZ
    var_set_integer("mbz", 1, 0, 0);
    var_set("cpu", "mbz", FALSE, FALSE);
    if (sysIsLM()) {
        char tmp[8];
        sal_sprintf(tmp, "lm%d", sysSlotIdGet());
        var_set_integer(tmp, 1, 0, 0);
        var_set_integer("lm", 1, 0, 0);
        var_set_integer("slot", sysSlotIdGet(), 0, 0);
    }
#endif
}

/*
 * Assertion routine with recovery for diag shell (see assert.h)
 */

void
_diag_assert(const char *expr, const char *file, int line)
{

#ifdef NO_SAL_APPL

    cli_out("ERROR: Assertion failed: (%s) at %s:%d\n", expr, file, line); 
    cli_out("DIAG SHELL HAS HALTED\n"); 
    for(;;); 

#else

    char        buf[80];

    cli_out("ERROR: Assertion failed: (%s) at %s:%d\n", expr, file, line);

#ifdef UNIX
    /*
     * You can have assertion failures call abort() when you're in GDB
     * by adding the following to your ~/.gdbinit: set environment GDB 1
     */

    if (getenv("GDB")) {
    abort();
    }
#endif


    if (sal_thread_self() != sal_thread_main_get()) {
#ifdef __KERNEL__
        sal_thread_exit(3);
#else
    sal_abort();
#endif
    }

#ifdef NO_CTRL_C
    sal_readline("Press Enter if you wish to continue anyway ",
                 buf, sizeof (buf), NULL);
#else
    /*
     * Allow user to continue or return to command prompt.
     */

    if (sal_readline("ERROR: Continue or quit (c/q)? ",
             buf, sizeof (buf), "q") == NULL ||
    toupper((int)buf[0]) != 'C') {
    sh_ctrl_c_take();
    } else
#endif
    cli_out("WARNING: Correct behavior no longer guaranteed\n");

#endif /* NO_SAL_APPL */
}

char cmd_assert_usage[] =
    "Parameters: None\n";

cmd_result_t
cmd_assert(int unit, args_t *a)
{

    sal_assert_set(_diag_assert);

    assert((1 == 2));

    return CMD_OK;
}


#ifdef INCLUDE_TELNET

int telnet_initted = 0;
extern int sal_telnet_active;

#define TELNET_SERVICE          23      /* telnet port number */

/*
 * Note: assumes console is ttys0, you can use "sh & vx devs" cmds to
 * show
 */

#define SYSLOG_CONSOLE(msg) {           \
    int _fd = open("/tyCo/0", O_RDWR, 0644);    \
    write(_fd, msg, sal_strlen(msg));       \
    close(_fd);                 \
}

extern BOOT_PARAMS  sysBootParams;  /* login parameters from boot line */


/*
 * Workaround to set the STDIO file descriptors for all tasks equal to
 * that of the calling task.
 */

#define TASKS_MAX       100

void
stdio_update(void)
{
    int         tid_list[TASKS_MAX];
    int         std_in, std_out, std_err;
    int         i, count;

    count = taskIdListGet(tid_list, TASKS_MAX);

    std_in = ioTaskStdGet(0, STD_IN);
    std_out = ioTaskStdGet(0, STD_OUT);
    std_err = ioTaskStdGet(0, STD_ERR);

    for (i = 0; i < count; i++) {
    ioTaskStdSet(tid_list[i], STD_IN, std_in);
    ioTaskStdSet(tid_list[i], STD_OUT, std_out);
    ioTaskStdSet(tid_list[i], STD_ERR, std_err);
    }
}

/*
 * When the telnet service has completed, resume shell
 * processing on last used unit.
 */
void
diag_shell_restart(void)
{
    SYSLOG_CONSOLE("Console unlocked\n");

    while (1) {
    sal_thread_main_set(sal_thread_self());
    stdio_update();
    sh_process(-1, "BCM", TRUE);
    sal_reboot();
    }
}

STATIC char *
rdiag_readline(char *buf, int buf_size, int echo_off)
{
    int ioOptions, savedOptions;
    int ptyFd = ioTaskStdGet(0,STD_IN);
    char *s;

    fflush(stdout);

    ioOptions = ioctl(ptyFd, FIOGETOPTIONS, 0);
    savedOptions = ioOptions;
    if (echo_off) {
    ioOptions &= ~OPT_ECHO;
    }
    ioOptions |= OPT_CRMOD;
    ioOptions &= ~OPT_TANDEM;
    ioOptions &= ~OPT_ABORT;    /* Will cause Interrupt even before login */

    (void)ioctl(ptyFd, FIOSETOPTIONS, ioOptions);
    s = fgets(buf, buf_size - 1, stdin);
    (void)ioctl(ptyFd, FIOSETOPTIONS, savedOptions);

    if (s == NULL) {
    return NULL;
    }

    buf[buf_size - 1] = 0;

    if ((s = sal_strchr(buf, '\n')) != NULL) {
    *s = 0;
    }

    return buf;
}

/*
 * When an inbound connection is established on either the onboard
 * ethernet or (if the socend driver is loaded) the front panel ports,
 * the telnet service will spawn this task.  This task will redirect
 * I/O to use the socket file descriptors, and then launch a new
 * diagnostics shell.  First, we query the user for username and
 * password and if this matches that set at the bootrom, it shuts down
 * the console while a remote connection is in progress by terminating
 * the first instance of the shell launched at startup.  It then
 * restarts the shell processing on the last attached unit.
 */
STATUS
rdiag_init(int interpConnArg,
       int slaveFd,
       int exitArg,
       int *disconnArg,
       char *msg)
{
    char user[BOOT_USR_LEN];
    char pass[BOOT_PASSWORD_LEN];

#ifdef MBZ
    if (sysIsLM()) {
    goto allow;
    }
#endif

    if (sysBootParams.usr == NULL ||
    sysBootParams.passwd == NULL) {
    SYSLOG_CONSOLE("NOTICE: username and password not set.\n");
    goto allow;
    }

    printf("Broadcom StrataSwitch SDK\n");
    /* Read and discard first line - telnet options */
    if (rdiag_readline(user, sizeof (user), FALSE) == NULL) {
    goto deny;
    }
    printf("\nlogin: ");
    if (rdiag_readline(user, sizeof (user), FALSE) == NULL) {
    goto deny;
    }
    printf("Password: ");
    if (rdiag_readline(pass, sizeof (pass), TRUE) == NULL) {
    goto deny;
    }

    if (!sal_strcmp(sysBootParams.usr, user) &&
    !sal_strcmp(sysBootParams.passwd, pass)) {
    SYSLOG_CONSOLE("Login succeeded for user ");
    SYSLOG_CONSOLE(user);
    SYSLOG_CONSOLE("\n");
    goto allow;
    }

 deny:
    printf("\nAccess denied.\n");
    fflush(stdout);

#if VX_VERSION == 69
    telnetdExit((void *)slaveFd);
#else
    telnetdExit(slaveFd);
#endif
    return OK;

 allow:
    sal_telnet_active = 1;
    SYSLOG_CONSOLE("Console locked by remote.\n");
    if (taskNameToId("bcmCLI") != ERROR) {
    taskDeleteForce(taskNameToId("bcmCLI"));
    }
    if (taskNameToId("tShell") != ERROR) {
    taskDeleteForce(taskNameToId("tShell"));
    }
    printf("\nBroadcom Command Monitor (BCM) service started.\n");
    sal_thread_main_set(sal_thread_self());
    stdio_update();
    sal_readline_init();
    sh_print_version(FALSE);
    sh_process(-1, "BCM", TRUE);
#if VX_VERSION == 69
    telnetdExit((void *)slaveFd);
#else
    telnetdExit(slaveFd);
#endif
    sal_readline_init();
    sal_thread_create("bcmCLI", 128 * 1024, SOC_CLI_THREAD_PRI,
              (VOIDFUNCPTR)diag_shell_restart, 0);

    return OK;
}

#endif /* INCLUDE_TELNET */

/*
 * Early initialization.
 */

void
diag_init(void)
{
    assert(sizeof (uint8) == 1);
    assert(sizeof (uint16) == 2);
    assert(sizeof (uint32) == 4);
    assert(sizeof (uint64) == 8);

    sal_assert_set(_diag_assert);

    sh_print_version(FALSE);

    init_symtab();

    sal_srand(1);   /* Seed random number generator: arbitrary value */

#ifdef INCLUDE_TCL
    sal_homedir_get(bootdir, sizeof (bootdir));
#endif

    gvar_init();

#ifdef INCLUDE_TELNET
    /*
     * Load telnet server for users who may connect
     * from remote.
     */
    if (!telnet_initted) {
    /* Initialize TELNET Server */
    telnetInit(1); /* we allow network connections,
            * but not concurrently
            */
    telnetCallAdd("rBCM",
              TELNET_SERVICE,
              (FUNCPTR)rdiag_init, 0,
              SOC_CLI_THREAD_PRI, 0, 128 * 1024);
    telnet_initted++;
    cli_out("TELNET service started on TCP port 23.\n");
    }
#endif /* INCLUDE_TELNET */

    sh_bg_init();
    DISPLAY_MEM ;

} /* end diag_init() */

/*
 * Function:
 *  parseEndOk
 * Purpose:
 *    Commonly used function used to parse up to the end of line.
 * Parameters:
 *    a     - arg type pointer
 *    pt    - parse table
 * Returns:
 *    TRUE -  success to parse
 *    FALSE - fail to parse
 * Notes:
 */

int
parseEndOk(args_t *a, parse_table_t *pt, cmd_result_t *retCode)
{
    if (!ARG_CNT(a)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(pt);
        parse_arg_eq_done(pt);
        *retCode = CMD_OK;
        return FALSE;
    }

    if (parse_arg_eq(a, pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(pt);
        *retCode = CMD_FAIL;
        return FALSE;
    }

    parse_arg_eq_done(pt);
    *retCode = CMD_OK;
    return TRUE;
}

#ifdef PLISIM
cmd_result_t
cmd_txen(int unit, args_t *a)
{
    if (SAL_BOOT_PLISIM) {
        if (!sh_check_attached(ARG_CMD(a), unit)) {
            return CMD_FAIL;
        }

        /* NOTE -- should take a portmap argument */

        if ((int)pli_tx_enable(unit, -1) < 0) {
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

cmd_result_t
cmd_shutd(int unit, args_t *a)
{
    if (SAL_BOOT_PLISIM) {
        if (!sh_check_attached(ARG_CMD(a), unit)) {
            return CMD_FAIL;
        }

        if ((int)pli_shutdown(unit) < 0) {
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

cmd_result_t
cmd_simstart(int unit, args_t *a)
{
    if (SAL_BOOT_PLISIM) {
        if (!sh_check_attached(ARG_CMD(a), unit)) {
            return CMD_FAIL;
        }

        if ((int)pli_sim_start(unit) < 0) {
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}
#endif /* PLISIM */

#ifdef INCLUDE_TCL
/*
 * Run a TCL shell or TCL script
 */

char **environ;

char tcl_usage[] =
    "Parameters: [args ...]]\n\t"
    "If argument is not given, starts a TCL shell.\n\t"
    "Otherwise, executes a TCL with specified arguments, which may\n\t"
    "start with the name of a TCL script to run.\n";

#define MAXENV      32

cmd_result_t
cmd_tcl(int unit, args_t *a)
{
    char        **argv, **envp, **envnext, *c;
    char        **environ_save;
    char        *bootdir_asst;
    int         argc;
    extern int      tclsh(int argc, char **argv);
    int         rv;
    char        *bootFlags;
    const char  *bootTxt = "SOC_BOOT_FLAGS=";
    char        *bootFlagsVal;
    char        *c3SimPort;
    const char  *c3SimTxt = "SOC_TARGET_PORT=";
    char        *c3SimPortVal;
    char        *c3SimRunLongSystemTests;
    const char  *c3SimRunLongSystemTestsTxt = "RUN_LONG_SYSTEM_TESTS=";
    char        *c3SimRunLongSystemTestsVal;

    argv = sal_alloc((1 + ARG_CNT(a)) * sizeof (char *), "tcl_argv");
    envp = sal_alloc((1 + MAXENV    ) * sizeof (char *), "tcl_envp");
    bootdir_asst = sal_alloc(sal_strlen(bootdir) + 10, "tcl_bootdir");
    bootFlags = sal_alloc(sal_strlen(bootTxt) + 12, "tcl_bootflags");
    c3SimPort = sal_alloc(sal_strlen(c3SimTxt) + 8, "tcl_simPort");
    c3SimRunLongSystemTests  = sal_alloc(
            sal_strlen(c3SimRunLongSystemTestsTxt) + 8,
            "tcl_runLongSysTests");

    if (argv == NULL || envp == NULL || bootdir_asst == NULL
	|| bootFlags == NULL || c3SimPort == NULL ||
    c3SimRunLongSystemTests == NULL) {
    cli_out("%s: Out of memory\n", ARG_CMD(a));
    rv = CMD_FAIL;
    goto done;
    }

    sal_sprintf(bootdir_asst, "BOOTDIR=%s", bootdir);

    envnext = envp;
    *envnext++ = "PATH=";
    *envnext++ = (SAL_BOOT_QUICKTURN ? "QUICKTURN=1" : "QUICKTURN=0");
    *envnext++ = (SAL_BOOT_PLISIM ? "PLISIM=1" : "PLISIM=0");
#ifdef UNIX
    *envnext++ = "UNIX=1";
#else
    *envnext++ = "UNIX=0";
#endif
    *envnext++ = bootdir_asst;

#ifdef ADAPTER_SERVER_MODE
    *envnext++ = "CMODEL=1";
#else
    *envnext++ = "CMODEL=0";
#endif /* ADAPTER_SERVER_MODE */

    bootFlagsVal = getenv("SOC_BOOT_FLAGS");
    if (bootFlagsVal != NULL)
      {
	sal_sprintf(bootFlags, "%s%s", bootTxt, bootFlagsVal);
	*envnext++ = bootFlags;
      }

    c3SimPortVal = getenv("SOC_TARGET_PORT");
    if (c3SimPortVal != NULL)
      {
	sal_sprintf(c3SimPort, "%s%s", c3SimTxt, c3SimPortVal);
	*envnext++ = c3SimPort;
      }
    c3SimPortVal = getenv("SOC_TARGET_PORT1");
    if (c3SimPortVal != NULL)
      {
	sal_sprintf(c3SimPort, "%s%s", "SOC_TARGET_PORT1=", c3SimPortVal);
	*envnext++ = c3SimPort;
      }
    c3SimRunLongSystemTestsVal = getenv("RUN_LONG_SYSTEM_TESTS");
    if (c3SimRunLongSystemTestsVal != NULL)
      {
        sal_sprintf(c3SimRunLongSystemTests, "%s%s",
            "RUN_LONG_SYSTEM_TESTS=", c3SimRunLongSystemTestsVal);
        *envnext++ = c3SimRunLongSystemTests;
      }
    *envnext = NULL;

    argc = 0;

    argv[argc++] = "./tcl";

    while ((c = ARG_GET(a)) != NULL) {
    argv[argc++] = c;
    }

    /*
     * Currently it is necessary to restart the environment on every TCL
     * invocation.  This is because TCL may globally re-assign "environ"
     * with memory it has allocated, which is freed when TCL exits.
     */

    environ_save = environ;
    environ = envp;

    rv = tclsh(argc, argv);
     
    /* Set console back to expected configuration. */

    environ = environ_save;

 done:
    if (envp != NULL) {
    sal_free(envp);
    }

    if (argv != NULL) {
    sal_free(argv);
    }

    if (bootdir_asst != NULL) {
    sal_free(bootdir_asst);
    }

    if (bootFlags != NULL) {
      sal_free(bootFlags);
    }

    if (c3SimPort != NULL) {
      sal_free(c3SimPort);
    }

    if (c3SimRunLongSystemTests != NULL) {
        sal_free(c3SimRunLongSystemTests);
    }


    return rv;
}
#endif /* INCLUDE_TCL */

#if defined(MOUSSE) || defined(BMW) || defined(GTO)
/*
 * System panic mode
 */

char panic_usage[] =
    "Parameters: [ExcMessage=on|off] [BackTrace=on|off] [Reboot=on|off]\n\t"
    "Sets OS BSP panic mode.\n";

cmd_result_t
cmd_panic(int unit, args_t *a)
{
    extern int      sysToMonitorExcMessage;
    extern int      sysToMonitorBacktrace;
    extern int      sysToMonitorReboot;
    parse_table_t   pt;

    COMPILER_REFERENCE(unit);

    if (ARG_CNT(a) == 0) {
    return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "ExcMessage",  PQ_BOOL|PQ_DFL, 0,
            &sysToMonitorExcMessage, NULL );
    parse_table_add(&pt, "BackTrace",   PQ_BOOL|PQ_DFL, 0,
            &sysToMonitorBacktrace, NULL );
    parse_table_add(&pt, "Reboot",  PQ_BOOL|PQ_DFL, 0,
            &sysToMonitorReboot, NULL );

    if (parse_arg_eq(a, &pt) < 0) {
    cli_out("%s: Error: Invalid option: %s\n", ARG_CMD(a),
            ARG_CUR(a) ? ARG_CUR(a) : "*");
    parse_arg_eq_done(&pt);
    return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    return CMD_OK;
}
#endif /* MOUSSE || BMW || GTO */

#ifdef INCLUDE_NFS
/* { */
/*
 * NFS Control (Currently not in use)
 */

/* Currently, this is not required since NFS is not included */
/* extern int _diag_nfs_mounts; */

char cmd_nfs_usage[] =
    "Parameters:\n\t"
    "\texports <host> - Query a host's exported filesystems\n"
    "\tauth <host> - Display authentication parameters\n"
    "\tauth <host> <uid> [<gid> [<auth_gid> ...]] - Set auth parameters\n"
    "\tmount <host>:<filesystem> <mountpoint> - Mount filesystem\n"
    "\tunmount <mountpoint> - Unmount filesystem\n"
    "\tmounts - Display currently mounted filesystems\n"
    "\tNote: raw IP addresses may not work; see 'host' command to use names\n";

cmd_result_t
cmd_nfs(int unit, args_t *a)
{
    char        *subcmd;
    char        *hostname, *mountpoint, *s;

    if ((subcmd = ARG_GET(a)) == NULL) {
    return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "exports")) {
    if ((hostname = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (nfsExportShow(hostname) != OK) {
        cli_out("nfsExportshow failed\n");
        return CMD_FAIL;
    }

    return CMD_OK;
    }

    if (!sal_strcasecmp(subcmd, "auth")) {
    int     uid, gid, ngids, i;
    int     gids[MAX_GRPS];

    if ((hostname = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if ((s = ARG_GET(a)) == NULL) {
        nfsAuthUnixGet(hostname, &uid, &gid, &ngids, gids);

        cli_out("Host:      %s\n", hostname);
        cli_out("UID:       %d\n", uid);
        cli_out("GID:       %d\n", gid);
        cli_out("Auth GIDs:");

        for (i = 0; i < ngids; i++) {
        cli_out(" %d", gids[i]);
        }

        cli_out("\n");
        return CMD_OK;
    }

    uid = parse_integer(s);

    if ((s = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    gid = parse_integer(s);

    ngids = 0;

    while ((s = ARG_GET(a)) != NULL) {
        gids[ngids++] = parse_integer(s);
    }

    nfsAuthUnixSet(hostname, uid, gid, ngids, gids);

    return CMD_OK;
    }

    if (!sal_strcasecmp(subcmd, "mount")) {
    char        *fs;

    if ((hostname = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if ((mountpoint = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if ((fs = sal_strchr(hostname, ':')) == NULL) {
        return CMD_USAGE;
    }

    *fs++ = 0;

    if (nfsMount(hostname, fs, mountpoint) != OK) {
        cli_out("nfsMount failed\n");
        return CMD_FAIL;
    }

    _diag_nfs_mounts++;

    return CMD_OK;
    }

    if (!sal_strcasecmp(subcmd, "unmount")) {
    if ((mountpoint = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (nfsUnmount(mountpoint) != OK) {
        cli_out("nfsUnmount failed\n");
        return CMD_FAIL;
    }

    _diag_nfs_mounts--;

    return CMD_OK;
    }

    if (!sal_strcasecmp(subcmd, "mounts")) {
    nfsDevShow();
    return CMD_OK;
    }

    return CMD_USAGE;
}
/* } */
#endif /* INCLUDE_NFS */

#ifdef INCLUDE_EDITLINE
/*
 * Regsfile completion routines for command line editor
 */

int diag_list_possib_unit = -1;

int
diag_list_possib(char *regpref, char ***avp)
{
#if  defined(BCM_ESW_SUPPORT)
    soc_reg_t reg;
    char **av;
#endif /* BCM_ESW_SUPPORT */
    int ac = 0;
    int unit = diag_list_possib_unit;
#if defined(BCM_ESW_SUPPORT)
#if !defined(SOC_NO_NAMES)
    soc_field_t fld;
#endif /* !SOC_NO_NAMES */
#if !defined(SOC_NO_NAMES) || !defined(SOC_NO_ALIAS)
    int len = sal_strlen(regpref);
#endif /* !SOC_NO_NAMES !SOC_NO_ALIAS */
#endif /* BCM_ESW_SUPPORT */

    COMPILER_REFERENCE(unit);

#if defined(BCM_ESW_SUPPORT)

    av = SAL_ALLOC((NUM_SOC_REG + NUM_SOC_REG + NUM_SOC_FIELD) *
           sizeof (char *));

    if (av == NULL) {
    return 0;
    }

    if (*regpref == 0 || unit < 0) {
    sal_free(av);
    return 0;
    }
#endif  /* BCM_ESW_SUPPORT */

#if defined(BCM_ESW_SUPPORT)
    {
        for (reg = 0; reg < (NUM_SOC_REG); reg++) {
            if (SOC_REG_IS_VALID(unit, reg)) { /* Check name and alias */
#if !defined(SOC_NO_NAMES)
                if (!sal_strncasecmp(regpref, soc_reg_name[reg], len)) {
                    av[ac++] = sal_strdup(soc_reg_name[reg]);
                }
#endif /* !SOC_NO_NAMES */
    
#if !defined(SOC_NO_ALIAS)
                if (soc_reg_alias[reg] != NULL &&
                    !sal_strncasecmp(regpref, soc_reg_alias[reg], len)) {
                    av[ac++] = sal_strdup(soc_reg_alias[reg]);
                }
#endif /* !defined(SOC_NO_ALIAS) */
            }
        }
#if !defined(SOC_NO_NAMES)
        for (fld = 0; fld < (NUM_SOC_FIELD); fld++) {
            if (sal_strncasecmp(regpref, soc_fieldnames[fld], len) == 0) {
                av[ac++] = sal_strdup(soc_fieldnames[fld]);
            }
        }
#endif /* !SOC_NO_NAMES */        
    }
#endif /* BCM_ESW_SUPPORT */

#if defined(BCM_ESW_SUPPORT)
    if (ac == 0) {
        sal_free(av);
    } else {
        *avp = av;
    }
#endif /* BCM_ESW_SUPPORT */

    return ac;
}

char *
diag_complete(char *regpref, int *unique)
{
    char *s;
    int i, j, len, upcase, pfxlen, pfxmin;
    char **av;
    int ac;

    if ((ac = diag_list_possib(regpref, &av)) == 0) {
    /*    coverity[leaked_storage : FALSE]    */
    return 0;
    }

    len = sal_strlen(regpref);
    pfxmin = 0;
    pfxlen = sal_strlen(av[0]);

    for (i = 1; i < ac; i++) {
    /* Find length of common prefix of all matches */
    for (j = 0; av[pfxmin][j] && av[i][j]; j++) {
        if (av[pfxmin][j] != av[i][j]) {
        break;
        }
    }

    if (j < pfxlen) {
        pfxmin = i;
        pfxlen = j;
    }
    }

    if (pfxlen < len) {
    sal_free(av);
    return 0;
    }

    /* Generate completion.  Mimic case of input string. */
    s = SAL_ALLOC(pfxlen - len + 1);

    if (s == NULL) {
    sal_free(av);
    return 0;
    }

    upcase = isupper((unsigned) regpref[len - 1]);

    for (i = 0; i < pfxlen - len; i++) {
    s[i] = upcase ? toupper((int)av[0][len + i]) : tolower((int)av[0][len + i]);
    }

    s[i] = 0;

    *unique = (ac == 1);

    for (i = 0; i < ac; i++) {
    sal_free(av[i]);
    }
    sal_free(av);

    return s;
}
#endif /* INCLUDE_EDITLINE */

int diag_user_var_unit = -1;

char *
diag_user_var_get(char *varname)
{
    /* feature lookup */
    if (strncmp(varname, "feature_", 8) == 0) {
        soc_feature_t   f;

        if (diag_user_var_unit < 0) {
            return NULL;
        }

        for (f = 0; f < soc_feature_count; f++) {
            if (sal_strcmp(varname + 8, soc_feature_name[f]) == 0) {
                if (soc_feature(diag_user_var_unit, f)) {
                    return "1";
                }
                return NULL;
            }
        }
        return NULL;
    }

#ifndef NO_SAL_APPL
    /* config and property lookup */
    if (diag_user_var_unit < 0) {
        return sal_config_get(varname);
    }
#endif

    return soc_property_get_str(diag_user_var_unit, varname);
}

static void
_display_current_shell_mode(int unit)
{
    char mode[5];

    mode[0] = '\0';

    switch (command_mode_get(unit)) {
        case ESW_CMD_MODE:
            sal_strncpy(mode ,"ESW", sizeof(mode));
            break;
        case DPP_CMD_MODE:
            sal_strncpy(mode ,"DPP", sizeof(mode));
            break;
        case DNX_CMD_MODE:
            sal_strncpy(mode ,"DNX", sizeof(mode));
            break;
        case DNXF_CMD_MODE:
            sal_strncpy(mode ,"DNXF", sizeof(mode));
            break;
        default:
            sal_strncpy(mode ,"None", sizeof(mode));
            break;
    }
    sal_printf("Current mode is now %s\n", mode);
}

char shell_mode_usage[] =
  "\n    mode <m> where m is ESW\n";

/*
 * Function:    if_mode
 * Purpose: Set shell mode to BCM
 * Returns: CMD_USAGE/CMD_FAIL/CMD_OK.
 */
cmd_result_t
cmd_mode(int unit, args_t *a)
{
    char *subcmd;
    int curr_mode;
    int new_mode;

    if (unit < 0) {
        sal_printf("Mode command failed. Invalid unit(-1) \n");
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) { /* Swap modes. */

        curr_mode = command_mode_get(unit);
        /*
         * Cycle through modes.
         *
         * The following loop will go through all modes at most once
         * because of the "new_mode != curr_mode" restriction below.
         */
        for (new_mode = curr_mode+1; new_mode != curr_mode; new_mode++) {
            if (new_mode > DNXF_CMD_MODE) {
                /*
                 * If the loop has gone beyond DNXF_CMD_MODE, which is
                 * the last element in include/appl/diag/diag.h::
                 * sh_cmd_mode_e, force it to be ESW_CMD_MODE,
                 * which is the first element in that enum.
                 */
                new_mode = ESW_CMD_MODE;
            }
            if (
                ((ESW_CMD_MODE == new_mode) && (!SOC_IS_ESW(unit)))) {
                /*
                 * This mode is unsupported on the current device.
                 * Let's try the next mode.
                 */
                continue;
            }
            command_mode_set(unit, new_mode);
            break;
        }
        
    } else {
        /* mode <m> */
        if ((sal_strcasecmp(subcmd, "ESW") == 0)) {
            if (!SOC_IS_ESW(unit)) {
                sal_printf("ESW Mode is not supported on non-ESW devices \n");
                return CMD_USAGE;
            }
            command_mode_set(unit, ESW_CMD_MODE);
        } else {
            return CMD_USAGE;
        }
    }

    _display_current_shell_mode(unit);
    return CMD_OK;
}

char shell_bcm_usage[] =
  "\n    bcm\n";

/*
 * Function:    cmd_mode_bcm
 * Purpose: Set shell mode to BCM
 * Returns: CMD_USAGE/CMD_FAIL/CMD_OK.
 */

cmd_result_t
cmd_mode_bcm(int unit, args_t *a)
{
    COMPILER_REFERENCE(a);

    if (unit < 0) {
        sal_printf("Mode set failed. Invalid unit(-1) \n");
        return CMD_FAIL;
    }

    if (SOC_IS_ESW(unit)) {
        command_mode_set(unit, ESW_CMD_MODE);
    } else {
        sal_printf("Unknown SOC device type\n");
        return CMD_FAIL;
    }
    _display_current_shell_mode(unit);

    return CMD_OK;
}

/*
 * API to allow overriding name of default RC script on a per-unit basis.
 * Must be called before diag_shell().  If NULL is used (default), then
 * default init script SOC_INIT_RC is used.  If the empty string is used,
 * no script will be loaded.
 */

#ifndef NO_SAL_APPL

void
diag_rc_set(int unit, const char *fname)
{
    assert(unit >= 0 && unit < SOC_MAX_NUM_DEVICES);
    if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        return;
    }

    if (soc_init_rc[unit] != NULL) {
    sal_free(soc_init_rc[unit]);
    soc_init_rc[unit] = NULL;
    }

    if (fname != NULL) {
    soc_init_rc[unit] = sal_strdup(fname);
    }
}

void
diag_rc_get(int unit, const char **fname)
{
    assert(unit >= 0 && unit < SOC_MAX_NUM_DEVICES);
    if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        return;
    }

    *fname = (soc_init_rc[unit] != NULL ?
          soc_init_rc[unit] :
          SOC_INIT_RC);
}

int
diag_rc_load(int unit)
{
    char    *script = (soc_init_rc[unit] != NULL ?
               soc_init_rc[unit] : SOC_INIT_RC);

    if (script[0] == 0) {
    return CMD_OK;
    } else {
    return sh_rcload_file(unit, NULL, script, FALSE);
    }
}
#endif /* NO_SAL_APPL */

/*
 * Diagnostics shell routine.
 */

#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
STATIC int
scache_read_dummy_func(int unit, uint8 *buf, int offset, int nbytes)
{
    return SOC_E_RESOURCE;
}

STATIC int
scache_write_dummy_func(int unit, uint8 *buf, int offset, int nbytes)
{
    return SOC_E_RESOURCE;
}
#endif /* BCM_ESW_SUPPORT || defined(BCM_SAND_SUPPORT) */
#endif /* BCM_WARM_BOOT_SUPPORT */

void
diag_sdk_init(void)
{
    uint32  flags;
#ifndef NO_SAL_APPL
    char    *script;
    int     no_rc_warning = 1; 
#endif
    int     i, unit; 
    int     rv = BCM_E_NONE;

#ifndef BCM_WARM_BOOT_API_TEST_AUTOSYNC
#ifndef BCM_WARM_BOOT_API_TEST
#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(BCM_LTSW_SUPPORT)
    int     warm_boot = FALSE;
#endif /* BCM_ESW_SUPPORT  || BCM_SAND_SUPPORT || BCM_LTSW_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_WARM_BOOT_API_TEST */
#else /* BCM_WARM_BOOT_API_TEST_AUTOSYNC */
    int     warm_boot = FALSE;
#endif /* BCM_WARM_BOOT_API_TEST_AUTOSYNC */

#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    int     stable_location = BCM_SWITCH_STABLE_NONE;
    uint32  stable_flags = 0;
    uint32  stable_size = 0;
    char    *stable_filename = NULL;
#endif /* BCM_ESW_SUPPORT  || BCM_SAND_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
#if defined(BCM_DFE_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    int easy_reload = FALSE;
#endif
#endif
#ifdef INCLUDE_CUSTOMER
    extern int custom_cmd(int unit);
#endif
    SHR_BPROF_STATS_DECL;

    sal_thread_main_set(sal_thread_self());

#if defined(INCLUDE_EDITLINE) 
    sal_readline_config(diag_complete, diag_list_possib);
#endif /* INCLUDE_EDITLINE */

    parse_user_var_get = diag_user_var_get;

    bslmgmt_init();

    diag_init();

    sysconf_init();

    /*
     * At boot time, probe for devices and attach the first one.
     * In PLISIM, this is not done; the probe and attach commands
     * must be given explicitly.
     */
    flags = sal_boot_flags_get();

    if (flags & BOOT_F_EARLY_DBG) {
        /* Enable BSL sources to print more debug messages. */
        bslmgmt_set(bslLayerShared, bslSourceScache, bslSeverityVerbose);
    }

#ifdef BCM_LTSW_SUPPORT
    if (flags & BOOT_F_QUICKTURN) {
        sysconf_ltsw_emulation_set(1);
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    if (flags & BOOT_F_WARM_BOOT) {
        sysconf_ltsw_warmboot_set(1);
    }
#endif
#endif

    if (!(flags & BOOT_F_NO_PROBE)) {
        SHR_BPROF_STATS_TIME(SHR_BPROF_SOC_PROBE) {
#ifdef BCM_LTSW_SUPPORT
            if ((sysconf_ltsw_probe()) < 0) {
                cli_out("ERROR: PCI LTSW device probe failed\n");
            }
#endif /* BCM_LTSW_SUPPORT */
            if ((sysconf_probe()) < 0) {
                cli_out("ERROR: PCI SOC device probe failed\n");
            }
        }

        var_set_integer("units", soc_ndev, FALSE, FALSE);

#ifdef BCM_LTSW_SUPPORT
        if ((sysconf_ltsw_ha_init()) < 0) {
            cli_out("ERROR: HA initialization failed.\n");
        }
        if ((sysconf_ltsw_issu_upgrade_start()) < 0) {
            cli_out("ERROR: Starting ISSU upgrade failed.\n");
        }
        if ((sysconf_ltsw_config_init()) < 0) {
            cli_out("ERROR: Config initialization failed.\n");
        }
        if ((sysconf_ltsw_core_start()) < 0) {
            cli_out("ERROR: ltsw device core start failed\n");
        }
        if ((sysconf_ltsw_keep_netif_set(1)) < 0) {
            cli_out("ERROR: ltsw keep netif set failed\n");
        }
#endif /* BCM_LTSW_SUPPORT */

        if (!(flags & BOOT_F_NO_ATTACH)) {
            for (i = 0; i < soc_all_ndev; i++) {
                unit = SOC_NDEV_IDX2DEV(i);
                SHR_BPROF_STATS_TIME(SHR_BPROF_SOC_ATTACH) {
                    /* coverity[stack_use_callee] */
                    /* coverity[stack_use_overflow] */
                    rv = sysconf_attach(unit);
                }
#if defined(BCM_ESW_SUPPORT)
                if (rv == 0) {
                    if (SOC_IS_ESW(unit)) {
                        mem_watch_counter_init(unit);
                        mem_watch_start(unit);
                    }
                }
#endif /* BCM_ESW_SUPPORT */
                if (rv < 0) {
                    cli_out("ERROR: SOC unit %d attach failed\n", unit);
                }
#ifdef BCM_LTSW_SUPPORT
                else if (SOC_IS_LTSW(i)) {
#ifndef NO_SAL_APPL
                    diag_rc_set(i, "ltsw.soc");
#endif
                }
#endif /* BCM_LTSW_SUPPORT */
            } /* for */
        } else {
        cli_out("Boot flags: Attach NOT performed\n");
    }
    } else {
        cli_out("Boot flags: Probe NOT performed\n");
    }

    cmdlist_init();

#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
#if defined(BCM_DFE_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    if (flags & BOOT_F_RELOAD) {
      cli_out("Boot flags: Easy Reload\n");
      easy_reload = TRUE;

      for (i = 0; i < soc_ndev; i++) {
        SOC_RELOAD_MODE_SET(SOC_NDEV_IDX2DEV(i), TRUE);        
      }
    } else {
      cli_out("Boot flags: regular load\n");
      for (i = 0; i < soc_ndev; i++) {
        SOC_RELOAD_MODE_SET(SOC_NDEV_IDX2DEV(i), FALSE);
      }
    }
#endif
#endif


#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(BCM_LTSW_SUPPORT)

    /* For Fastboot, use warmboot, but cold boot most modules except
     * pcix, cmic, port, serdes */
    if (flags & BOOT_F_FAST_REBOOT) {
        for (i = 0; i < soc_ndev; i++) {
/* For now only TH3 supports Fast Reboot, so only allow TH3 to set flags */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(SOC_NDEV_IDX2DEV(i))) {
                SOC_FAST_REBOOT_START(SOC_NDEV_IDX2DEV(i));
#ifndef NO_SAL_APPL
                diag_rc_set(SOC_NDEV_IDX2DEV(i), "freboot.soc");
#endif
                flags |= BOOT_F_WARM_BOOT;
            }
#endif
        }
        if (flags & BOOT_F_WARM_BOOT) {
            cli_out("Boot flags: Fast reboot\n");
        }
    }

    if (flags & BOOT_F_WARM_BOOT) {
        cli_out("Boot flags: Warm boot\n");

#ifndef BCM_WARM_BOOT_API_TEST_AUTOSYNC
#ifndef BCM_WARM_BOOT_API_TEST
#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(BCM_LTSW_SUPPORT)
        warm_boot = TRUE;
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT || BCM_LTSW_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_WARM_BOOT_API_TEST */
#endif /* BCM_WARM_BOOT_API_TEST_AUTOSYNC */

        for (i = 0; i < soc_ndev; i++) {
            SOC_WARM_BOOT_START(SOC_NDEV_IDX2DEV(i));
#ifdef BCM_LTSW_SUPPORT
            if (SOC_IS_LTSW(SOC_NDEV_IDX2DEV(i))) {
                bcm_warmboot_set(SOC_NDEV_IDX2DEV(i), 1);
#ifndef NO_SAL_APPL
                diag_rc_set(SOC_NDEV_IDX2DEV(i), "reload_ltsw.soc");
#endif
            }
#endif /* BCM_LTSW_SUPPORT */
#ifndef NO_SAL_APPL
            if (!SOC_FAST_REBOOT(SOC_NDEV_IDX2DEV(i)) &&
                !SOC_IS_LTSW(SOC_NDEV_IDX2DEV(i))) {
                diag_rc_set(SOC_NDEV_IDX2DEV(i), "reload.soc");
            }
#endif
        }
    } else {
        cli_out("Boot flags: Cold boot\n");
        for (i = 0; i < soc_ndev; i++) {
            SOC_WARM_BOOT_DONE(SOC_NDEV_IDX2DEV(i));
            SOC_FAST_REBOOT_DONE(SOC_NDEV_IDX2DEV(i));
#ifdef BCM_LTSW_SUPPORT
            if (SOC_IS_LTSW(SOC_NDEV_IDX2DEV(i))) {
                bcm_warmboot_set(SOC_NDEV_IDX2DEV(i), 0);
            }
#endif /* BCM_LTSW_SUPPORT */
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_LTSW_SUPPORT
    if ((sysconf_ltsw_ha_state_comp()) < 0) {
        cli_out("ERROR: HA state comparison failed.\n");
    }
#endif /* BCM_LTSW_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    for (i = 0; i < soc_ndev; i++) {

        /* LTSW devices will not use scache for warmboot. */
        if (SOC_IS_LTSW(SOC_NDEV_IDX2DEV(i))) {
            continue;
        }

        if (!soc_attached(SOC_NDEV_IDX2DEV(i))) continue;
        stable_filename = soc_property_get_str(SOC_NDEV_IDX2DEV(i), spn_STABLE_FILENAME);

        if ((NULL == stable_filename) &&
                   (stable_filename =
                    soc_property_get_str(SOC_NDEV_IDX2DEV(i), "scache_filename"))) {
            stable_location = _SHR_SWITCH_STABLE_APPLICATION;
            stable_flags = 0;
            stable_size = SOC_DEFAULT_LVL2_STABLE_SIZE;
            if ((SOC_CONTROL(SOC_NDEV_IDX2DEV(i)) != NULL) &&
                soc_feature(SOC_NDEV_IDX2DEV(i), soc_feature_ser_parity) &&
                soc_property_get(SOC_NDEV_IDX2DEV(i), "memcache_in_scache", 0)) {
                if (SOC_IS_TD2_TT2(SOC_NDEV_IDX2DEV(i))) {
                    uint16 dev_id;
                    uint8  rev_id;
                    soc_cm_get_id(SOC_NDEV_IDX2DEV(i), &dev_id, &rev_id);
                    if (dev_id == BCM56830_DEVICE_ID) {
                        stable_size *= 20;
                    } else {
                        stable_size *= 12;
                    }
                } else if (SOC_IS_TRIUMPH3(SOC_NDEV_IDX2DEV(i))) {
                    stable_size *= 30;
                } else if (SOC_IS_TD_TT(SOC_NDEV_IDX2DEV(i))) {
                    stable_size *= 14;
                } else {
                    stable_size *= 16;
                }
            }

            if ((SOC_CONTROL(SOC_NDEV_IDX2DEV(i)) != NULL) &&
               SOC_IS_TOMAHAWKX(SOC_NDEV_IDX2DEV(i))) {
               stable_size = 85 * 1024 * 1024;
            }

            if ((SOC_CONTROL(SOC_NDEV_IDX2DEV(i)) != NULL) &&
               SOC_IS_TRIDENT3X(SOC_NDEV_IDX2DEV(i))) {
               stable_size += 50 * 1024 * 1024;
               if (SOC_IS_HELIX5(SOC_NDEV_IDX2DEV(i)) ||
                   SOC_IS_HURRICANE4(SOC_NDEV_IDX2DEV(i))) {
               /* Add additional storage space for HX5 Broadscan */
               stable_size += 5 * 1024 * 1024;
               }
            }

            /* Recently increased field scache size to push more data. VERSION_1_15 */
            if ((SOC_CONTROL(SOC_NDEV_IDX2DEV(i)) != NULL) &&
               SOC_IS_TD_TT(SOC_NDEV_IDX2DEV(i))) {

                if (SOC_IS_APACHE(SOC_NDEV_IDX2DEV(i))) {
                    /* Default scache size 7MB is needed for apache family */
                    stable_size += 4 * 1024 * 1024;
                } else if (SOC_IS_TRIDENT2X(SOC_NDEV_IDX2DEV(i))) {
                    /* Default scache size 16MB is needed for trident2x */
                    stable_size += 13 * 1024 * 1024;
                } else {
                    stable_size += 135 * 1024;
                }

            }

            if ((SOC_CONTROL(SOC_NDEV_IDX2DEV(i)) != NULL) &&
                SOC_IS_TRIUMPH3(SOC_NDEV_IDX2DEV(i))) {
                stable_size += 2 * 1024 * 1024;
            }

            /* Default scache size 6MB is needed */
            if ((SOC_CONTROL(SOC_NDEV_IDX2DEV(i)) != NULL) &&
                SOC_IS_GREYHOUND2(SOC_NDEV_IDX2DEV(i))) {
                stable_size += 3 * 1024 * 1024;
            }
        }

        stable_location = soc_property_get(SOC_NDEV_IDX2DEV(i), spn_STABLE_LOCATION,
                                           stable_location);

#if (!defined(BCM_PETRA_SUPPORT) && !defined(BCM_DNX_SUPPORT)) || defined(__KERNEL__) || (!defined (LINUX) && !defined(UNIX))
        if (stable_location == _SHR_SWITCH_STABLE_SHARED_MEM) {
            stable_location =  _SHR_SWITCH_STABLE_APPLICATION;
            cli_out(
               "Unit %d: soc property stable_location=4 is only supported with\n"
			   "    (BCM_PETRA_SUPPORT || BCM_DNX_SUPPORT) && !__KERNEL__ && (LINUX || UNIX) compilation\n "
			   "    falling back to stable_location=3\n",
               SOC_NDEV_IDX2DEV(i));
        }
#endif

        if (BCM_SWITCH_STABLE_NONE != stable_location) {
            /* Otherwise, nothing to do */
            stable_flags = soc_property_get(SOC_NDEV_IDX2DEV(i), spn_STABLE_FLAGS,
                                            stable_flags);
            stable_size = soc_property_get(SOC_NDEV_IDX2DEV(i), spn_STABLE_SIZE,
                                           stable_size);

            if ((BCM_SWITCH_STABLE_APPLICATION == stable_location) &&
                (NULL != stable_filename)) {
#if (defined(LINUX) && !defined(__KERNEL__)) || defined(UNIX)
                /* Try to open the scache file */
                if (appl_scache_file_open(SOC_NDEV_IDX2DEV(i), (flags & BOOT_F_WARM_BOOT) ?
                                          TRUE : FALSE,
                                          stable_filename) < 0) {
                    cli_out("Unit %d: stable cache file not %s\n", SOC_NDEV_IDX2DEV(i),
                            (flags & BOOT_F_WARM_BOOT) ?
                            "recovered" : "created");

                    /* Fall back to Level 1 Warm Boot */
                    stable_location = BCM_SWITCH_STABLE_NONE;
                    stable_size = 0;
                }

#else /* (defined(LINUX) && !defined(__KERNEL__)) || defined(UNIX) */
                cli_out("Build-in stable cache file not supported in this configuration\n");
                /* Use Level 1 Warm Boot instead*/
                stable_location = BCM_SWITCH_STABLE_NONE;
                stable_size = 0;
#endif /* (defined(LINUX) && !defined(__KERNEL__)) || defined(UNIX) */
           }
#ifdef BCM_PETRA_SUPPORT
           else if (BCM_SWITCH_STABLE_DEVICE_EXT_MEM == stable_location) {
               if (appl_scache_user_buffer(SOC_NDEV_IDX2DEV(i))) {
                   cli_out("Unit %d: error when initializing user buffer", SOC_NDEV_IDX2DEV(i));

                   /* Fall back to Level 1 Warm Boot */
                   stable_location = BCM_SWITCH_STABLE_NONE;
                   stable_size = 0;
               }
            }
#endif /* BCM_PETRA_SUPPORT */

            if (soc_stable_set(SOC_NDEV_IDX2DEV(i), stable_location, stable_flags) < 0) {
                cli_out("Unit %d: soc_stable_set failure\n", SOC_NDEV_IDX2DEV(i));
            } else if (soc_stable_size_set(SOC_NDEV_IDX2DEV(i), stable_size) < 0) {
                cli_out("Unit %d: soc_stable_size_set failure\n", SOC_NDEV_IDX2DEV(i));
            }

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
            /* Verify that provided scache is from an allowed version */
            if (flags & BOOT_F_WARM_BOOT &&
                (SOC_IS_ARAD(SOC_NDEV_IDX2DEV(i)) ||
                     SOC_IS_DNX(SOC_NDEV_IDX2DEV(i)) ||
                     SOC_IS_DNXF(SOC_NDEV_IDX2DEV(i)))) {
                if (soc_stable_size_set(SOC_NDEV_IDX2DEV(i), stable_size)) {
                    cli_out("Unit %d: soc_stable_size_set failed. stable_size=%d\n", SOC_NDEV_IDX2DEV(i), stable_size);
                }                    

                if (soc_scache_recover(SOC_NDEV_IDX2DEV(i))) {
                    cli_out("Unit %d: soc_scache_recover failed\n", SOC_NDEV_IDX2DEV(i));
                }
            }
#endif /* defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

            if (1 == soc_property_get(SOC_NDEV_IDX2DEV(i),
                        spn_WARMBOOT_EVENT_HANDLER_ENABLE, 0)) {
                if (soc_event_register(SOC_NDEV_IDX2DEV(i),
                        appl_warm_boot_event_handler_cb, NULL) < 0) {
                    cli_out("Unit %d: soc_event_register failure\n", SOC_NDEV_IDX2DEV(i));
                }
            }
        } else {

            
            /* EMPTY SCACHE INITIALIZATION ->
               in case stable_* parameters are not defined in configuration file, 
               initiating scache with size 0(zero). in order that scache commits 
               wont fail and cause application exit upon startup */
            if (soc_switch_stable_register(SOC_NDEV_IDX2DEV(i),
                                           &scache_read_dummy_func,
                                           &scache_write_dummy_func,
                                           NULL, NULL) < 0) {
                cli_out("Unit %d: soc_switch_stable_register failure\n", SOC_NDEV_IDX2DEV(i));
            }

            stable_location = BCM_SWITCH_STABLE_NONE;
            stable_size     = 0;
            stable_flags    = 0;

            if (soc_stable_set(SOC_NDEV_IDX2DEV(i), stable_location, stable_flags) < 0) {
                cli_out("Unit %d: soc_stable_set failure\n", SOC_NDEV_IDX2DEV(i));
            } else if (soc_stable_size_set(SOC_NDEV_IDX2DEV(i), stable_size) < 0) {
                cli_out("Unit %d: soc_stable_size_set failure\n", SOC_NDEV_IDX2DEV(i));
            }
            /* <- EMPTY SCACHE INITIALIZATION */

            /* Treat all virtual ports as MIM for Level 1 */
            SOC_WARM_BOOT_MIM(SOC_NDEV_IDX2DEV(i));
        }
    }
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT || BCM_LTSW_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_DNX_SUPPORT
#ifdef ADAPTER_SERVER_MODE
    dnx_adapter_init_lock = sal_mutex_create("dnx_adapter_init_lock");
    if (!dnx_adapter_init_lock)
    {
        cli_out("unable to create dnx_adapter_init_lock\n");
    }
#endif
#endif

#if defined(BCM_SAND_SUPPORT)
    for (i = 0; i < soc_ndev; i++)
    {
        if (SOC_CONTROL(SOC_NDEV_IDX2DEV(i)) && SOC_IS_SAND(SOC_NDEV_IDX2DEV(i)))
        {
            if (bsldnx_mgmt_init(SOC_NDEV_IDX2DEV(i)) < 0)
            {
                cli_out("Unit %d: bslmgmt_dnx_init failure\n", SOC_NDEV_IDX2DEV(i));
            }
        }
    }
#endif

#ifndef NO_SAL_APPL
    /* Add backdoor for mem tuner to update system configuration */
    soc_mem_config_set = sal_config_set;

    /*
     * If a startup script is given in the boot parameters, attempt to
     * load it.  This script is for general system configurations such
     * as host table additions and NFS mounts.
     */

    if ((script = sal_boot_script()) != NULL) {
        if (sh_rcload_file(-1, NULL, script, FALSE) != CMD_OK) {
            cli_out("ERROR loading boot init script: %s\n", script);
        }
        
        no_rc_warning = 0;
     }
    
    /*
     * If a default init file is given, attempt to load it.
     */
    if (!(flags & BOOT_F_NO_RC)) {
        for (i = 0; i < soc_ndev; i++) {
            if (soc_attached(SOC_NDEV_IDX2DEV(i))) {
                sh_swap_unit_vars(SOC_NDEV_IDX2DEV(i));
                if (SOC_IS_RCPU_ONLY(SOC_NDEV_IDX2DEV(i))) {
                    /* Wait for master unit to establish link */
                    sal_sleep(3);
                }
                if (diag_rc_load(SOC_NDEV_IDX2DEV(i)) != CMD_OK) {
                    cli_out("ERROR loading rc script on unit %d\n", SOC_NDEV_IDX2DEV(i));
                }
            }
        }
    } else if (no_rc_warning) {
        cli_out("Boot flags: initialization scripts NOT loaded\n");
    }

    if (soc_ndev <= 0) {
        cli_out("No attached units.\n");
    }

#endif /* NO_SAL_APPL */

#ifndef BCM_WARM_BOOT_API_TEST_AUTOSYNC
#ifndef BCM_WARM_BOOT_API_TEST
#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(BCM_LTSW_SUPPORT)
    if (warm_boot) {
#ifdef BCM_LTSW_SUPPORT
        if ((sysconf_ltsw_issu_upgrade_done()) < 0) {
            cli_out("ERROR: ISSU upgrade failed.\n");
        }
        sysconf_ltsw_warmboot_set(0);
#endif /* BCM_LTSW_SUPPORT */
        /* Warm boot is done, clear reloading state */
        for (i = 0; i < soc_ndev; i++) {
            SOC_WARM_BOOT_DONE(SOC_NDEV_IDX2DEV(i));
#ifdef BCM_LTSW_SUPPORT
            if (SOC_IS_LTSW(SOC_NDEV_IDX2DEV(i))) {
                bcm_warmboot_set(SOC_NDEV_IDX2DEV(i), 0);
            }
#endif /* BCM_LTSW_SUPPORT */
        }
    }
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT || BCM_LTSW_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_WARM_BOOT_API_TEST */
#else
    /* If "BCM_WARM_BOOT_API_TEST_AUTOSYNC" is defined, "warm_boot" in this function
     * will not be referenced, which leads to compilation warnings. Add a dummy
     * referencing code below to suppress it.
     */
    if(warm_boot){}
#endif /* BCM_WARM_BOOT_API_TEST_AUTOSYNC */

#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
#if defined(BCM_DFE_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    if (easy_reload) {
        for (i = 0; i < soc_ndev; i++) 
        {
            /* reload is done, clear reloading state, if user wants to call apis
               without affecting the Hw its his responsibility to set the reload mode back to true */
            SOC_RELOAD_MODE_SET(SOC_NDEV_IDX2DEV(i), FALSE);        
        }    
    }
#endif
#endif

#ifdef BCM_DIAG_SHELL_CUSTOM_INIT_F
    {
        /* Call custom init function prior to entering input loop */
        extern int BCM_DIAG_SHELL_CUSTOM_INIT_F (void); 
        BCM_DIAG_SHELL_CUSTOM_INIT_F (); 
    }      
#endif /* BCM_DIAG_SHELL_CUSTOM_INIT_F */

#if (defined(INCLUDE_CUSTOMER) && !defined(__KERNEL__))
    /* Add custom commands */
    for (i = 0; i < soc_ndev; i++)
    {
        (void) custom_cmd(SOC_NDEV_IDX2DEV(i));
    }
#endif

    DISPLAY_MEM ;
    DISPLAY_SW_STATE_MEM ;
    DISPLAY_MEM_PRINTF(("%s(): Just before BCM shell\r\n",__func__)) ;

}

void diag_cli_run(void) 
{

    while (1) {
#if !defined(IN_CODE_SUBSTITUTE) && !defined(SKIP_BCM_SHELL_PROMPT)
    sh_process(-1, "BCM", TRUE);
#endif
#ifdef BCM_DNX_SUPPORT
#ifdef ADAPTER_SERVER_MODE
    /** destory the mutex after process done. It was created in diag_sdk_init because process may run from that step also */
    if (dnx_adapter_init_lock != NULL)
    {
        sal_mutex_destroy(dnx_adapter_init_lock);
        dnx_adapter_init_lock = NULL;
    }
#endif
#endif

#ifdef NO_SAL_APPL
    return;
#else
#ifdef __KERNEL__
    cli_out("Exit Diag Shell.\n");
    return; 
#endif

#if !defined(IN_CODE_SUBSTITUTE) && !defined(SKIP_BCM_SHELL_PROMPT)
    sal_reboot();
#else
    return;
#endif

#endif
    }
}

void diag_shell(void) 
{
    diag_sdk_init();
    diag_cli_run();
}
#define SYSTEM_INIT_CHECK(action, description)          \
    if ((rv = (action)) < 0) {              \
        msg = (description);                \
        goto done;                      \
    }

#if defined(BCM_LTSW_SUPPORT)

static int
system_ltsw_init(int unit)
{
    int rv = 0;
    char *msg = NULL;
    int port;
    sal_usecs_t usec;
    pbmp_t      pbmp, hg_pbmp;
    bcm_port_config_t pcfg;

    if (ltsw_pw_running(unit)) {
        SYSTEM_INIT_CHECK(ltsw_pw_clean(unit), "PacketWatcher clean");
    }

    SYSTEM_INIT_CHECK(sysconf_ltsw_dev_init(unit), "Device init");

    sysconf_ltsw_dev_env_set(unit);

    SYSTEM_INIT_CHECK(bcm_init(unit), "BCM driver layer init");

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &pcfg));

    usec = BCM_LINKSCAN_INTERVAL_DEFAULT;
    SYSTEM_INIT_CHECK(bcm_linkscan_enable_set(unit, usec), "Linkscan enable");

    BCM_PBMP_ASSIGN(pbmp, pcfg.port);
    BCM_PBMP_ASSIGN(hg_pbmp, pcfg.hg);

    BCM_PBMP_ITER(pbmp, port) {
        int autoneg, pause_tx, pause_rx;
        SYSTEM_INIT_CHECK(bcm_linkscan_mode_set(unit, port,
                                                BCM_LINKSCAN_MODE_SW),
                          "Linkscan mode set");
        autoneg = FALSE;
        SYSTEM_INIT_CHECK(bcm_port_autoneg_set(unit, port, autoneg),
                          autoneg ? "Autoneg enable" : "Autoneg disable");
        if (BCM_PBMP_MEMBER(hg_pbmp, port)) {
            pause_tx = pause_rx = FALSE;
        } else {
            pause_tx = pause_rx = TRUE;
        }
        SYSTEM_INIT_CHECK(bcm_port_pause_set(unit, port, pause_tx, pause_rx),
                          pause_tx ? "Pause enable" : "Pause disable");
        SYSTEM_INIT_CHECK(bcm_port_stp_set(unit, port, BCM_PORT_STP_FORWARD),
                          "Port Forwarding");
        SYSTEM_INIT_CHECK(bcm_stat_clear(unit, port),
                          "Stat clear");
    }

    if (!SAL_BOOT_XGSSIM) {
        SYSTEM_INIT_CHECK(ltsw_do_resync_counters(unit, pbmp),
                          "Resync counters");
    }

done:
    if (msg != NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit ,
                              "system_init: %s failed: %s\n"),
                   msg, soc_errmsg(rv)));
    }

    if (BCM_CONTROL(unit)) {
        BCM_CONTROL(unit)->system_initialized_rv = rv;
    }

    return BCM_E_NONE;
}

#endif /* BCM_LTSW_SUPPORT */

/*
 * Function:
 *  system_init
 * Purpose:
 *  Reset the switch chip, re-initialize, and initialize the BCM API layer.
 * Parameters:
 *  unit - StrataSwitch unit #.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  After initialization, ports are in the spanning tree disabled
 *  state.  Some STP implementation needs to turn them on.
 */

int
system_init(int unit)
{


    int         rv=0;
    char        *msg = NULL;
#if defined(BCM_ESW_SUPPORT) 
    soc_port_t      port, dport;
    sal_usecs_t     usec;
    pbmp_t      pbmp;
    bcm_port_config_t pcfg;
#endif

#if defined(BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        return system_ltsw_init(unit);
    }
#endif /* BCM_LTSW_SUPPORT */

    {
#if defined(BCM_ESW_SUPPORT)
        SYSTEM_INIT_CHECK(soc_reset_init(unit), "Device reset");
#if defined(CANCUN_SUPPORT) && !defined(NO_SAL_APPL)
        if (soc_feature(unit, soc_feature_cancun)) {
            SYSTEM_INIT_CHECK(soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CMH), "load cmh");
            SYSTEM_INIT_CHECK(soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CCH), "load cch");
            SYSTEM_INIT_CHECK(soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CEH), "load ceh");
        }
#endif
        SYSTEM_INIT_CHECK(soc_misc_init(unit), "Misc init");
#if defined(CANCUN_SUPPORT) && !defined(NO_SAL_APPL)
        if (soc_feature(unit, soc_feature_cancun)) {
            SYSTEM_INIT_CHECK(soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CIH), "load cih");
            if (soc_feature(unit, soc_feature_flex_flow)) {
                SYSTEM_INIT_CHECK(soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CFH), "load cfh");
            }
        }
#endif
        SYSTEM_INIT_CHECK(soc_mmu_init(unit), "MMU init");

#ifdef  BCM_XGS_SWITCH_SUPPORT
        if (soc_feature(unit, soc_feature_arl_hashed) && !SOC_IS_RCPU_ONLY(unit)) {
            usec = soc_property_get(unit, spn_L2XMSG_THREAD_USEC, SOC_L2X_INTERVAL_DEFAULT);
            rv = soc_l2x_start(unit, 0, usec);
            if ((rv < 0) && (rv != SOC_E_UNAVAIL)) {
                msg = "L2X thread init";
                goto done;
            }
        }
#endif  /* BCM_XGS_SWITCH_SUPPORT */
#endif
    }
#if defined(BCM_ESW_SUPPORT) 
    SYSTEM_INIT_CHECK(bcm_init(unit), "BCM driver layer init");

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &pcfg));

    if (soc_property_get_str(unit, spn_BCM_LINKSCAN_PBMP) == NULL) {
        BCM_PBMP_ASSIGN(pbmp, pcfg.port);
    } else {
        pbmp = soc_property_get_pbmp(unit, spn_BCM_LINKSCAN_PBMP, 0);
    }

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        int autoneg;

        autoneg = soc_property_bcm_port_get(unit, port, 
                                            spn_PORT_INIT_AUTONEG, TRUE);

        SYSTEM_INIT_CHECK(bcm_port_stp_set(unit, port, BCM_PORT_STP_FORWARD),
                          "Port Forwarding");
#if defined(BCM_RCPU_SUPPORT)
        if (SOC_IS_RCPU_ONLY(unit) && !IS_RCPU_PORT(unit, port))
#endif /* BCM_RCPU_SUPPORT */
        {
            SYSTEM_INIT_CHECK(bcm_port_autoneg_set(unit, port, autoneg),
                              autoneg ? "Autoneg enable" : "Autoneg disable");
        }
            SYSTEM_INIT_CHECK(bcm_linkscan_mode_set(unit, port,
                                                SOC_IS_XGS12_FABRIC(unit) ? 
                                                BCM_LINKSCAN_MODE_HW :
                                                BCM_LINKSCAN_MODE_SW),
                          "Linkscan mode set");
        if (!soc_feature(unit, soc_feature_no_stat_mib)) {
            SYSTEM_INIT_CHECK(bcm_stat_clear(unit, port),
                              "Stat clear");
        }
    }

    usec = soc_property_get(unit, spn_BCM_LINKSCAN_INTERVAL,
                            BCM_LINKSCAN_INTERVAL_DEFAULT);

    SYSTEM_INIT_CHECK(bcm_linkscan_enable_set(unit, usec), "Linkscan enable");
#if defined(BCM_ESW_SUPPORT)
    if (SOC_IS_ESW(unit) && ((!SAL_BOOT_SIMULATION) || SAL_BOOT_BCMSIM)) {
        SYSTEM_INIT_CHECK(soc_counter_sync(unit), "Counter sync");
        SYSTEM_INIT_CHECK(do_resync_counters(unit, pbmp), "Resync counters");
    }
#endif
#endif

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_PKTIO)
    if (soc_feature(unit, soc_feature_sdklt_pktio)) {
        diag_pktio_mode_enable(unit);
    } else {
        diag_pktio_mode_disable(unit);
    }
#endif

#if defined(BCM_ESW_SUPPORT)
 done:
#endif

    if (msg != NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit ,
                              "system_init: %s failed: %s\n"),
                   msg, soc_errmsg(rv)));
    }

    if (BCM_CONTROL(unit)) {
        BCM_CONTROL(unit)->system_initialized_rv = rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  system_shutdown
 * Purpose:
 *  Shutdown threads that may access hardware during reset.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  cleanup - If true, attempt to free all resources
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  Ideally this function would call _bcm_shutdown, which is
 *      currently used for WARM_BOOT only.
 */

int
system_shutdown(int unit, int cleanup)
{
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    bcm_flowtracker_periodic_export_config_t config;
#endif

#if defined(INCLUDE_PKTIO)
    if (soc_feature(unit, soc_feature_sdklt_pktio)) {
        if (pktio_pw_running(unit)) {
            (void) pktio_pw_clean(unit);
        }
    } else
#endif /* INCLUDE_PKTIO */
    {
        if (pw_running(unit)) {
            pw_stop(unit, TRUE);
        }
    }

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
        soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {

        bcm_flowtracker_periodic_export_config_t_init(&config);
        (void)bcm_flowtracker_periodic_export_config_set(unit,
                        BCM_FLOWTRACKER_TYPE_AGGREGATE, &config);
    }
#endif

    /* Disregard the error codes on these two calls due to shutdown */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        (void)bcm_cpri_speed_scan_config_set(unit, 0, 0);
    }
#endif
    (void)bcm_linkscan_enable_set(unit, 0);
    (void)bcm_rx_stop(unit, NULL);

    if (cleanup) {
        /* Clean up SOC and BCM */
    }

    return BCM_E_NONE;
}


/*
 * These implementations are provided as temporary stubs
 * for builds using NO_SAL_APPL only
 */

#ifdef NO_SAL_APPL

int 
sal_portable_printf(const char* fmt, ...)
{
    va_list args; 
    int rc; 

    va_start(args, fmt); 
    rc = sal_portable_vprintf(fmt, args); 
    va_end(args); 
    return rc; 
}

char *
sal_portable_readline_internal(char *prompt, char *buf, int bufsize, char *defl)
{
    char *s, *full_prompt, *cont_prompt;
    int len;

    if (bufsize == 0)
        return NULL;

    cont_prompt = prompt[0] ? "? " : "";
    full_prompt = sal_alloc(sal_strlen(prompt) + (defl ? sal_strlen(defl) : 0) + 8, __FILE__);
    sal_strncpy(full_prompt, prompt, sal_strlen(prompt) + 1);
    if (defl)
    sal_sprintf(full_prompt + sal_strlen(full_prompt), "[%s] ", defl);

    s = sal_portable_readline(full_prompt, buf, bufsize); 

    if (s == 0) {                       /* Handle Control-D */
        buf[0] = 0;
    /* EOF */
    buf = 0;
    goto done;
    } 

    len = 0;
    if (s[0] == 0) {
    if (defl && buf != defl) {
            if ((len = sal_strlen(defl)) >= bufsize) {
                len = bufsize - 1;
            }
        sal_memcpy(buf, defl, len);
    }
    } else {
    if ((len = sal_strlen(s)) >= bufsize) {
            len = bufsize - 1;
            cli_out("WARNING: input line truncated to %d chars\n", len);
        }
    sal_memcpy(buf, s, len);
    }
    buf[len] = 0;


    /*
     * If line ends in a backslash, perform a continuation prompt.
     */

    s = buf + sal_strlen(buf) - 1;
    if (*s == '\\' && sal_readline(cont_prompt, s, bufsize - (s - buf), 0) == 0)
    buf = 0;

 done:
    sal_free(full_prompt);
    return buf;
}


#endif /* NO_SAL_APPL */
