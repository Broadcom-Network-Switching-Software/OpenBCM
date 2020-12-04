/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. $
 */
/** \file utilex_framework.h
 */

#ifndef UTILEX_FRAMEWORK_H_INCLUDED
/*
 * {
 */
#define UTILEX_FRAMEWORK_H_INCLUDED

/*************
* INCLUDES  *
 */

#include <sal/types.h>

#include <shared/shrextend/shrextend_debug.h>
/* #include <shared/utilex/utilex_os_interface.h> */

/**
 * Recommended for systems in bring-up phase.
 * If defined then debug code is included (such as tests.
 * See, e.g., utilex_multi_set_test_1())
 */
#define UTILEX_DEBUG 1

/*
 * NULL pointer
 */
#ifndef NULL
#define NULL (void *)0
#endif
/*
 * boolean true
 */
#ifndef TRUE
#define TRUE 1
#endif
/*
 * boolean false
 */
#ifndef FALSE
#define FALSE 0
#endif

/*
 * basic types definition
 */

#define UTILEX_BOOL2NUM(b_val) ((b_val) == FALSE?0x0:0x1)
#define UTILEX_NUM2BOOL(n_val) ((uint8)((n_val) == 0x0?FALSE:TRUE))

/*
 *  Invert the result - TRUE if value is 0, FALSE otherwise
 */
#define UTILEX_BOOL2NUM_INVERSE(b_val) ((b_val) == TRUE?0x0:0x1)
#define UTILEX_NUM2BOOL_INVERSE(n_val) (uint8)((n_val) == 0x0?TRUE:FALSE)

/*
 *  Range-related, unsigned.
 *  Get count of entities, first and last entity in range.
 */
#define UTILEX_RNG_COUNT(n_first, n_last) (((n_last) >= (n_first))?((n_last) - (n_first) + 1):((n_first) - (n_last) + 1))
#define UTILEX_RNG_FIRST(n_last, n_cnt)   ((((n_last) + 1) >= (n_cnt))?(((n_last) + 1) - (n_cnt)):0)
#define UTILEX_RNG_LAST(n_first, n_cnt)   ((n_first) + (n_cnt) - 1)
#define UTILEX_RNG_NEXT(n_curr)           ((n_curr) + 1)
#define UTILEX_RNG_PREV(n_curr)           (((n_curr) > 0)?((n_curr) - 1):0)

/*
 * basic return type of basic driver methods
 */
typedef unsigned short UTILEX_RET;

/*
 * the basic UTILEX_RET for error
 */
#define UTILEX_ERR 1
/*
 * the basic UTILEX_RET for success
 */
#define UTILEX_OK  0
/*
 * all input params to a method must be one of these 3
 */
#define UTILEX_IN   const
#define UTILEX_OUT
#define UTILEX_INOUT

#define UTILEX_NOF_BITS_IN_BYTE 8

#define UTILEX_BIT_BYTE_SHIFT 3
#define UTILEX_BIT_KB_SHIFT 13

#define UTILEX_TRANSLATE_BITS_TO_BYTES(_var)   \
           ((_var) >> UTILEX_BIT_BYTE_SHIFT)

#define UTILEX_TRANSLATE_BITS_TO_KB(_var)   \
           ((_var) >> UTILEX_BIT_KB_SHIFT)

#define UTILEX_TRANSLATE_KB_TO_BITS(_var)   \
           ((_var) << UTILEX_BIT_KB_SHIFT)
/*
 * A generic indication for an invalid value, 32-bit.
 * May be used by internal driver functions
 */
#define UTILEX_INTERN_VAL_INVALID_32  0xffffffff

/*
 *      A value marking invalid register in the device
 */
#define UTILEX_REG_VAL_INVALID   0xDEADBEAF

/*
 * TRUE if the internal value is the "invalid-value" indication
 */
#define UTILEX_IS_INTERN_VAL_INVALID(intern_val)  \
  UTILEX_NUM2BOOL(intern_val == UTILEX_INTERN_VAL_INVALID_32)

/*
 * Byte swapping MACRO.
 */
#define UTILEX_BYTE_SWAP(x) ((((x) << 24)) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24)))

/*
 * read / write from the chip must take one of this forms
 */
typedef enum
{
    UTILEX_DIRECT,
    UTILEX_INDIRECT
} UTILEX_ACCESS_METHOD;
/*
 * sets the x bit in a word, and only it
 */
#ifndef UTILEX_BIT
#define UTILEX_BIT(x) (1UL<<(x))
#endif
/*
 * resets the x bit in a word, and only it
 */
