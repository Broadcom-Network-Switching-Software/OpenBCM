/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * Tasks information.
 */
#include <appl/diag/dpp/tasks_info.h>

#include <utilities/utils_ip_mgmt.h>

/*
 * INCLUDE FILES:
 */
#ifdef SIM_ON_WINDOWS
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
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
/*
 * Dune chips include file.
 */
#include <appl/diag/dpp/dune_chips.h>
/* } */
#else
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <taskLib.h>
#include <errnoLib.h>
#include <usrLib.h>
#include <tickLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <logLib.h>
#include <pipeDrv.h>
#include <timers.h>
#include <sigLib.h>
#include <cacheLib.h>
#include <drv/mem/eeprom.h>
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include <private/timerLibP.h>
#include <shellLib.h>
#include <dbgLib.h>
/*
 * Definitions specific to reference system.
 */
#include <usrapp.h>
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
/*
 * Dune chips include file.
 */
#include <appl/diag/dpp/dune_chips.h>
/* } */
#endif


#include <appl/diag/dpp/utils_string.h>


#if INCLUDE_NET_SNMP
/* { */


#include <snmp/mib_dir_util.h>
#include <snmp/snmp_files.h>


/*****************************************************
*NAME
*  subject_snmp
*TYPE: PROC
*DATE: 24/JUL/2002
*FUNCTION:
*  Process input line which has an 'snmp' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_snmp(current_line,current_line_ptr)
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
int subject_snmp(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  int           ret, err;
  char          err_msg[80*4] = "";
  char          *proc_name;
  proc_name = "subject_snmp";
  ret = FALSE ;
  /*
   * the rest ot the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'snmp').
     * This should be protected against by the calling environment.
     */
    send_string_to_screen(
      "\r\n\n"
      "*** There are no parameters on the line (just \'snmp\'). SW error.\r\n",TRUE) ;
    ret = TRUE ;
    goto susnmp_exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'snmp').
   */
  {
    unsigned int match_index, num_handled;
    num_handled = 0 ;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SNMP_SHUTDOWN_AGENT_ID,1))
    {
      /*
       * Enter if this is a 'snmp shutdown_snmp_agent' request.
       */
      int counter = 0;
      num_handled++ ;
      send_string_to_screen("\r\n\nShutting down SNMP Agent .", FALSE);
      /*
       * Now lets signal the snmptask that it should reconfigure itself
       * ASAP (SIGHUP is connected to SnmpdReconfig() at the snmp Task)
       */
      err = kill(get_task_id(SNMP_TASK_ID), SIGTERM);
      while (counter < 50)
      {
        if ( !is_task_alive(SNMP_TASK_ID))
        {
          break;
        }
        send_string_to_screen(" .", FALSE);
        d_taskDelay(sysClkRateGet()/5);
        counter++;
      }
      if ( !is_task_alive(SNMP_TASK_ID))
      {
        send_string_to_screen("\r\nSNMP Agent down", TRUE);
      }
      else
      {
        send_string_to_screen("\r\nAfter 10 seconds SNMP Agent still not down", TRUE);
      }
      goto susnmp_exit;
    }
    /*
     */
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SNMP_LOAD_CONFIG_FILE_ID,1))
    {
      /*
       * Enter if this is a 'snmp load_config_file' request.
       */
      char  host_address[16], *config_file =  "snmpd.conf";
      long  long_ip;
      /*
       */
      num_handled++ ;
      long_ip = utils_ip_get_host_ip();
      sprint_ip(host_address, long_ip, FALSE);
      sal_sprintf (err_msg,
               "\r\n"
               "\n"
               "==> Downloading configurstion file %s from host %s",
               config_file, host_address);
      send_string_to_screen(err_msg, TRUE);
      err = load_configuration_file_from_host(host_address, config_file);
      if (OK == err)
      {
        sal_sprintf (err_msg,
                 "\r\n"
                 "==> Succeeded in downloading and saving configuration file to disk (Flash)"
                 );
      }
      else
      {
        sal_sprintf (err_msg,
                 "\r\n"
                 "==> Failed to download or to save configuration file to disk (Flash)"
                 );
      }
      send_string_to_screen(err_msg, TRUE);
      /*
       * Now lets signal the snmptask that it should reconfigure itself
       * ASAP (SIGHUP is connected to SnmpdReconfig() at the snmp Task)
       */
      err = kill(get_task_id(SNMP_TASK_ID), SIGHUP);
      goto susnmp_exit;
    }
    /*
     */
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SNMP_DISP_FILE_ID,1))
    {
      /*
       * Enter if this is a 'snmp display_file' request.
       */
      PARAM_VAL *param_val;
      FILE      *pf;
      char      line[160];
      int       c;
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_SNMP_DISP_FILE_ID, 1,
                        &param_val, VAL_TEXT, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'snmp display_file\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto susnmp_exit ;
      }
      /*
       */
      pf = fopen(param_val->value.val_text, "r");
      if (0 == pf)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'snmp display_file\' error\r\n"
                  "***  unknown file: %s.\r\n", param_val->value.val_text);
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto susnmp_exit ;
      }
      sal_sprintf(line, "\r\n\n"
                    "Printing the content of %s:"
                    "\r\n"
                    "------------------------------------------------------------"
                    "\r\n",
                    param_val->value.val_text);
      send_string_to_screen(line, FALSE);
      while( (c = fgetc(pf)) != EOF)
      {
        if (is_abort_print_flag_set())
        {
          break;
        }
        if (isalnum(c) || isspace(c) || ispunct(c))
        {
          send_char_to_screen((char)c);
        }
        else
        {
          send_char_to_screen('.');
        }
      }
      fclose(pf);
      goto susnmp_exit;
    }
    /*
     */
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SNMP_DIR_ID,1))
    {
      /*
       * Enter if this is a 'snmp dir' request.
       */
      int  i;
      char *conf_file =   CONFIG_FILE_NAME;
      char *pers_file =   PERSIST_FILE_NAME;
      char *mib_name[] =
      {
        IANA_MIB_NAME, ETHERLIKE_MIB_NAME, IF_MIB_NAME, INETADDRESS_MIB_NAME,
        IPFORWARD_MIB_NAME, IP_MIB_NAME, RFC1215_MIB_NAME, RFC1155_MIB_NAME,
        RFC1213_MIB_NAME, SNMPV2CONF_MIB_NAME, SNMPV2_MIB_NAME, SNMPV2SMI_MIB_NAME,
        SNMPV2TC_MIB_NAME, SNMPV2TM_MIB_NAME, TCP_MIB_NAME, UDP_MIB_NAME,
        END_OF_MIBS_STR
      };
      char mib_file[80];
      char str_to_print[80];
      struct stat tmp_stat;
      /*
       */
      num_handled++ ;
      send_string_to_screen("\r\n\n"
                            "Directory of SNMP:\\\r\n\n"
                            " Size  | Path\r\n"
                            "-------+--------------------------------------",
                            TRUE);
      if ( !stat(conf_file, &tmp_stat))
      {
        sal_sprintf(str_to_print, " %-6lu| %s", tmp_stat.st_size, conf_file);
        send_string_to_screen(str_to_print, TRUE);
      }
      if ( !stat(pers_file, &tmp_stat))
      {
        sal_sprintf(str_to_print, " %-6lu| %s", tmp_stat.st_size, pers_file);
        send_string_to_screen(str_to_print, TRUE);
      }
      for (i=0;;++i)
      {
        if (!strcmp(mib_name[i], END_OF_MIBS_STR))
        {
          break;
        }
        strcpy(mib_file, MIB_DEVICE_NAME);
        UTILS_STRCAT_SAFE(mib_file,mib_name[i], 80) ;
        if ( !stat(mib_file, &tmp_stat))
        {
          sal_sprintf(str_to_print, " %-6lu| %s", tmp_stat.st_size, mib_file);
          send_string_to_screen(str_to_print, TRUE);
        }
      }
      goto susnmp_exit;
    }
  }
susnmp_exit:
exit:
  return (ret) ;
}

/* } INCLUDE_NET_SNMP */
#endif


