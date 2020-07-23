/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * INCLUDE FILES:
 */
#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* } */
#else
/* { */
#include <private/stdioP.h>
#include <inetLib.h>

/* } */
#endif

/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defx.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_skt.h>


#include <Skt/skts.h>
#include <Skt/skt_test.h>
#include <DCL/dcl_skt.h>

static
  char *
    action2str[] = {
      "none",
      "set",
      "start",
      "stop",
      "suspend",
      "resume",
      "show",
      "clear",
} ;

STATIC
void
log_action(TEST_ACTION_T action, SKT_TEST *test_ptr)
{
  unsigned char
      buf[INET_ADDR_LEN],
      log_msg[80];
  struct in_addr in;

  in.s_addr = test_ptr->u.client_test.server_ip;
  inet_ntoa_b(in, buf);
  sal_sprintf(log_msg, "%s %s test on ip %s port %u",
          action2str[action],
          test_ptr->is_server ? "server" : "client",
          buf,
          test_ptr->server_port);

  d_syslog(SVR_MSG, log_msg);

}

/*****************************************************
*NAME
*  subject_skt
*TYPE: PROC
*DATE: 06/MAR/2005
*FUNCTION:
*  Process input line which has a 'skt' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_skt(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_skt(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    ret ;
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4];
  TEST_ACTION_T
    action = TEST_ACTION_NONE;
  unsigned long
      test_id;
  SKT_TEST *
    test_ptr;
  static BOOL
    initiated = FALSE;

  ret = FALSE ;

  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'vx_shell').
     * This should be protected against by the calling environment.
     */
    send_string_to_screen(
      "\r\n\n"
      "*** There are no parameters on the line (just \'skt\'). SW error.\r\n",TRUE) ;
    send_string_to_screen(err_msg,TRUE) ;
    ret = TRUE ;
    goto suvs_exit ;
  }
  else
  {
    /*
     * Enter if there are parameters on the line (not just 'mem').
     */
    unsigned
      int
        match_index ;

    if (!get_val_of(
                  current_line,&match_index,PARAM_SKT_TEST_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
    {
      /*
       * Enter if 'test' is on the line.
       */
      if (!initiated) {
        skt_test_module_init();
        initiated = TRUE;
      }

      test_id = param_val->value.ulong_value;
      test_ptr = skt_test_get_by_id(test_id);
      if (!test_ptr)
      {
        send_string_to_screen(
          "\r\n\n"
          "*** Illegal test ID.\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto suvs_exit ;
      }

      /* Get the test action: set/start/stop/suspend/resume/show/clear */
      if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_SKT_SET_ID,1))
      {
        action = TEST_ACTION_SET;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_SKT_START_ID,1))
      {
        action = TEST_ACTION_START;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_SKT_STOP_ID,1))
      {
        action = TEST_ACTION_STOP;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_SKT_SUSPEND_ID,1))
      {
        action = TEST_ACTION_SUSPEND;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_SKT_RESUME_ID,1))
      {
        action = TEST_ACTION_RESUME;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_SKT_SHOW_ID,1))
      {
        action = TEST_ACTION_SHOW;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_SKT_CLEAR_ID,1))
      {
        action = TEST_ACTION_CLEAR;
      }

      if (TEST_ACTION_SET == action)
      {

        /* Test can not be modified when it is running */
        if (test_ptr->valid &&
            ((TEST_STATUS_RUNNING == test_ptr->status) ||
             (TEST_STATUS_SUSPENDED == test_ptr->status)))
        {
          sal_sprintf(err_msg, "\r\n\n*** Test %d is running and can not be modified.\r\n",
                  (int)test_id);
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto suvs_exit ;
        }
        test_ptr->valid = TRUE;
        test_ptr->id = test_id;

        /* Read mandatory common parameters for both client & server */
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SKT_SERVER_PORT_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
        {
          test_ptr->server_port = param_val->value.ulong_value ;
        }

        /* Read optional common parameters for both client & server */
        if (!search_param_val_pairs(
            current_line,&match_index,PARAM_SKT_LOG_ID,1))
        {
          test_ptr->should_log = TRUE;
        }

        /* Check it test should run as a client or as a server */
        if (!search_param_val_pairs(
            current_line,&match_index,PARAM_SKT_CLIENT_ID,1))
        {
          test_ptr->is_server = FALSE;
          /* Read client specific cmd line params */
          if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SKT_SERVER_IP_ID,1,
              &param_val,VAL_IP,err_msg))
          {
            test_ptr->u.client_test.server_ip = param_val->value.ip_value ;
          }
          if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SKT_NOF_PACKETS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
          {
            test_ptr->u.client_test.nof_packets = param_val->value.ulong_value ;
          }
          if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SKT_WAIT_FOR_REPLY_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
          {
            test_ptr->u.client_test.wait_for_reply = param_val->value.ulong_value ;
          }
          if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SKT_PAYLOAD_SIZE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
          {
            test_ptr->u.client_test.payload_size = param_val->value.ulong_value ;
          }
          if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SKT_IPG_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
          {
            test_ptr->u.client_test.ipg = param_val->value.ulong_value ;
          }
        }
        else if (!search_param_val_pairs(
                current_line,&match_index,PARAM_SKT_SERVER_ID,1))
        {
          test_ptr->is_server = TRUE;
          /* Read server specific cmd line params */
          if (!search_param_val_pairs(
              current_line,&match_index,PARAM_SKT_ECHO_ID,1))
          {
            test_ptr->u.server_test.should_echo = TRUE;
          }
        }
      } /* TEST_ACTION_SET */

      else if (TEST_ACTION_CLEAR == action)
      {
        if (skt_test_clear(test_id))
        {
          sal_sprintf(err_msg, "\r\n\n*** Failed to clear Test %d.\r\n",
                  (int)test_id);
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto suvs_exit ;
        }
      } /* TEST_ACTION_CLEAR */

      else if (TEST_ACTION_SHOW == action)
      {
        skt_test_print(test_id);
      } /* TEST_ACTION_SHOW */

      else if (TEST_ACTION_START == action)
      {
        if (skt_test_start(test_id))
        {
          sal_sprintf(err_msg, "\r\n\n*** Failed to start Test %d.\r\n",
                  (int)test_id);
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto suvs_exit ;
        }
      } /* TEST_ACTION_START */

      else if (TEST_ACTION_STOP == action)
      {
        if (skt_test_stop(test_id))
        {
          sal_sprintf(err_msg, "\r\n\n*** Failed to stop Test %d.\r\n",
                  (int)test_id);
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto suvs_exit ;
        }
      } /* TEST_ACTION_STOP */

      else if (TEST_ACTION_SUSPEND == action)
      {
        if (skt_test_suspend(test_id))
        {
          sal_sprintf(err_msg, "\r\n\n*** Failed to suspend Test %d.\r\n",
                  (int)test_id);
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto suvs_exit ;
        }
      } /* TEST_ACTION_SUSPEND */

      else if (TEST_ACTION_RESUME == action)
      {
        if (skt_test_resume(test_id))
        {
          sal_sprintf(err_msg, "\r\n\n*** Failed to resume Test %d.\r\n",
                  (int)test_id);
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto suvs_exit ;
        }
      } /* TEST_ACTION_RESUME */

      if (test_ptr->should_log &&
          ((TEST_ACTION_START == action) ||
           (TEST_ACTION_STOP == action) ||
           (TEST_ACTION_SUSPEND == action) ||
           (TEST_ACTION_RESUME == action))
          )
      {
        log_action(action, test_ptr);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SKT_DCL_STATUS_ID,1))
    {
      dcl_skt_status_print();
    }
  }
suvs_exit:
  return (ret) ;
}