#ifndef UTILEX_RBIT
#define UTILEX_RBIT(x) (~(1UL<<(x)))
#endif

/*
 */
#define UTILEX_OFFSETOF(x,y)  ((uint32)((char*)(&(((x *)0)->y)) - (char*)0))
/*
 * Take value and put it in its proper location within a 'long'
 * register (and make sure it does not effect bits outside its
 * predefined mask).
 */
#define  UTILEX_SET_FLD_IN_PLACE(val,shift,mask) (uint32)(((uint32)(val) << ((uint32)shift)) & (uint32)(mask))
/*
 * Get a value out of location within a 'long' register (and make sure it
 * is not effected by bits outside its predefined mask).
 */
#define  UTILEX_GET_FLD_FROM_PLACE(val,shift,mask) (((uint32)(val) & (mask)) >> (shift))
/**
 * Set all bits, from 'ls_bit' to 'ms_bit' to '1' and all the rest to '0'.
 * Both 'ms_bit' and 'ls_bit' are assumed to be smaller than 32!.
 */
#define  UTILEX_BITS_MASK(ms_bit,ls_bit) \
    ( ((uint32)SAL_BIT(ms_bit)) - ((uint32)SAL_BIT(ls_bit)) + ((uint32)SAL_BIT(ms_bit)) )
/**
 * Set all bits, from 'ls_bit' to 'ms_bit' to '0' and all the rest to '1'.
 * Both 'ms_bit' and 'ls_bit' are assumed to be smaller than 32!.
 */
#define  UTILEX_ZERO_BITS_MASK(ms_bit,ls_bit)  (~(UTILEX_BITS_MASK(ms_bit, ls_bit)))

#define  UTILEX_RBITS_MASK(ms_bit,ls_bit)      (UTILEX_ZERO_BITS_MASK(ms_bit, ls_bit))
/**
 * Move all bits, starting from BIT0, in input uint32 value ('val'), to the range from
 * 'ls_bit' to 'ms_bit' and set all the rest to '0'.
 * Both 'ms_bit' and 'ls_bit' are assumed to be smaller than 32!.
 */
#define  UTILEX_SET_BITS_RANGE(val,ms_bit,ls_bit) \
 (((uint32)(val) << (ls_bit)) & (UTILEX_BITS_MASK(ms_bit,ls_bit)))

#define  UTILEX_GET_BITS_RANGE(val,ms_bit,ls_bit) \
 (((uint32)(val) & (UTILEX_BITS_MASK(ms_bit,ls_bit))) >> (ls_bit))

#define  UTILEX_GET_BIT(val,bit) (UTILEX_GET_BITS_RANGE(val,bit,bit)?(uint32)0x1:(uint32)0x0)

/*
 * Set/Get of bytes.
 * Byte-0: Bits 24-31
 * Byte-1: Bits 16-23
 * Byte-2: Bits 8 -15
 * Byte-3: Bits 0 -7
 */
#define  UTILEX_GET_BYTE_3(val)  UTILEX_GET_FLD_FROM_PLACE(val,0,  0x000000FF)
#define  UTILEX_GET_BYTE_2(val)  UTILEX_GET_FLD_FROM_PLACE(val,8,  0x0000FF00)
#define  UTILEX_GET_BYTE_1(val)  UTILEX_GET_FLD_FROM_PLACE(val,16, 0x00FF0000)
#define  UTILEX_GET_BYTE_0(val)  UTILEX_GET_FLD_FROM_PLACE(val,24, 0xFF000000)

#define  UTILEX_SET_BYTE_2(val)  UTILEX_SET_FLD_IN_PLACE(val,8,  0x0000FF00)
#define  UTILEX_SET_BYTE_1(val)  UTILEX_SET_FLD_IN_PLACE(val,16, 0x00FF0000)

#define UTILEX_SET_BIT(reg,val,bit_i)         \
  reg = (val ? (reg | SAL_BIT(bit_i)) : (reg & SAL_RBIT(bit_i)));

#define UTILEX_APPLY_MASK(_reg, _enable, _mask)   \
    _reg = (_enable ? (_reg | _mask) : (_reg & ~_mask));

/*
 * Basic SOC_SAND types limits
 */
#define UTILEX_U8_MAX 0xff

#define UTILEX_U16_MAX 0xffff
#define UTILEX_I16_MAX 0x7fff

#define UTILEX_U32_MAX 0xffffffff
#define UTILEX_I32_MAX 0x7fffffff

