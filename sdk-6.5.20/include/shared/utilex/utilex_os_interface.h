/** \file utilex_os_interface.h
 *  
 * 
 * OS interface for math utils. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef UTILEX_OS_INTERFACE_H_INCLUDED
#define UTILEX_OS_INTERFACE_H_INCLUDED

/* 
 * {
 */

#ifdef  __cplusplus
extern "C"
{
#endif

#include <shared/utilex/utilex_framework.h>
#include <shared/shrextend/shrextend_error.h>
#include <sal/core/sync.h>

/* #include <soc/drv.h> */
/* some os methods accept a timeout parameter,
 * that might indicate (a) infinite timeout (b) no timeout.
 * We use this instead, inorder to be os indepenedent
 */
#define UTILEX_INFINITE_TIMEOUT (-1)
#define UTILEX_NO_TIMEOUT       0

/* Helper for sal_alloc */
#define sal_dnx_alloc(size,str) sal_alloc(size,str)

/* Helper for sal_rand */
#ifndef __KERNEL__
#include <sal/appl/sal.h>
#else
    extern int utilex_sal_rand_in_kernel_mode(
    void);
#define sal_rand utilex_sal_rand_in_kernel_mode
#endif
/*
 * Macro "UTILEX_PRINTF_ATTRIBUTE":
 * This define is originated from GCC capabilities to check
 * "printf" like function formating.
 * For exact formating refer to GCC docs section "Declaring Attributes of Functions".
 *
 * Define "UTILEX_PRINTF_ATTRIBUTE_ENABLE_CHECK"
 * In order to do the checks, define "UTILEX_PRINTF_ATTRIBUTE_ENABLE_CHECK"
 * in your compilation system. In GCC add "-DSAND_PRINTF_ATTRIBUTE_ENABLE_CHECK"
 * to the compilation command line.
 */
#ifdef UTILEX_PRINTF_ATTRIBUTE_ENABLE_CHECK
#ifdef __GNUC__
#define UTILEX_PRINTF_ATTRIBUTE(string_index, first_to_check)  \
      __attribute__ ((format (__printf__, string_index, first_to_check)))
#else
#error  "Add your system support for 'UTILEX_PRINTF_ATTRIBUTE(string_index, first_to_check)'."
#endif

#elif defined(COMPILER_ATTRIBUTE)
#define UTILEX_PRINTF_ATTRIBUTE(string_index, first_to_check) \
    COMPILER_ATTRIBUTE ((format (__printf__, string_index, first_to_check)))
#else
#define UTILEX_PRINTF_ATTRIBUTE(string_index, first_to_check)
#endif

/**
 *  ANSI C forbids casts from function pointers to data pointers and vice versa, so you can't 
 *  use void * to pass a pointer to a function.
 */
    typedef void (
    *UTILEX_FUNC_PTR) (
    void *);

/**
 * {
 * Sorting and Searching
 */

/**
 * qsort
 */
    void utilex_os_qsort(
    void *bot,
    uint32 nmemb,
    uint32 size,
    int (*compar) (void *,
                   void *));
/**
 * bsearch
 */
    void *utilex_os_bsearch(
    void *key,
    void *base0,
    uint32 nmemb,
    uint32 size,
    int (*compar) (void *,
                   void *));

/*
 * }
 */

/*
 * {
 * Task management
 */

/*
 * Lock and unlock task switching.
 */
    shr_error_e utilex_os_task_lock(
    void);
    shr_error_e utilex_os_task_unlock(
    void);

/*
 * taskSpawn is for unix fork()
 */
    sal_thread_t utilex_os_task_spawn(
    char *name,                 /* name of new task (stored at pStackBase) */
    long priority,              /* priority of new task */
    long options,               /* task option word */
    long stackSize,             /* size (bytes) of stack needed plus name */
    UTILEX_FUNC_PTR entryPt,    /* entry point of new task */
    long arg1,                  /* 1st of 10 req'd task args to pass to func */
    long arg2,
    long arg3,
    long arg4,
    long arg5,
    long arg6,
    long arg7,
    long arg8,
    long arg9,
    long arg10);
/*
 * delete a task
 */
    shr_error_e utilex_os_task_delete(
    sal_thread_t task_id);
/*
 * returns the running thread tid
 * should never be zero.
 */
    sal_thread_t utilex_os_get_current_tid(
    void);

/*
 * }
 */

/*
 * {
 * Memory Management
 */

/*
 * malloc
 */
    void *utilex_os_malloc(
    uint32 size,
    char *str);
/*
 * free
 */
    shr_error_e utilex_os_free(
    void *memblock);

/*
 * malloc
 */
    void *utilex_os_malloc_any_size(
    uint32 size,
    char *str);
/*
 * free
 */
    shr_error_e utilex_os_free_any_size(
    void *memblock);
/*
 * Get amount of bytes are currently allocated by utilex_os_malloc()
 */
    uint32 utilex_os_get_total_allocated_memory(
    void);

/*
 * copy a buffer from source to destination
 */
    shr_error_e utilex_os_memcpy(
    void *destination,          /* destination of copy */
    const void *source,         /* source of copy */
    uint32 size                 /* size of memory to copy (in bytes) */
        );
/*
 * compare 2 buffers
 */
    int utilex_os_memcmp(
    const void *s1,             /* array 1 */
    const void *s2,             /* array 2 */
    uint32 n                    /* size of memory to compare */
        );
/*
 * loads memory with a char
 */
    shr_error_e utilex_os_memset(
    void *m,                    /* array 1 */
    int c,                      /* array 2 */
    uint32 n                    /* size of memory in bytes */
        );

/*
 * }
 */

/*
 * {
 * Sleep and Time
 */

/*
 * suspend a task for mili-second.
 */
    shr_error_e utilex_os_task_delay_milisec(
    uint32 delay_in_milisec);
/*
 * This routine returns the system clock rate.
 * The number of ticks per second of the system clock.
 */
    int utilex_os_get_ticks_per_sec(
    void);
/*
 * most rt OS can suspend a task for several nano seconds
 */
    int utilex_os_nano_sleep(
    uint32 nano,
    uint32 *premature);
/**
 * \brief
 *   Get current system time in seconds and nano-seconds
 * \par DIRECT INPUT
 *   \param [in] seconds -
 *     Pointer to uint32 \n
 *     \b As \b output: \n
 *         This procedure loads pointed memory by the 'seconds' section
 *         of the current time
 *   \param [in] nano_seconds -
 *     Pointer to uint32 \n
 *     \b As \b output: \n
 *         This procedure loads pointed memory by the 'nano seconds' section
 *         of the current time
 * \par INDIRECT INPUT
 *   * OS timer
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *seconds - \n
 *     See 'seconds' on DIRECT INPUT
 *   \b *nano_seconds - \n
 *     See 'nano_seconds' on DIRECT INPUT
 * \remark
 *   POSIX 1003.1b documentation
 */
    shr_error_e utilex_os_get_time(
    uint32 *seconds,
    uint32 *nano_seconds);
/*
 * number of ticks passed since system start up
 */
    shr_error_e utilex_os_tick_get(
    uint32 *ticks);
/*
 *	Get time difference. 
 *  Assumes start_nano_seconds and end_nano_seconds
 *  are less then 1 second.
 *  Assumes that end_seconds >= start_seconds
 */
    shr_error_e utilex_os_get_time_diff(
    uint32 start_seconds,
    uint32 start_nano_seconds,
    uint32 end_seconds,
    uint32 end_nano_seconds,
    uint32 *diff_seconds,
    uint32 *diff_nano_seconds);
    uint32 utilex_os_get_time_micro(
    void);

/*
 * returns the number of system ticks in 1 second
 */
    uint32 utilex_os_sys_clk_rate_get(
    void);

/*
 * }
 */

/*
 * {
 * Semaphores
 */

/*
 * returns the sem_id to be used by the other procs
 */
    sal_mutex_t utilex_os_mutex_create(
    void);
/*
 * deletes a mutex
 */
    shr_error_e utilex_os_mutex_delete(
    sal_mutex_t sem_id);
/*
 * timeout might be UTILEX_INFINITE_TIMEOUT
 */
    shr_error_e utilex_os_mutex_take(
    sal_mutex_t sem_id,
    long timeout);
/*
 * release a mutex
 */
    shr_error_e utilex_os_mutex_give(
    sal_mutex_t sem_id);

/*
 * }
 */

/*
 * {
 * Level control
 */

/*
 * disable interrupts
 */
    shr_error_e utilex_os_stop_interrupts(
    uint32 *flags);
/*
 * resotore interrupts
 */
    shr_error_e utilex_os_start_interrupts(
    uint32 flags);

/*
 * }
 */

/*
 * {
 * Message queues
 */

/*
 * returns MSG_Q_ID used by the other procs
 */
    void *utilex_os_msg_q_create(
    int max_msgs,
    int max_msglength);
/*
 * deletes a message queue
 */
    shr_error_e utilex_os_msg_q_delete(
    void *msg_q_id);
/*
 * sends a message to a message queue
 */
    shr_error_e utilex_os_msg_q_send(
    void *msg_q_id,
    char *data,
    uint32 data_size,
    int timeout,
    int priority);
/*
 * return = 0 => got a message
 * return > 0 => time out
 * return < 0 => error
 * timeout might be UTILEX_INFINITE_TIMEOUT
 */
    int utilex_os_msg_q_recv(
    void *msg_q_id,
    unsigned char *data,
    uint32 max_nbytes,
    long timeout_in_ticks);
    int utilex_os_msg_q_num_msgs(
    void *msg_q_id);

/*
 * }
 */

/*
 * {
 * STDIO and String - only for error/debug/printing
 */

/*
 * determine the length of a string (ANSI)
 */
    uint32 utilex_os_strlen(
    const char *s               /* string */
        );

/*
 * compare N chars
 */
    int utilex_os_strncmp(
    const char *string1,
    const char *string2,
    uint32 count);

/*
 * copy a string from source to destination
 */
    shr_error_e utilex_os_strncpy(
    char *s1,                   /* string to copy to */
    const char *s2,             /* string to copy from */
    uint32 n                    /* max no. of characters to copy */
        );

/* {  Random function. */
/*
 * The srand function sets the starting point for generating
 *  a series of pseudorandom integers
 */
    void utilex_os_srand(
    uint32);
/*
 * Generates a pseudo-random integer between 0 and RAND_MAX
 */
    uint32 utilex_os_rand(
    void);
/* }  END Random function. */
/* {  IO printing functions. */
/*
 * printf
 */
    int utilex_os_printf(
    char *format,
    ...) UTILEX_PRINTF_ATTRIBUTE(
    1,
    2);
/* } END IO printing functions. */

/*
 * }
 */

/*
 * Report on the taken resources.
 */
    shr_error_e utilex_os_resource_print(
    void);

#ifdef  __cplusplus
}
#endif

/*
 * }
 */
#endif
