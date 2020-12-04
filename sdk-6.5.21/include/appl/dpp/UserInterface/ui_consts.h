/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_CONSTS_H_INCLUDED__
/* { */
#define __UI_CONSTS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif


/*
 * Special editing characters: ASCII codes.
 */
/*
 * CTRL-A
 */
#define DEBUG_INFO             0x01
/*
 * CTRL-B
 */
/*
 * CTRL-C
 */
#define CTRL_C                 0x03
#define FLUSH_STD_OUTPUT       CTRL_C
/*
 * Ctrl-e. Enter 'no echo' mode
 */
#define CTRL_E                 0x05
#define ENTER_ECHO_MODE_OFF    CTRL_E
/*
 * Ctrl-F. Enter 'echo' mode
 */
#define CTRL_F                 0x06
#define ENTER_ECHO_MODE_ON     CTRL_F
/*
 * CTRL-H
 */
#define BACK_SPACE             0x08
/*
 * CTRL-I
 */
#define TAB                    0x09
/*
 * CTRL-J
 */
#define LINE_FEED              0x0A
/*
 * CTRL-K
 */
#define CLEAR_LINE             0x0B
/*
 * CTRL-M
 */
#define CARRIAGE_RETURN        0x0D
/*
 * CTRL-T
 */
#define CLEAR_TYPE_AHEAD       0x14
/*
 * Ctrl-U. Equivalent of 'up-arrow'
 */
#define CTRL_U                 0x15
#define CTRL_H                 0x48

/*
 * Ctrl-d. Equivalent of 'down-arrow'
 */
#define CTRL_D                 0x04
#define CTRL_P                 0x50

#define DOWN_ARROW             CTRL_H
#define UP_ARROW               CTRL_P

#define CTRL_Q                 0x11
#define RESTART_TERMINAL       CTRL_Q
#define EXIT_TERMINAL          '`'

/*
 * Escape key
 */
#define ESC                    0x1B
/*
 * Space
 */
#define BLANK_SPACE            0x20
/*
 * Quotation mark (for text with spaces)
 */
#define QUOTATION_MARK         0x22
/*
 * Dollar sign (special character for marking space and
 * for marking specila fields)
 */
#define SPECIAL_DOLLAR         0x24
#define SPECIAL_DOLLAR_STRING  "$"
/*
 * Single quotation mark (for text within text)
 */
/*
 * Question mark (for help)
 */
#define QUESTION_MARK          0x3F
/*
 * DEL key (on PC)
 */
#define DEL_KEY                0x7F
/*
 * Maximal number of characters in any symbol in this
 * system (including ending null). Must be larger
 * than maximal number of characters in any field
 * on current line.
 */
#define MAX_SIZE_OF_SYMBOL      50
/*
 * BELL character on ansi chracter table.
 */
#define BELL_CHAR               ((char)(0x7))
/*
 * Maximal number of characters in text input variable
 * (including ending null).
 */
#define MAX_SIZE_OF_TEXT_VAR           140
/*
 * Maximal number of parameter strings on one line.
 */
#define MAX_PARAM_NAMES_ON_LINE 50
/*
 * Maximal number of parameters per one subject.
 * This system will only accept that many parameters
 * from input rom arrays.
 */
#define MAX_PARAMS_PER_SUBJECT         2800
/*
 * Maximal number of values per one parameter.
 * This system will only accept that many values
 * from input rom arrays.
 */
#define MAX_VALUES_PER_PARAM           400
/*
 * Maximal number of characters per help one value.
 */
#define MAX_CHAR_PER_HELP_PER_VALUE    (80*3)
/*
 * Maximal number of subjects in this system.
 * This system will only accept that much subjects
 * from input rom arrays.
 */
#define MAX_SUBJECTS_IN_UI             100
/*
 * Maximal number of parameter strings and corresponding
 * values on one line.
 */
#define MAX_PARAMS_VAL_PAIRS_ON_LINE   300
/*
 * Maximal number of times a value can be repeated
 * for one parameter (e.g. 'data' on 'mem write' command).
 */
#define MAX_REPEATED_PARAM_VAL         300
/*
 * The size of screen (terminal) line.
 */
#define MAX_CHARS_ON_SCREEN_LINE       79
/*
 * Maximal number of characters acceptable on one line.
 * One line is 4 screen lines
 */
#define MAX_CHARS_ON_LINE    MAX_CHARS_ON_SCREEN_LINE*15
/*
 * Maximal number of characters on a null terminated string
 * representing IP address. Add 2 bytes as spare.
 */
#define MAX_IP_STRING (4*3+3+1+2)
#define BROADCAST_IP 0xFFFFFFFF
#define INVALID_UNICAST_IP  (BROADCAST_IP)
#define DEFAULT_DOWNLOAD_IP (INVALID_UNICAST_IP)


#ifdef  __cplusplus
}
#endif


/* } __UI_CONSTS_H_INCLUDED__*/
#endif