#define UTILEX_UINT_MAX SAL_UINT32_MAX

/*
 * MAC address related
 * {
 */
/*************
* DEFINES   *
 */
/*
 * {
 */
/*
 * String length of MAC address
 */
#define  UTILEX_PP_MAC_ADDRESS_STRING_LEN  12
/*
 * number of bytes in MAC address
 */
#define  UTILEX_PP_MAC_ADDRESS_NOF_U8      6
/*
 * number of bits in MAC address
 */
#define  UTILEX_PP_MAC_ADDRESS_NOF_BITS (UTILEX_PP_MAC_ADDRESS_NOF_U8 * UTILEX_NOF_BITS_IN_CHAR)
/*
 * number of longs in MAC address
 */
#define  UTILEX_PP_MAC_ADDRESS_NOF_UINT32S 2

/*
 * String length of IPV6 address
 */
#define  UTILEX_PP_IPV6_ADDRESS_STRING_LEN  32
/*
 * number of bytes in IPV6 address
 */
#define  UTILEX_PP_IPV6_ADDRESS_NOF_U8      16
/*
 * number of bytes in IPv6 multicast group
 */
#define  UTILEX_PP_IPV6_GROUP_NOF_U8        15
/*
 * number of bits in IPV6 address
 */
#define  UTILEX_PP_IPV6_ADDRESS_NOF_BITS (UTILEX_PP_IPV6_ADDRESS_NOF_U8 * UTILEX_NOF_BITS_IN_CHAR)
/*
 * number of bits in IPV6 multicast group
 */
#define  UTILEX_PP_IPV6_GROUP_NOF_BITS   (UTILEX_PP_IPV6_GROUP_NOF_U8 * UTILEX_NOF_BITS_IN_CHAR)
/*
 * number of longs in IPV6 address
 */
#define  UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S 4

#define UTILEX_PP_IP_STR_SIZE (4*3+3+1+2)

/*
 * }
 */

/*************
* MACROS    *
 */
/*
 * {
 */

#define UTILEX_PP_MAC_ADDRESS_IS_ZERO(_mac_)  \
    (((_mac_).address[0] | (_mac_).address[1] | (_mac_).address[2] | \
      (_mac_).address[3] | (_mac_).address[4] | (_mac_).address[5]) == 0)

/*
 * }
 */

/*************
* TYPE DEFS *
 */
/*
 * {
 */

/*
 * }
 */
/**************
 * PROTOTYPES *
 */
/*
 * {
 */
/**
 * \brief
 *   Convert a MAC address structure (uint8[] of size 6) to an array
 *   of uint32s
 * \par DIRECT INPUT
 *   \param [in] mac_address -
 *     Pointer to struct of type mac_add_long
 *     \b As \b input:
 *       Six bytes array pointer
 *       of MAC address to convert
 *   \param [in] mac_add_long -
 *     Pointer to array of UTILEX_PP_MAC_ADDRESS_NOF_UINT32S uint32s
 *     \b As \b output:
 *       Pointed array is loaded by the input bytes (from 'mac_add_struct')
 *       such that they are laid from LS byte to MS byte within each
 *       long. So \n
 *                   MS bit ........................................................................ LS BIT
 *       FIRST LONG: BYTE3 of MAC ADDRESS, BYTE2 of MAC ADDRESS, BYTE1 of MAC ADDRESS, BYTE0 of MAC ADDRESS
 *       SECND LONG:          0          ,          0          , BYTE5 of MAC ADDRESS, BYTE6 of MAC ADDRESS
 * \par INDIRECT INPUT
 *   * See 'mac_add_struct'
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'mac_add_long'
 */
shr_error_e utilex_pp_mac_address_struct_to_long(
    uint8 mac_address[UTILEX_PP_MAC_ADDRESS_NOF_U8],
    uint32 mac_add_long[UTILEX_PP_MAC_ADDRESS_NOF_UINT32S]);
/**
 * \brief
 *   Convert an array of uint32s to a MAC address structure (utilex_pp_mac_address_t)
 * \par DIRECT INPUT
 *   \param [in] mac_add_long -
 *     Pointer to array of UTILEX_PP_MAC_ADDRESS_NOF_UINT32S uint32s
 *     \b As \b input:
 *       Pointed array is moved to 'mac_add_struct' as follows \n
 *                   MS bit ........................................................................ LS BIT
 *       FIRST LONG: BYTE3 of MAC ADDRESS, BYTE2 of MAC ADDRESS, BYTE1 of MAC ADDRESS, BYTE0 of MAC ADDRESS
 *       SECND LONG:          0          ,          0          , BYTE5 of MAC ADDRESS, BYTE6 of MAC ADDRESS
 *   \param [in] mac_address -
 *     Six bytes array pointer
 *     \b As \b output:
 *       Pointed array is loaded by 6
 *       (UTILEX_PP_MAC_ADDRESS_NOF_U8) bytes bytes of MAC
 *       address as described in 'mac_add_long' above
 * \par INDIRECT INPUT
 *   * See 'mac_add_long'
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'mac_add_struct'
 */
shr_error_e utilex_pp_mac_address_long_to_struct(
    uint32 mac_add_long[UTILEX_PP_MAC_ADDRESS_NOF_UINT32S],
    uint8 mac_address[UTILEX_PP_MAC_ADDRESS_NOF_U8]);

/*
 * \brief
 *   Convert a IPV6 address structure (uint8[] of size 16) to an
 *   array of uint32s
 * \par DIRECT INPUT
 *   \param [in] ipv6_address -
 *     Pointer to array of type uint8s
 *     \b As \b input:
 *       16 bytes array pointer
 *       of IPV6 address to convert
 *   \param [in] ipv6_add_long -
 *     Pointer to array of UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S
 *     uint32s
 *     \b As \b output:
 *       Pointed array is loaded by the input bytes
 *       such that they are laid from LS byte to MS byte within each
 *       long. So \n
 *                   MS bit ........................................................................ LS BIT
 *       FIRST LONG: BYTE3 of MAC ADDRESS, BYTE2 of MAC ADDRESS, BYTE1 of MAC ADDRESS, BYTE0 of MAC ADDRESS
 *       SECND LONG: BYTE7 of MAC ADDRESS, BYTE6 of MAC ADDRESS, BYTE5 of MAC ADDRESS, BYTE4 of MAC ADDRESS
 *       ...
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e utilex_pp_ipv6_address_struct_to_long(
    uint8 ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_U8],
    uint32 ipv6_add_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S]);
/**
 * \brief
 *   Convert an array of uint32s to a IPV6 address array of
 *   uint8s
 * \par DIRECT INPUT
 *   \param [in] ipv6_add_long -
 *     Pointer to array of UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S
 *     uint32s
 *     \b As \b input:
 *       Pointed array is moved to 'ipv6_address' as follows \n
 *                   MS bit ........................................................................ LS BIT
 *       FIRST LONG: BYTE3 of MAC ADDRESS, BYTE2 of MAC ADDRESS, BYTE1 of MAC ADDRESS, BYTE0 of MAC ADDRESS
 *       SECND LONG: BYTE7 of MAC ADDRESS, BYTE6 of MAC ADDRESS, BYTE5 of MAC ADDRESS, BYTE4 of MAC ADDRESS
 *   \param [in] ipv6_address -
 *     Six bytes array pointer
 *     \b As \b output:
 *       Pointed array is loaded by 16
 *       (UTILEX_PP_IPV6_ADDRESS_NOF_U8) bytes bytes of IPV6
 *       address as described in 'ipv6_add_long' above
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e utilex_pp_ipv6_address_long_to_struct(
    uint32 ipv6_add_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S],
    uint8 ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_U8]);

/**
 * \brief
 *   Convert a IPV6 adress structure (uint8[] of size 16) with a
 *   meaning of IPv6 MC group (14 least significant bytes from
 *   the IPv6 address) to an array of uint32s
 * \param [in] ipv6_address -
 *     Pointer to array of type uint8s - 16 bytes array pointer
 *     of IPV6 address to convert
 * \param [out] ipv6_add_long -
 *     Pointer to array of UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S
 *     uint32s. Pointed array is loaded by the input bytes such
 *     that they are laid from LS byte to MS byte within each
 *     long.
 *     Pointed array is moved to 'ipv6_add_long' as follows \n
 *                    MS bit ........................................................................ LS BIT
 *       FIRST LONG:  BYTE12, BYTE13, BYTE14, BYTE15
 *       SECND LONG:  BYTE8,  BYTE9,  BYTE10, BYTE11
 *       THIRD LONG:  BYTE4,  BYTE5,  BYTE6,  BYTE7
 *       FOURTH LONG: EMPTY,  EMPTY,  BYTE2,  BYTE3
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e utilex_pp_ipv6_group_struct_to_long(
    uint8 ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_U8],
    uint32 ipv6_add_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S]);
/**
 * \brief - convert addr to string (short and long format)
 */
shr_error_e utilex_pp_ip_addr_numeric_to_string(
    uint32 ip_addr,
    uint8 short_format,
    char decimal_ip[UTILEX_PP_IP_STR_SIZE]);
/*
 * }
 */
/*
 * }
 */

/**
 * If defined, package of utilities for measurement of the execution
 * time of  various source code sections is included
 */
#define UTILEX_LL_TIMER

#ifdef UTILEX_LL_TIMER
/* { */
/**
 * Maximal number of execution-time timer-groups that can be activated.
 * Each timers groups has UTILEX_LL_TIMER_MAX_NOF_TIMERS available timers.
 */
#define UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS               (4)
/**
 * Maximal number of characters in a single timer-group's name
 */
#define UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME  (20)
/**
 * Index of timers' group which indicates 'none'. When soch index
 * is returned by, say, utilex_ll_timer_group_allocate(), it means
 * no group is available (All are allocated).
 */
#define UTILEX_LL_TIMER_NO_GROUP                           (-1)
/**
 * Name assigned to an empty timer-group
 * Make sure this is not longer than UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME
 * (including the ending NULL)
 */
#define UTILEX_LL_TIMER_EMPTY_GROUP_NAME                   "NONE"
/**
 * Maximal number of execution-time timers that can be activated
 */
#define UTILEX_LL_TIMER_MAX_NOF_TIMERS                     (30)
/**
 * Maximal number of characters in a single timer's name
 */
#define UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME        (40)

/*************
* TYPEDEFS   *
 */
/*
 * {
 */

/**
 * Structure of data base attached to each timer group.
 * \see
 *  *  utilex_ll_timer_group_allocate()
 *  *  utilex_ll_timer_group_free()
 */
typedef struct
{
    /**
     * Name (identifier) of the timer group.
     * For example, if the timer is activated upon
     * a function call, the function name.
     */
    char name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    /**
     * Flag. If non-zero then this timers' group has been allocated
     * by some user and may no be used by another user until it is freed.
     */
    int locked;
    /**
     * Flag. If non-zero then at least one of this timers' group has been activated.
     */
    int is_any_active;
    /**
     * Sum of all collected time of all timers.
     */
    uint32 timer_total;
} UTILEX_LL_TIMER_GROUP;

/**
 * Structure of data base attached to each timer.
 * \see
 *  *  utilex_ll_timer_clear()
 *  *  utilex_ll_timer_set()
 *  *  utilex_ll_timer_stop()
 *  *  utilex_ll_timer_stop_all()
 */
typedef struct
{
    /**
     * Name (identifier) of the timer.
     * For example, if the timer is activated upon
     * a function call, the function name.
     */
    char name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME];
    /**
     * Number of times the timer was hit.
     * For example, if the timer is activated upon a function call,
     * this would be the number of function calls.
     */
    uint32 nof_hits;
    /**
     * Boolean indication on whether this timer is running or not.
     * Non-zero indicates 'running'
     */
    uint32 active;
    /**
     * Start time of the timer (in micro seconds).
     */
    uint32 start_timer;
    /**
     * Accumulated time measured by this timer,
     * in micro seconds
     */
    uint32 total_time;
} UTILEX_LL_TIMER_FUNCTION;
/*
 * }
 */
/*************
* Global     *
 */
/*
 * {
 */
/**
 * The timers' groups
 */
extern UTILEX_LL_TIMER_GROUP Utilex_ll_timer_group[UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS];
/**
 * Array of structures, each describing one counter.
 */
extern UTILEX_LL_TIMER_FUNCTION
    Utilex_ll_timer_cnt[UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS][UTILEX_LL_TIMER_MAX_NOF_TIMERS];
/**
 * Flag indicating whether any of the timers is active. Cleared in utilex_ll_timer_clear_all(). \n
 * Set in utilex_ll_timer_set().
 */
extern uint8 Utilex_ll_is_any_active[UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS];
/*
 * Total 'active' time of all timers. In Micro seconds. Updated in utilex_ll_timer_set(). \n
 * Cleared in utilex_ll_timer_clear_all()
 */
extern uint32 Utilex_ll_timer_total[UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS];
/*
 * }
 */
/**************
 * PROTOTYPES *
 */
/*
 * {
 */
/**
 * \brief
 *   Get current system time in milliseconds
 * \par DIRECT INPUT
 *   None
 * \par INDIRECT INPUT
 *   * OS timer
 * \par DIRECT OUTPUT
 *   \retval System Time in milli seconds
 * \par INDIRECT OUTPUT
 *   None
 * \see
 *   utilex_os_get_time()
 */
uint32 utilex_ll_get_time_in_ms(
    void);

/**
 * \brief
 *   Look for a free timers' group and lock it for allocation by other users.
 *   Return the identifier of the newly allocated group.
 * \par DIRECT INPUT
 *   \param [in] group_name -
 *     Pointer to NULL-terminated string
 *     to this counter \n
 *     \b As \b input: \n
 *         Pointed memory contains the name to assign to this timers' group.
 *   \param [in] timers_group_p -
 *     Pointer to uint32. \n
 *     \b As \b output: \n
 *         This procedure loads pointed memory by an identifier of an available
 *         group or, if there is none, by UTILEX_LL_TIMER_NO_GROUP.
 * \par INDIRECT INPUT
 *   * UTILEX_LL_TIMER_MAX_NOF_TIMER_GROUPS (number of available grups)
 *   * See 'group_name' on DIRECT INPUT
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'timers_group_p' on DIRECT INPUT
 *   * \b Utilex_ll_timer_group - \n
 *     Array of status anf config of all groups
 * \see
 *   * utilex_ll_timer_group_free()
 */
shr_error_e utilex_ll_timer_group_allocate(
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME],
    uint32 *timers_group_p);
/**
 * \brief
 *   Free specified timers' group (unlock it for allocation by other users).
 *   Indicated timers' group must be in 'locked' state
 * \par DIRECT INPUT
 *   \param [in] timers_group -
 *     Identifier of the timer's group that needs to be freed.
 * \par INDIRECT INPUT
 *   * Utilex_ll_timer_group[timers_group]
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * Utilex_ll_timer_group[timers_group].locked
 * \see
 *   * utilex_ll_timer_group_allocate()
 */
shr_error_e utilex_ll_timer_group_free(
    uint32 timers_group);

/**
 * \brief
 *   Reset all counters (via their attached data bases) to their
 *   initial state.
 * \par DIRECT INPUT
 *   \param [in] timers_group -
 *     Identifier of the timer's group that needs to be cleared.
 * \par INDIRECT INPUT
 *   * UTILEX_LL_TIMER_MAX_NOF_TIMERS (number of available counters)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b Utilex_ll_timer_cnt[timers_group] - \n
 *     Array of data bases for all counters
 *   \b Utilex_ll_is_any_active[timers_group] - \n
 *     Flag indicating whether any timer is active
 *   \b Utilex_ll_timer_total[timers_group] - \n
 *     Total 'active' time of all counters
 * \see
 *   * utilex_ll_timer_set()
 *   * utilex_ll_timer_stop_all()
 */
shr_error_e utilex_ll_timer_clear_all(
    uint32 timers_group);
/**
 * \brief
 *   Stop all counters (via their attached data bases) but keep
 *   stored info.
 * \par DIRECT INPUT
 *   \param [in] timers_group -
 *     Identifier of the timer's group whose timers need to be stopped.
 * \par INDIRECT INPUT
 *   * UTILEX_LL_TIMER_MAX_NOF_TIMERS (number of available counters)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b Utilex_ll_timer_cnt - \n
 *     Array of data bases for all counters
 *   \b Utilex_ll_is_any_active - \n
 *     Flag indicating whether any timer is active
 *   \b Utilex_ll_timer_total - \n
 *     Total 'active' time of all counters
 * \see
 *   * utilex_ll_timer_print_all()
 */
shr_error_e utilex_ll_timer_stop_all(
    uint32 timers_group);
/**
 * \brief
 *   Associate a counter id with a string (name), set it as active
 *   and mark this activation time. See 'remarks' below
 * \par DIRECT INPUT
 *   \param [in] name -
 *     Pointer to NULL-terminated string \n
 *     \b As \b input: \n
 *         Pointed memory contains the name to assign to this timer.
 *   \param [in] timers_group -
 *     Identifier of the timer's group to which the timer, identified by
 *     'fn_ndx' belongs.
 *   \param [in] fn_ndx -
 *     Identifier of the timer to start (or restart). If this value is \n
 *     larger than (UTILEX_LL_TIMER_MAX_NOF_TIMERS - 1) then this invocation \n
 *     returns with error.
 * \par INDIRECT INPUT
 *   * OS timer
 *   \b *name - \n
 *     See 'name' on DIRECT INPUT
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *seconds - \n
 *     See 'seconds' on DIRECT INPUT
 *   \b *nano_seconds - \n
 *     See 'nano_seconds' on DIRECT INPUT
 * \remark
 *   * This procedure handles error conditions by returning error code to \n
 *     the caller.
 *   * To reste all counters to the initial state, invoke utilex_ll_timer_clear_all()
 *   * This procedure relates to 'name' only if this is the first activation. \n
 *     Name is only used for display purposes.
 *   * This procedure can be invoked a few times, one after the other, provided \n
 *     utilex_ll_timer_stop() is invoked between each two consecutive activations. \n
 *     Both the number of activations and the total 'active' time are collected.
 *   * Results of all timers are displayed using utilex_ll_timer_print_all()
 */
shr_error_e utilex_ll_timer_set(
    char name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_NAME],
    uint32 timers_group,
    uint32 fn_ndx);

/**
 * \brief
 *   Get data base info on specified timer id.
 * \par DIRECT INPUT
 *   \param [in] timers_group -
 *     Identifier of the timer's group to which the timer, identified by
 *     'timer_ndx' belongs.
 *   \param [in] timer_ndx -
 *     Identifier of the timer to retrieve info of.
 *   \param [in] is_active -
 *     Pointer to int \n
 *     \b As \b output: \n
 *         This procedure loads pointed memory by the value of the 'active'
 *         flag which indicates whether this timer is currently running (non-zero/TRUE)
 *         or not (zero/FALSE).
 *   \param [in] name -
 *     Pointer to pointer to string \n
 *     \b As \b output: \n
 *         This procedure loads pointed memory by a pointer to the name of
 *         that timer (NULL terminated string).
 *   \param [in] nof_hits -
 *     Pointer to uint32 \n
 *     \b As \b output: \n
 *         This procedure loads pointed memory by the value of the 'nof_hits'
 *         counter which indicates how many periods this timer has monitored.
 *         (A 'period' is a 'start/stop' session).
 *   \param [in] total_time -
 *     Pointer to uint32 \n
 *     \b As \b output: \n
 *         This procedure loads pointed memory by the value of the 'total_time'
 *         counter which indicates how much time was spent within the periods
 *         that this timer has monitored. Only meaningful of 'nof_hits' is
 *         larger than zero.
 *         (A 'period' is a 'start/stop' session).
 * \par INDIRECT INPUT
 *   * Utilex_ll_timer_cnt[timers_group][timer_ndx]
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *is_active - \n
 *     See 'is_active' on DIRECT INPUT
 *   \b *name - \n
 *     See 'name' on DIRECT INPUT
 *   \b *nof_hits - \n
 *     See 'nof_hits' on DIRECT INPUT
 *   \b *total_time - \n
 *     See 'total_time' on DIRECT INPUT
 * \remark
 */
shr_error_e utilex_ll_timer_get_info(
    uint32 timers_group,
    uint32 timer_ndx,
    int *is_active,
    char **name,
    uint32 *nof_hits,
    uint32 *total_time);

/**
 * \brief
 *   Get data base info on specified group of timers.
 * \par DIRECT INPUT
 *   \param [in] timers_group -
 *     Identifier of the timer's group to retrieve info about.
 *   \param [in] is_locked -
 *     Pointer to int
 *     \b As \b output: \n
 *         This procedure loads pointed memory by the value of the 'locked'
 *         flag which indicates whether this group is currently assigned (non-zero/TRUE)
 *         or not (zero/FALSE).
 *   \param [in] name -
 *     Pointer to pointer to string
 *     \b As \b output: \n
 *         This procedure loads pointed memory by a pointer to the name of
 *         that timers' group (NULL terminated string).
 *   \param [in] is_any_active -
 *     Pointer to int
 *     \b As \b input: \n
 *         This procedure loads pointed memory by an indication on whether
 *         at least one timer has been activated on this group (TRUE) or not (FALSE)
 *   \param [in] total_time_all_timers -
 *     Pointer to uint32
 *     \b As \b input: \n
 *         This procedure loads pointed memory by the sum of all 'total_time's
 *         on all timers.
 * \par INDIRECT INPUT
 *   * Utilex_ll_timer_cnt[timers_group][timer_ndx]
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *is_locked - \n
 *     See 'is_locked' on DIRECT INPUT
 *   \b *name - \n
 *     See 'name' on DIRECT INPUT
 *   \b *is_any_active - \n
 *     See 'is_any_active' on DIRECT INPUT
 *   \b *total_time_all_timers - \n
 *     See 'total_time_all_timers' on DIRECT INPUT
 * \remark
 */
shr_error_e utilex_ll_timers_group_get_info(
    uint32 timers_group,
    int *is_locked,
    char **name,
    int *is_any_active,
    uint32 *total_time_all_timers);

/**
 * \brief
 *   Get name of timers group.
 * \par DIRECT INPUT
 *   \param [in] timers_group -
 *     Identifier of the timer's group to retrieve info about.
 *   \param [in] name -
 *     Pointer to pointer to string
 *     \b As \b output: \n
 *         This procedure loads pointed memory by a pointer to the name of
 *         that timers' group (NULL terminated string).
 * \par INDIRECT INPUT
 *   * Utilex_ll_timer_cnt[timers_group][timer_ndx]
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *   \b *name - \n
 *     See 'name' on DIRECT INPUT
 * \remark
 */
shr_error_e utilex_ll_timers_group_name_get(
    uint32 timers_group,
    char **name);

/**
 * \brief
 *   Stop one counter after updating its attached data base for elapsed time, \n
 *   total time and number of hits. Keep stored info.
 * \par DIRECT INPUT
 *   \param [in] timers_group -
 *     Identifier of the timer's group to which the timer, identified by
 *     'fn_ndx' belongs.
 *   \param [in] fn_ndx -
 *     Identifier of the timer to stop. If this value is \n
 *     larger than (UTILEX_LL_TIMER_MAX_NOF_TIMERS - 1) then this procedure \n
 *     returns with error.
 * \par INDIRECT INPUT
 *   * UTILEX_LL_TIMER_MAX_NOF_TIMERS (number of available counters)
 *   * Utilex_ll_timer_cnt[fn_ndx].active - Flag indicating whether this timer \n
 *     is active
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b Utilex_ll_timer_cnt[timers_group][fn_ndx] - \n
 *     Data bases of this counter
 *   \b Utilex_ll_timer_total[timers_group] - \n
 *     Total 'active' time of all counters
 * \remark
 *   Counter may be reactivated (via utilex_ll_timer_set())
 * \see
 *   * utilex_ll_timer_set()
 */
shr_error_e utilex_ll_timer_stop(
    uint32 timers_group,
    uint32 fn_ndx);
/**
 * \brief
 *   Print info collected on all counters for specified group (via their attached data bases).
 * \par DIRECT INPUT
 *   \param [in] timers_group -
 *     Identifier of the timer's group to print info from. All active timers \n
 *     on this group are displayed.
 * \par INDIRECT INPUT
 *   * Utilex_ll_timer_cnt[timers_group]
 *   * Utilex_ll_is_any_active[timers_group]
 *   * Utilex_ll_timer_total[timers_group]
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Printed info
 * \see
 *   * utilex_ll_timer_set()
 */
shr_error_e utilex_ll_timer_print_all(
    uint32 timers_group);
/**
 * \brief
 *   Small test of timers package. Activate two timers and verify
 *   total time and number of activations.
 * \par DIRECT INPUT
 *   None
 * \par INDIRECT INPUT
 *   * Timers pacjkage
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b Utilex_ll_timer_cnt - \n
 *     Array of data bases for all counters
 *   \b Utilex_ll_is_any_active - \n
 *     Flag indicating whether any timer is active
 *   \b Utilex_ll_timer_total - \n
 *     Total 'active' time of all counters
 * \see
 *   * utilex_ll_timer_print_all()
 * \remark
 *   All timers are reset at end of procedure
 */
shr_error_e utilex_ll_timer_test_01(
    void);

/*
 * }
 */
/* } UTILEX_LL_TIMER */
#endif

/**
 * \brief
 *  set field onto the register
 *
 * \par DIRECT INPUT
 *   \param [in,out] *reg_val  - \n
 *       the value of the register to be changed
 *   \param [in] ms_bit         -\n
 *       most significant bit where to set the field_val,
 *   \param [in] ls_bit         -\n
 *       less significant bit where to set the field_val
 *   \param [in] field_val         -\n
 *       field to set into reg_val
 *  \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 *  \par INDIRECT OUTPUT
 *   See *reg_val above - the value of the register after setting the value into it.
 */
shr_error_e utilex_set_field(
    uint32 *reg_val,
    uint32 ms_bit,
    uint32 ls_bit,
    uint32 field_val);

shr_error_e utilex_U8_to_U32(
    uint8 *u8_val,
    uint32 nof_bytes,
    uint32 *u32_val);

shr_error_e utilex_U32_to_U8(
    uint32 *u32_val,
    uint32 nof_bytes,
    uint8 *u8_val);

/*
 * } UTILEX_FRAMEWORK_H_INCLUDED
 */
#endif
