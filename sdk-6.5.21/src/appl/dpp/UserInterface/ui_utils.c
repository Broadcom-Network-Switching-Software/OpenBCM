/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

 /*
 * INCLUDE FILES:
 */
#include <appl/diag/dpp/ref_sys.h>
#include <appl/diag/dpp/utils_string.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#ifdef __DUNE_HRP__
  #include <DHRP/dhrp_defs.h>
#endif

#if (defined(LINUX) || defined(UNIX))
  #define min(a,b)    (((a) < (b)) ? (a) : (b))

  #include <unistd.h>
#else
  #if !defined(__DUNE_GTO_BCM_CPU__)
  #include <appl/diag/dpp/utils_nvram_configuration.h>
  #endif
#endif

#ifdef WIN32
  #include <bcm_app/dpp/../H/usrApp.h>
#endif


/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <appl/diag/dpp/utils_defx.h>

STATUS
  ioctl(
    int fd,
    int fio,
    int read_count
  );

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

  #define MAX_FILENAME_LENGTH (256)
#if (defined(LINUX) || defined(UNIX))
  #define STD_IN (0)
#endif
  #define FIOGETNAME (0)    /*TODO!! - fix*/

  int take_screen_semaphore(void) {return 0;}
  int give_screen_semaphore(void) {return 0;}

  void
    start_print_services(
      void
    )
  {
    take_screen_semaphore() ;
    return;
  }

  void
    end_print_services(
      void
    )
  {
    give_screen_semaphore() ;
    /*
     * Display CLI Prompt
     */ 
    display_cli_prompt() ;
    return;
  }

/* } */

#ifndef __DUNE_SSF__
  #ifdef SAND_LOW_LEVEL_SIMULATION
  /* { */
#if !DUNE_BCM
  #include <io.h>
  #include <appl/diag/dpp/utils_line_TGS.h>
  #include <appl/diag/dpp/utils_line_FTG.h>
#endif
  /* } SAND_LOW_LEVEL_SIMULATION */
  #endif

  #if LINK_PETRA_LIBRARIES
    #include <appl/diag/dpp/utils_line_PTG.h>
  #endif
  #if LINK_TIMNA_LIBRARIES || LINK_T20E_LIBRARIES
    #include <appl/diag/dpp/utils_line_TEVB.h>
  #endif
#endif

/*
 * Static variables for ui interpreter package.
 * {
 */
/*
 * }
 */

/*****************************************************
*NAME
*  fill_blanks
*TYPE: PROC
*DATE: 25/FEB/2002
*FUNCTION:
*  Add the specified number of blanks to input string
*  (starting at ending null). Resultant string ends,
*  of course, with the standard null.
*CALLING SEQUENCE:
*  fill_blanks(in_str,num,options)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *in_str -
*      String to handle.
*    unsigned int num -
*      Number of blanks to add or, otherwise,
*      see 'options'.
*    unsigned long options -
*      Bit map of control options:
*        0 - Just add blanks
*        1 - Make sure there are 'num
*            characters beyond last \r or \n
*            and, if there are not, fill with
*            blanks.
*        2 - Add blanks so that the total length
*            of the string is 'num'. If current
*            length is larger than/ equal to
*            'num' then no action is taken.
*        Otherwise - same as '0'.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Updated string.
*REMARKS:
*  Calling procedure is assumed to make sure there is
*  enough memory for added spaces on input string.
*SEE ALSO:
 */
void
  fill_blanks(
    char          *in_str,
    unsigned int  num,
    unsigned long options
  )
{
  unsigned
    int
      index ;
  char
    *c_ptr,
    *d_ptr ;
  switch (options)
  {
    case 1:
    {
      c_ptr = strrchr(in_str,(int)'\r') ;
      if (c_ptr == (char *)0)
      {
        c_ptr = in_str ;
      }
      d_ptr = strrchr(c_ptr,(int)'\n') ;
      if (d_ptr == (char *)0)
      {
        d_ptr = c_ptr ;
      }
      d_ptr++ ;
      index = strlen(d_ptr) ;
      if (index < num)
      {
        memset(&d_ptr[index],' ',num - index) ;
        d_ptr[num] = 0 ;
      }
      break ;
    }
    case 2:
    {
      index = strlen(in_str) ;
      if (index < num)
      {
        memset(&in_str[index],' ',num - index) ;
        in_str[num] = 0 ;
      }
      break ;
    }
    case 0:
    default:
    {
      index = strlen(in_str) ;
      memset(&in_str[index],' ',num) ;
      in_str[index+num] = 0 ;
      break ;
    }
  }
  return ;
}

/*****************************************************
*NAME
*  get_rotating_stick
*TYPE: PROC
*DATE: 26/APR/2002
*FUNCTION:
*  Return a rotating stick character: Change returned
*  character every invocation.
*CALLING SEQUENCE:
*  get_rotating_stick()
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    Local static indication on last index.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    char -
*      Rotating stick character.
*  SOC_SAND_INDIRECT:
*    Local static indication on last index.
*REMARKS:
*  This procedure is non-reentrant, of course.
*SEE ALSO:
 */
char
  get_rotating_stick(
    void
    )
{
  char
    ret ;
  static
    unsigned
      int
        last_mark ;
  last_mark++ ;
  if (last_mark > 3)
  {
    last_mark = 0 ;
  }
  switch (last_mark)
  {
    case 0:
    {
      ret = '|' ;
      break ;
    }
    case 1:
    {
      ret = '/' ;
      break ;
    }
    case 2:
    {
      ret = '-' ;
      break ;
    }
    default:
    case 3:
    {
      ret = '\\' ;
      break ;
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  get_blank_line
*TYPE: PROC
*DATE: 26/APR/2002
*FUNCTION:
*  Get a pointer to a blank line, of the size used
*  in this system.
*CALLING SEQUENCE:
*  get_blank_line()
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    Local static indication on initialized line.
*    MAX_CHARS_ON_LINE
*OUTPUT:
*  SOC_SAND_DIRECT:
*    char *-
*      Pointer to blank line
*  SOC_SAND_INDIRECT:
*    Local static indication on initialized line.
*    Local image of line.
*REMARKS:
*  This procedure is non-reentrant, of course.
*SEE ALSO:
 */
char
  *get_blank_line(
    void
    )
{
  char
    *ret ;
  static
    char
      blank_line[MAX_CHARS_ON_LINE + 1] ;
  static
    unsigned
      int
        line_initialized = 0 ;
  if (!line_initialized)
  {
    line_initialized = 1 ;
    blank_line[0] = 0 ;
    fill_blanks(blank_line,MAX_CHARS_ON_SCREEN_LINE,0) ;
  }
  ret = &blank_line[0] ;
  return (ret) ;
}

/*****************************************************
*NAME
*  is_params_on_line
*TYPE: PROC
*DATE: 27/JAN/2002
*FUNCTION:
*  Check whether specified parameter is already
*  on current line.
*CALLING SEQUENCE:
*  is_params_on_line(param_name,current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    char         *param_name -
*      Pointer to parameter to check.
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then parameter has already been
*      entered on current line.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
int
  is_params_on_line(
    const char   *param_name,
    CURRENT_LINE *current_line
  )
{
  int
    ret ;
  const char
    *par_name ;
  unsigned
    int
      num_param_names,
      ui ;
  PARAM
    **parameters ;
  num_param_names = current_line->num_param_names ;
  parameters = &current_line->parameters[0] ;
  par_name = param_name ;
  ret = FALSE ;
  for (ui = 0 ; ui < num_param_names ; ui++,parameters++)
  {
    if (strcmp(par_name,(*parameters)->par_name) == 0)
    {
      /*
       * Parameter has been found! Quit.
       */
      ret = TRUE ;
      break ;
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  is_params_mutex
*TYPE: PROC
*DATE: 19/FEB/2002
*FUNCTION:
*  Check whether specified parameter may be added
*  to other parameters already on current line.
*CALLING SEQUENCE:
*  is_params_mutex(match_index,current_line,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int  match_index -
*      Index, in array of allowed parameters on
*      current line, of parameter to check.
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    char         *err_msg -
*      Pointer to char. To be loaded by
*      error message produced by this procedure.
*      Start and end this message with "\r\n".
*      It is assumed that caller has assigned
*      enough memory so as required by this procedure.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then another parameter, which
*      effectively blocks this parameter, has been
*      found on current line.
*  SOC_SAND_INDIRECT:
*    See err_msg.
*REMARKS:
*
*SEE ALSO:
 */
int
  is_params_mutex(
    unsigned int  match_index,
    CURRENT_LINE *current_line,
    char         *err_msg
  )
{
  int
    ret ;
  unsigned
    long
      zero_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL],
      input_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL],
      summary_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL],
      current_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL] ;
  unsigned
    int
      ui,
      uj,
      num_elements,
      param_match_index ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  PARAM
    *allowed_params ;
  /*
   * Just making sure...
   */
  err_msg[0] = 0 ;
  ret = FALSE ;
  allowed_params =
        current_line->line_subject->allowed_params ;
  param_val_pair = current_line->param_val_pair ;
  memset(zero_mutex_control,0,sizeof(zero_mutex_control)) ;
  memcpy(input_mutex_control,
      allowed_params[match_index].mutex_control,sizeof(input_mutex_control)) ;
  memcpy(summary_mutex_control,input_mutex_control,sizeof(summary_mutex_control)) ;
  for (ui = 0 ; ui < current_line->param_index ; ui++)
  {
    /*
     * Do not check input parameter against itself, of course.
     */
    param_match_index = param_val_pair[ui].param_match_index ;
    if (match_index != param_match_index)
    {
      memcpy(current_mutex_control,
        allowed_params[param_match_index].mutex_control,sizeof(current_mutex_control)) ;
      num_elements = sizeof(summary_mutex_control) / sizeof(summary_mutex_control[0]) ;
      for (uj = 0 ; uj < num_elements ; uj++)
      {
        summary_mutex_control[uj] &= current_mutex_control[uj] ;
      }
      if (memcmp(summary_mutex_control,
            zero_mutex_control,sizeof(summary_mutex_control)) == 0)
      {
        ret = TRUE ;
        sal_sprintf(err_msg,
            "\r\n"
            "*** With current parameters, this parameter (\'%s\') cannot\r\n"
            "    appear on the same line with \'%s\'. Replace or delete...\r\n",
            allowed_params[match_index].par_name,
            allowed_params[param_match_index].par_name) ;
        goto ispa_exit ;
      }
    }
  }
  /*
   * Check the last entry only if parameter string has already been
   * entered.
   */
  if (current_line->param_string_entered)
  {
    param_match_index = param_val_pair[ui].param_match_index ;
    if (match_index != param_match_index)
    {
      memcpy(current_mutex_control,
              allowed_params[param_match_index].mutex_control,sizeof(current_mutex_control)) ;
      num_elements = sizeof(summary_mutex_control) / sizeof(summary_mutex_control[0]) ;
      for (uj = 0 ; uj < num_elements ; uj++)
      {
        summary_mutex_control[uj] &= current_mutex_control[uj] ;
      }
      if (memcmp(summary_mutex_control,
            zero_mutex_control,sizeof(summary_mutex_control)) == 0)
      {
        ret = TRUE ;
        sal_sprintf(err_msg,
            "\r\n"
            "*** With current parameters, this parameter (\'%s\') cannot\r\n"
            "    appear on the same line with \'%s\'. Replace or delete...\r\n",
            allowed_params[match_index].par_name,
            allowed_params[param_match_index].par_name) ;
        goto ispa_exit ;
      }
    }
  }
  /*
   * At this point, no mutual exclusion problem has been found.
   */
ispa_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  help_for_match
*TYPE: PROC
*DATE: 26/APR/2002
*FUNCTION:
*  Print help for all parameter values of type
*  VAL_SYMBOL which match input string entered
*  so far on last field.
*CALLING SEQUENCE:
*  help_for_match(
*       field_string,num_loaded,parameter_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    char         *field_string -
*      Pointer to the beginning of the string
*      entered so far on current line. NOT
*      null terminated.
*    unsigned int num_loaded -
*      Number of characters pointed by field_string.
*    PARAM        *parameter_ptr -
*      Pointer to corresponding parameter, for which
*      the symbolic value is being entered.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then operation could not be carried
*      out. Otherwise, completion was successful and
*      help has been printed.
*  SOC_SAND_INDIRECT:
*    Printed help text.
*REMARKS:
*  At least one synbolic value is assumed to be on
*  the list of values of input parameter.
*SEE ALSO:
 */
int
  help_for_match(
    char         *field_string,
    unsigned int num_loaded,
    PARAM        *parameter_ptr
  )
{
  int
    ret ;
  unsigned
    int
      uk ;
  PARAM_VAL_RULES
    *param_val_rules ;
  char
    help_buffer[80*15] ;
  ret = FALSE ;
  param_val_rules = parameter_ptr->allowed_vals ;
  for (uk = 0 ; uk < parameter_ptr->num_allowed_vals ; uk++,param_val_rules++)
  {
    if ((param_val_rules->val_type & VAL_SYMBOL) == VAL_SYMBOL)
    {
      if (strlen(param_val_rules->symb_val) >= num_loaded)
      {
        if (memcmp(param_val_rules->symb_val,field_string,num_loaded) == 0)
        {
          help_buffer[0] = 0 ;
          sal_sprintf(help_buffer,"%02u) ",(unsigned short)(uk + 1)) ;
          UTILS_STRCAT_SAFE(help_buffer,"String variable: \'", 90*15) ;
          UTILS_STRCAT_SAFE(help_buffer,param_val_rules->symb_val, 90*15) ;
          UTILS_STRCAT_SAFE(help_buffer,"\' .", 90*15) ;
          if (strlen(param_val_rules->val_descriptor.val_symb_descriptor.plain_text_help))
          {
            UTILS_STRCAT_SAFE(help_buffer,"\r\n", 90*15) ;
            UTILS_STRCAT_SAFE(help_buffer,
              param_val_rules->val_descriptor.val_symb_descriptor.plain_text_help, 90*15) ;
          }
          send_string_to_screen(help_buffer,TRUE) ;
        }
      }
    }
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  params_fully_loaded
*TYPE: PROC
*DATE: 20/FEB/2002
*FUNCTION:
*  Check whether parameters on input line form a
*  complete list, ready for processing (i.e. no
*  parameter is missing, by preset rules).
*CALLING SEQUENCE:
*  params_fully_loaded(current_line,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    char         *err_msg -
*      Pointer to char. To be loaded by
*      error message produced by this procedure.
*      Start and end this message with "\r\n".
*      It is assumed that caller has assigned
*      enough memory so as required by this procedure.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some parameters are
*      missing, to make the line meaningful.
*  SOC_SAND_INDIRECT:
*    See err_msg.
*REMARKS:
*
*SEE ALSO:
 */
int
  params_fully_loaded(
    CURRENT_LINE *current_line,
    char         *err_msg
  )
{
  int
    ret ;
  unsigned
    long
      zero_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL],
      compare_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL],
      input_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL],
      summary_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL],
      current_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL] ;
  unsigned
    int
      first_pair_match_index,
      num_allowed_params,
      ui,
      uj,
      num_elements,
      uk,
      group_ok,
      param_match_index ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  PARAM
    *last_err_param,
    *allowed_params,
    *allowed_params_ptr ;
  unsigned
    char
      selected_params[MAX_PARAMS_PER_SUBJECT] ;
  /*
   * Just making sure...
   */
  err_msg[0] = 0 ;
  ret = FALSE ;
  group_ok = 0;
  last_err_param = NULL;
  if (!(current_line->subject_entered))
  {
    goto pafu_exit ;
  }
  /*
   * Subject has been entered. Check whether there are parameters.
   * If there are none, but subject, alone, is sufficient, then
   * indicate 'OK'. Otherwise, indicate error.
   */
  if (current_line->num_param_names == 0)
  {
    if (!(current_line->line_subject->may_stand_alone))
    {
      ret = TRUE ;
      sal_sprintf(err_msg,
        "\r\n"
        "*** Parameters are required on a line with this subject (\'%s\').\r\n"
        "    Add parameters..."
        "\r\n",
        current_line->line_subject->subj_name) ;
    }
    goto pafu_exit ;
  }
  num_allowed_params = current_line->line_subject->num_allowed_params ;
  allowed_params =
        current_line->line_subject->allowed_params ;
  param_val_pair = current_line->param_val_pair ;
  /*
   * Load 'first_pair_match_index' by index of the parameter
   * indicated on the first 'param_val_pair'. There must
   * be one since at least one parameter name has been
   * recorded.
   */
  first_pair_match_index = param_val_pair[0].param_match_index ;
  /*
   * Mark every parameter which matches the "common group"
   * search criterion.
   */
  memset(selected_params,0,sizeof(selected_params)) ;
  selected_params[first_pair_match_index] = TRUE ;
  memset(zero_mutex_control,0,sizeof(zero_mutex_control)) ;
  memcpy(input_mutex_control,
    allowed_params[first_pair_match_index].mutex_control,sizeof(input_mutex_control)) ;
  memcpy(summary_mutex_control,input_mutex_control,sizeof(summary_mutex_control)) ;
  for (ui = 0 ; ui < current_line->param_index ; ui++)
  {
    /*
     * Do not check input parameter against itself, of course.
     */
    param_match_index = param_val_pair[ui].param_match_index ;
    if (first_pair_match_index != param_match_index)
    {
      memcpy(current_mutex_control,
          allowed_params[param_match_index].mutex_control,sizeof(current_mutex_control)) ;
      num_elements = sizeof(summary_mutex_control) / sizeof(summary_mutex_control[0]) ;
      for (uk = 0 ; uk < num_elements ; uk++)
      {
        summary_mutex_control[uk] &= current_mutex_control[uk] ;
      }
      if (memcmp(summary_mutex_control,
            zero_mutex_control,sizeof(summary_mutex_control)) == 0)
      {
        /*
         * The mutual exclusion test is redundant here since it
         * is supposed to have been carried out before this line has
         * become candidate for proccesing, but we get it for free...
         */
        ret = TRUE ;
        sal_sprintf(err_msg,
            "\r\n"
            "*** With current parameters, first parameter (\'%s\') cannot\r\n"
            "    appear on the same line with \'%s\'. Replace or delete...\r\n",
            allowed_params[first_pair_match_index].par_name,
            allowed_params[param_match_index].par_name) ;
        goto pafu_exit ;
      }
      selected_params[param_match_index] = TRUE ;
    }
  }
  /*
   * Check the last entry only if parameter string has already been
   * entered.
   */
  if (current_line->param_string_entered)
  {
    param_match_index = param_val_pair[ui].param_match_index ;
    if (first_pair_match_index != param_match_index)
    {

      memcpy(current_mutex_control,
          allowed_params[param_match_index].mutex_control,sizeof(current_mutex_control)) ;
      num_elements = sizeof(summary_mutex_control) / sizeof(summary_mutex_control[0]) ;
      for (uk = 0 ; uk < num_elements ; uk++)
      {
        summary_mutex_control[uk] &= current_mutex_control[uk] ;
      }
      if (memcmp(summary_mutex_control,
            zero_mutex_control,sizeof(summary_mutex_control)) == 0)
      {
        ret = TRUE ;
        sal_sprintf(err_msg,
            "\r\n"
            "*** With current parameters, first parameter (\'%s\') cannot\r\n"
            "    appear on the same line with \'%s\'. Replace or delete...\r\n",
            allowed_params[first_pair_match_index].par_name,
            allowed_params[param_match_index].par_name) ;
        goto pafu_exit ;
      }
      selected_params[param_match_index] = TRUE ;
    }
  }
  /*
   * At this point, no mutual exclusion problem has been found.
   * 'selected_params' now contains a non-zero value on all entries
   * which have been selected as belonging to this group.
   */
  /*
   * Check the list of remaining allowed parameters to see whether
   * other parameters are required.
   * The rule is as follows: If at least one group can be closed
   * with no more parameters (or with parameters that, all, have
   * default values), then this line is acceptable. Also,
   * If processing state is 'expecting_enter' then this line
   * is acceptable. Otherwise, some parameter is missing.
   * At this point, 'summary_mutex_control' contains all the
   * groups that should be considered (Usually, there would
   * only be one bit set, at this point. In any case, at least
   * one bit must be set, at this point).
   */
  if (current_line->proc_state == EXPECTING_ENTER)
  {
    goto pafu_exit ;
  }
  for (ui = 0 ; ui < (sizeof(summary_mutex_control)*BITS_IN_BYTE) ; ui++)
  {
    uk = ui / (sizeof(summary_mutex_control[0])*BITS_IN_BYTE) ;
    compare_mutex_control[uk] =
        summary_mutex_control[uk] &
            BIT(ui % (sizeof(summary_mutex_control[0])*BITS_IN_BYTE)) ;
    if (compare_mutex_control[uk])
    {
      /*
       * An active group has been found. check whether there
       * are more parameters on the 'allowed' list which
       * belong to this group.
       */
      group_ok = TRUE ;
      allowed_params_ptr = allowed_params ;
      for (uj = 0 ; uj < num_allowed_params ; uj++, allowed_params_ptr++)
      {
        if (!selected_params[uj])
        {
          if (allowed_params_ptr->mutex_control[uk] & compare_mutex_control[uk])
          {
            /*
             * Enter if a parameter which is on the current group
             * has been found. If it does not have a default
             * value (and it is not allowed to not be on the line)
             * then this is an error
             */
            if ((allowed_params_ptr->default_care & HAS_DEFAULT_MASK) == HAS_NO_DEFAULT)
            {
              if ((allowed_params_ptr->default_care & MUST_APPEAR_MASK) == MUST_APPEAR)
              {
                /*
                 * Reach here for parameters which are NOT only the line and which
                 * are marked as (a) having no default and (b) must appear on the line.
                 * If they are not in the 'ordinal' ball park or are marked as 'LAST_ORDINAL'
                 * then this is an error.
                 * Otherwise, if their 'ordinal' has already been taken then it is
                 * NOT an error for them to not be on the line.
                 */
                unsigned
                  int
                    um,
                    found ;
                PARAM
                  *allowed_params_ord_ptr ;
                found = FALSE ;
                if ((allowed_params_ptr->ordinal != 0) && (allowed_params_ptr->ordinal != LAST_ORDINAL))
                {
                  allowed_params_ord_ptr = allowed_params ;
                  for (um = 0 ; um < num_allowed_params ; um++, allowed_params_ord_ptr++)
                  {
                    if (selected_params[um])
                    {
                      if (allowed_params_ord_ptr->ordinal == allowed_params_ptr->ordinal)
                      {
                        found = TRUE ;
                        break ;
                      }
                    }
                  }
                }
                if (!found)
                {
                  group_ok = FALSE ;
                  last_err_param = allowed_params_ptr ;
                  break ;
                }
              }
            }
          }
        }
      }
      if (group_ok)
      {
        /*
         * If a full by-the-rules group has been found then quit with
         * ret=FALSE (OK indication).
         */
        goto pafu_exit ;
      }
    }
  }
  /*
   * If this point has been reached then no acceptable group has been found
   * and the last parameter which caused the failure is pointed by
   * 'last_err_param'. 'group_ok' must be 'FALSE' here but use the 'if'
   * clause anyway.
   */
  if (!group_ok)
  {
    ret = TRUE ;
    sal_sprintf(err_msg,
        "\r\n"
        "*** With current parameters, some parameters are missing\r\n"
        "    on this line (for example, \'%s\').\r\n"
        "    Complete filling in the line...\r\n",
        last_err_param->par_name) ;
    goto pafu_exit ;
  }
pafu_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_num_params_on_line
*TYPE: PROC
*DATE: 27/JAN/2002
*FUNCTION:
*  Get number of parameters entered on line so far.
*CALLING SEQUENCE:
*  get_num_params_on_line(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Number of parameters entered on current line.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
int
  get_num_params_on_line(
    CURRENT_LINE *current_line
  )
{
  int
    ret ;
  ret = current_line->num_param_names ;
  return (ret) ;
}
/*****************************************************
*$NAME$
*  send_char_to_screen
*TYPE: PROC
*DATE: 20/JAN/2002
*FUNCTION:
*  Send a single character to the screen (standard out).
*CALLING SEQUENCE:
*  send_char_to_screen(out_char)
*INPUT:
*  SOC_SAND_DIRECT:
*    char out_char -
*      Character to send to the screen
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then action could not be carried
*      out due to the fact that the standard output
*      file descriptor has changed (move from
*      telnet to local terminal or the other way
*      round).
*  SOC_SAND_INDIRECT:
*    Printed character.
*REMARKS:
*  If the standard output file has changed right in the
*  middle of the execution of this 'write' routine
*  then just ignore the input. This character will
*  be lost.
*SEE ALSO:
 */
int
  send_char_to_screen(
    char    out_char
  )
{
  int
    ret ;
  ret = send_array_to_screen(&out_char,1) ;
  return (ret);
}
/*****************************************************
*$NAME$
*  send_array_to_screen
*TYPE: PROC
*DATE: 20/JAN/2002
*FUNCTION:
*  Send an array of characters to the screen
*  (standard out) checking for page mode.
*CALLING SEQUENCE:
*  send_array_to_screen(out_string,num_to_send)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *out_string -
*      Characters array to send to the screen
*    int num_to_send -
*      Number of characters in the array to send.
*      This number is assumed to be smaller than
*      4000. No check is carried out on that.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then action could not be carried
*      out due to the fact that the standard output
*      file descriptor has changed (move from
*      telnet to local terminal or the other way
*      round).
*  SOC_SAND_INDIRECT:
*    Printed character.
*REMARKS:
*  If the standard output file has changed right in the
*  middle of the execution of this 'write' routine
*  then just ignore the input and continue. Some
*  characters may be lost.
*
*  Caller is warned that this may be a very long
*  operation (human user response) so task is
*  opened to delete and restart!
*
*  This procedure use the screen semaphore!!!
*SEE ALSO:
 */
int
  send_array_to_screen(
    const char *const_out_string,
    int  num_to_send
  )
{
  int
    ret,
    ask_response ;
  unsigned
    int
      num_bytes,
      extra_lines,
      num_lines ;
  char
    *c_ptr;
  char
    *out_string;

  out_string = (char*)malloc(num_to_send+1);
  if (out_string == (char *)0)
  {
      c_ptr = "\r\nmalloc() returned with ERROR. (in \'send_array_to_screen\')!!!\r\n" ;
      num_bytes = strlen(c_ptr) ;
      low_level_to_screen(c_ptr,num_bytes) ;
      ret = TRUE ;
      goto exit_free ;
  }

  ret = 0 ;
  memcpy(out_string, const_out_string, num_to_send);
  out_string[num_to_send] = 0;
#if !DUNE_BCM

#else

  d_printf(out_string);
  fflush(NULL);
  goto exit_free;
#endif

#if !DUNE_BCM
  ret = take_screen_semaphore() ;
  if (ret)
  {
    c_ptr = "\r\nFailed to take screen semaphore (in \'send_array_to_screen\')!!!\r\n" ;
    num_bytes = strlen(c_ptr) ;
    low_level_to_screen(c_ptr,num_bytes) ;
    goto sear_exit ;
  }
 #endif
  if ( is_page_mode_set() && !is_periodic_on())
  {
    while (TRUE)
    {
      if (is_abort_print_flag_set())
      {
        goto sear_exit ;
      }
      num_lines = get_array_num_lines(out_string,num_to_send) ;
      extra_lines = get_page_overflow(num_lines) ;
      if (extra_lines == 0)
      {
        /*
         * Enter if extra lines do not result in overflow state.
         */
        inc_num_lines_printed(num_lines) ;
        ret = low_level_to_screen(out_string,num_to_send) ;
        goto sear_exit ;
      }
      else
      {
        /*
         * Enter if extra lines result in overflow state.
         * Print whatever can be printed and wait for confirmation
         * from the user on continuation.
         */
        extra_lines = get_page_left() ;
        if (extra_lines == 0)
        {
          /*
           * Even if we are at overflow, make sure prompting ends at a line feed.
           */
          extra_lines++ ;
        }
        {
          /*
           * Enter if some more lines may be printed before asking
           * whether to continue.
           */
          c_ptr = get_ptr_of_lf(out_string,extra_lines,num_to_send) ;
          num_bytes = 0 ;
          if (c_ptr && (c_ptr != out_string))
          {
            num_bytes = c_ptr - out_string ;
          }
          /*
           * Print whatever can still be printed before overflow.
           */
          ret = low_level_to_screen(out_string,num_bytes) ;
          if (ret)
          {
            goto sear_exit ;
          }
          num_to_send -= num_bytes ;
          out_string += num_bytes ;
        }
        {
          c_ptr = "Hit \'space\' to continue, any other key to abort ==>" ;
          /*
           * Since this is going to be a very long operation
           * (human user response) then make the task open
           * to delete and restart (and, after getting response,
           * get back to original state.
           */
          ask_response = ask_get(c_ptr,EXPECT_CONT_ABORT,TRUE,TRUE) ;
          clear_num_lines_printed() ;
          if (ask_response == 0)
          {
            set_abort_print_flag() ;
            goto sear_exit ;
          }
        }
      }
    }
  }
  else
  {
    if (is_abort_print_flag_set())
    {
      {
        /*
         * Display dynamic 'wait' message for cases where print
         * is a long process.
         */
        static
          unsigned
            long
              last_disp_time_in_10ms = 0 ;
        unsigned
          long
            current_disp_time_in_10ms ;
        static
          unsigned
            long
              last_disp_count = 0,
              num_disp_steps = 10 ;
        char
          *loc_string ;
        current_disp_time_in_10ms = get_watchdog_time(TRUE) ;
        /*
         * Change display every 800 ms.
         */
        if ((current_disp_time_in_10ms - last_disp_time_in_10ms) > 80)
        {
          last_disp_time_in_10ms = get_watchdog_time(TRUE) ;
          switch (last_disp_count)
          {
            case 0:
            {
              loc_string = "\r                  " ;
              low_level_to_screen(loc_string,strlen(loc_string)) ;
              loc_string = "\rWAIT" ;
              low_level_to_screen(loc_string,strlen(loc_string)) ;
              break ;
            }
            default:
            {
              loc_string = "." ;
              low_level_to_screen(loc_string,strlen(loc_string)) ;
              break ;
            }
          }
          last_disp_count++ ;
          if (last_disp_count >= num_disp_steps)
          {
            last_disp_count = 0 ;
          }
        }
      }
      goto sear_exit ;
    }
    ret = low_level_to_screen(out_string,num_to_send) ;
  }
sear_exit:
  ret = give_screen_semaphore() ;
  if (ret)
  {
    c_ptr = "\r\nFailed to give screen semaphore (in \'send_array_to_screen\')!!!\r\n" ;
    num_bytes = strlen(c_ptr) ;
    low_level_to_screen(c_ptr,num_bytes) ;
  }
  goto exit_free;
exit_free:
  if (out_string) {
	  free(out_string);
  }
  return (ret) ;
}
/*****************************************************
*$NAME$
*  low_level_to_screen
*TYPE: PROC
*DATE: 03/APR/2002
*FUNCTION:
*  Send an array of characters to the screen
*  (standard out).
*CALLING SEQUENCE:
*  send_array_to_screen(out_string,num_to_send)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *out_string -
*      Characters array to send to the screen
*    int num_to_send -
*      Number of characters in the array to send.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then action could not be carried
*      out due to the fact that the standard output
*      file descriptor has changed (move from
*      telnet to local terminal or the other way
*      round).
*  SOC_SAND_INDIRECT:
*    Printed character.
*REMARKS:
*  If the standard output file has changed right in the
*  middle of the execution of this 'write' routine
*  then just ignore the input and continue. Some
*  character may be lost.
*SEE ALSO:
 */
int
  low_level_to_screen(
    char *out_string,
    int  num_to_send
  )
{
  int
    ret ;

/* { */
  {
    unsigned
      int
        ui ;
    char
      *c_ptr ;
    ret = FALSE ;
    c_ptr = out_string ;
    for (ui = 0 ; ui < (unsigned int)num_to_send ; ui++)
    {
      putchar(*c_ptr++) ;
    }
  }
/* } */

  return (ret) ;
}
/*****************************************************
*$NAME$
*  send_string_to_screen
*TYPE: PROC
*DATE: 20/JAN/2002
*FUNCTION:
*  Send a null terminated string of characters
*  to the screen (standard out).
*CALLING SEQUENCE:
*  send_string_to_screen(out_string,add_cr_lf)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *out_string -
*      Character string to send to the screen
*      (null terminated).
*    int add_cr_lf -
*      If 'true' then add CR/LF after the string.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then action could not be carried
*      out due to the fact that the standard output
*      file descriptor has changed (move from
*      telnet to local terminal or the other way
*      round).
*  SOC_SAND_INDIRECT:
*    Printed character.
*REMARKS:
*  If the standard output file has changed right in the
*  middle of the execution of this 'write' routine
*  then just ignore the input and continue. Some
*  character may be lost.
*SEE ALSO:
 */
int
  send_string_to_screen(
    const char *out_string,
    int  add_cr_lf
  )
{
  int
    ret ;
  int
    num_to_send ;
  num_to_send = strlen(out_string) ;
  ret = send_array_to_screen(out_string,num_to_send) ;
  if (ret != ERROR)
  {
    if (add_cr_lf)
    {
      num_to_send = 2 ;
      ret = send_array_to_screen("\r\n",num_to_send) ;
    }
  }
  return (ret) ;
}
/*****************************************************
*$NAME$
*  msg_and_redisplay
*TYPE: PROC
*DATE: 20/JAN/2002
*FUNCTION:
*  Start a new line, send a message (string of
*  characters, null terminated) to the screen
*  (standard out) and then redisplay prompt line.
*  Leave the cursor at its original location on
*  that line.
*CALLING SEQUENCE:
*  msg_and_redisplay(msg_string,current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *msg_string -
*      Message to send to the screen. Must be null
*      terminated.
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Printed character.
*REMARKS:
*  None
*SEE ALSO:
 */
void
  msg_and_redisplay(
    const char         *msg_string,
    CURRENT_LINE *current_line
  )
{
  char
    *prompt_string,
    *prompt_line ;
  int
    num_on_prompt_line;
#if !DUNE_BCM
  take_screen_semaphore() ;
#endif
  prompt_string = current_line->prompt_string ;
  prompt_line = current_line->line_buf ;
  num_on_prompt_line = current_line->num_loaded ;
  send_array_to_screen("\r\n",2) ;
  send_string_to_screen(msg_string,TRUE) ;
  /*
   * Abort_print flag and num_lines_printed MUST be cleared
   * every place the prompt is displayed.
   */
  clear_abort_print_flag() ;
  clear_num_lines_printed() ;
  /*
   * Actually send to screen only if echo mode is on.
   */
  if (is_echo_mode_on())
  {
    send_string_to_screen(prompt_string,FALSE) ;
    send_array_to_screen(prompt_line,num_on_prompt_line) ;
    /*
     * disabled this section when added support for more than one screen line
     *  for CURRENT_LINE. This code assumes rewritng on ONE screen line.
     */
  }
#if !DUNE_BCM
  give_screen_semaphore() ;
#endif
  return ;
}
/*****************************************************
*$NAME$
*  redisplay_line
*TYPE: PROC
*DATE: 22/JAN/2002
*FUNCTION:
*  Go to the beginning of the current prompt line and
*  redisplay it, clearing the rest of the line.
*CALLING SEQUENCE:
*  redisplay_line(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    Current_line.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Printed line.
*REMARKS:
*  None
*SEE ALSO:
 */
void
  redisplay_line(
    CURRENT_LINE *current_line
  )
{
  unsigned
    int
      fill,
      size ;
  char
    line[MAX_CHARS_ON_LINE + 1] ;
#if !DUNE_BCM
  take_screen_semaphore() ;
#endif
  size = strlen(current_line->prompt_string) ;
  memcpy(line,current_line->prompt_string,size) ;
  memcpy(&line[size],current_line->line_buf,current_line->num_loaded) ;
  size += current_line->num_loaded ;
  line[size] = 0 ;
  /*
   * Abort_print flag and num_lines_printed MUST be cleared
   * every place the prompt is displayed.
   */
  clear_abort_print_flag() ;
  clear_num_lines_printed() ;
  /*
   * In case we are typing one screen line we should fill blanks only for this screen line.
   */
  if (size <= current_line->max_chars_on_screen_line)
  {
    fill = current_line->max_chars_on_screen_line - size ;
    fill_blanks(line,fill,0) ;
    /*
     * Actually send to screen only if echo mode is on.
     */
    if (is_echo_mode_on())
    {
      send_char_to_screen('\r') ;
      send_array_to_screen(line,current_line->max_chars_on_screen_line) ;
    }
  }
  /*
   * Actually send to screen only if echo mode is on.
   */
  if (is_echo_mode_on())
  {
    send_char_to_screen('\r') ;
    send_string_to_screen(current_line->prompt_string,FALSE) ;
    send_array_to_screen(current_line->line_buf,current_line->line_buf_index) ;
  }
#if !DUNE_BCM
  give_screen_semaphore() ;
#endif
  return ;
}
/*****************************************************
*$NAME$
*  back_space_line
*TYPE: PROC
*DATE: 22/JAN/2002
*FUNCTION:
*  Go to the beginning of the current prompt line and
*  redisplay it, clearing only one character beyond
*  current end.
*CALLING SEQUENCE:
*  back_space_line(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    Current_line.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Printed line.
*REMARKS:
*  None
*SEE ALSO:
 */
void
  back_space_line(
    CURRENT_LINE *current_line
  )
{
  /*
   * Actually send to screen only if echo mode is on.
   */
  if (is_echo_mode_on())
  {
    if (((current_line->num_loaded +
            strlen(current_line->prompt_string)) %
                      current_line->max_chars_on_screen_line == 0))
    {
      msg_and_redisplay("--",current_line);
    }
    else
    {
      send_array_to_screen("\b \b",3) ;
    }
  }
  return ;
}
/*****************************************************
*$NAME$
*  clear_n_redisplay_line
*TYPE: PROC
*DATE: 23/JAN/2002
*FUNCTION:
*  Clear line by overwriting it with blank spaces. Then,
*  clear input array on current prompt line, load
*  initialization parameters and display clear line with
*  prompt, ready for input.
*CALLING SEQUENCE:
*  clear_n_redisplay_line(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    Current_line.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Printed line.
*REMARKS:
*  If processing state has been 'ECPECTING_PASSWORD'
*  then it remains as is. If processing state has been
*  'RECEIVING_PASSWORD' then it shifts to
*  'ECPECTING_PASSWORD'.
*SEE ALSO:
 */
void
  clear_n_redisplay_line(
    CURRENT_LINE *current_line
  )
{
  unsigned
    int
      proc_state,
      array_to_screen_size ;
#if !DUNE_BCM
  take_screen_semaphore() ;
#endif
  proc_state = current_line->proc_state ;
  memset(current_line->line_buf,' ',current_line->num_loaded) ;
  /*
   * Abort_print flag and num_lines_printed MUST be cleared
   * every place the prompt is displayed.
   */
  clear_abort_print_flag() ;
  clear_num_lines_printed() ;
  /*
   * Actually send to screen only if echo mode is on.
   */
  if (is_echo_mode_on())
  {
    send_char_to_screen('\r') ;
    send_string_to_screen(current_line->prompt_string,FALSE) ;
  }
  array_to_screen_size = min(current_line->num_loaded,
                             (current_line->max_chars_on_screen_line - strlen(current_line->prompt_string)));
  /*
   * Actually send to screen only if echo mode is on.
   */
  if (is_echo_mode_on())
  {
    send_array_to_screen(current_line->line_buf,array_to_screen_size) ;
  }
  init_current_line(current_line) ;
  switch (proc_state)
  {
    case EXPECTING_PASSWORD:
    case RECEIVING_PASSWORD:
    {
      current_line->proc_state = EXPECTING_PASSWORD ;
      break ;
    }
    default:
    {
      proc_state = EXPECTING_SUBJECT ;
      break ;
    }
  }
  /*
   * Actually send to screen only if echo mode is on.
   */
  if (is_echo_mode_on())
  {
    send_char_to_screen('\r') ;
    send_string_to_screen(current_line->prompt_string,FALSE) ;
  }
#if !DUNE_BCM
  give_screen_semaphore() ;
#endif
  return ;
}
/*****************************************************
*NAME
*  get_subj_list_ptr
*TYPE: PROC
*DATE: 20/JAN/2002
*FUNCTION:
*  Get pointer to ASCII list of all subjects in the
*  system, one on each line (i.e., separated by cr/lf).
*CALLING SEQUENCE:
*  get_subj_list_ptr()
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    Subjects_ascii_list
*OUTPUT:
*  SOC_SAND_DIRECT:
*    char * -
*      pointer to the list.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
char
  *get_subj_list_ptr(
    void
  )
{
  char
    *ret ;
  ret = Subjects_ascii_list ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_subj_list_len
*TYPE: PROC
*DATE: 21/JAN/2002
*FUNCTION:
*  Get number of characters in the ascii list of
*  subjects names.
*CALLING SEQUENCE:
*  get_subj_list_len()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    List of all subjects in the system (Subjects_ascii_list).
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Number of subjects in the system.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_subj_list_len(
    void
  )
{
  int
    ret ;
  ret = strlen(Subjects_ascii_list) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_num_subjs
*TYPE: PROC
*DATE: 21/JAN/2002
*FUNCTION:
*  Get number of subjects in the system.
*CALLING SEQUENCE:
*  get_num_subjs()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Num_subjects
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Number of subjects in the system.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_num_subjs(
    void
  )
{
  int
    ret ;
  ret = Num_subjects ;
  return (ret) ;
}
/*****************************************************
*NAME
*  complete_last_field
*TYPE: PROC
*DATE: 22/JAN/2002
*FUNCTION:
*  Complete last field entered on current line using
*  input string. First few characters of input string
*  should be the same as whatever has been entered
*  so far on last field.
*CALLING SEQUENCE:
*  complete_last_field(
*       field_string,current_line,
*       partial_in_string,c_len)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *field_string -
*      String to use to complete last field entered
*      on current line.
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    int partial_in_string -
*      Flag. If 'true' then not the full field will be
*      used for completion but only the first 'c_len'
*      characters.
*    unsigned int c_len -
*      See 'partial_in_string'. This is the number of
*      characters to use out of input field.
*      This parameter is assumed to be smaller than
*      the length of 'field_string'.
*  SOC_SAND_INDIRECT:
*    Cursor is assumed to be at the location ready for
*    completion.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then operation could not be carried
*      out. Otherwise, completion was successful and
*      line has been printed on screen.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  complete_last_field(
    const char         *field_string,
    CURRENT_LINE *current_line,
    int          partial_in_string,
    unsigned int c_len
  )
{
  int
    ret ;
  const char
    *field;
  char
    *line ;
  unsigned
    int
      num_same,
      num_added ;
  ret = FALSE ;
  num_same = current_line->num_loaded_this_field ;
  field = &field_string[num_same] ;
  if (partial_in_string)
  {
    num_added = c_len - num_same ;
  }
  else
  {
    num_added = strlen(field) ;
  }
  /*
   * Partial field on current line has 'num_same'
   * characters. To those, 'num_added' should be added
   * for completion.
   */
  if ((current_line->num_loaded +
          strlen(current_line->prompt_string) + num_added) >=
                                current_line->max_chars_on_line)
  {
    /*
     * Enter if there is no place, on the line, for completed field.
     * Issue warning and return to exactly same state
     * and place.
     */
    if (!is_line_mode_set())
    {
      msg_and_redisplay(
        "\r\n"
        "*** Can not complete. Too many characters on this line.\r\n",
        current_line) ;
    }
    else
    {
      send_string_to_screen(
        "\r\n"
        "*** Can not complete. Too many characters on this line.",
        TRUE
        ) ;
    }
    ret = TRUE ;
  }
  else
  {
    line =
      &current_line->line_buf[
          current_line->start_index_this_field + num_same] ;
    field = &field_string[num_same] ;
    memcpy(line,field,num_added) ;
    if (!is_line_mode_set())
    {
      /*
       * Actually send to screen only if echo mode is on.
       */
      if (is_echo_mode_on())
      {
        send_array_to_screen(line,num_added) ;
      }
    }
    current_line->num_loaded += num_added ;
    current_line->line_buf_index += num_added ;
    current_line->num_loaded_this_field += num_added ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  all_subjs_short_help
*TYPE: PROC
*DATE: 23/JAN/2002
*FUNCTION:
*  For subjects:
*  Go through the list of subjects on this system and
*  display short help on each.
*  For parameters:
*  Go through the list of parameters related to the
*  subject on current line and display short help
*  on each.
*CALLING SEQUENCE:
*  all_subjs_short_help(current_line,for_subjects)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    int for_subjects -
*      Flag. If 'true' print help for subjects.
*      Otherwise, print help for parameters.
*  SOC_SAND_INDIRECT:
*    List of all subjects in the system (Subjects_list).
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Printed short help text.
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  all_subjs_short_help(
    CURRENT_LINE *current_line,
    int          for_subjects
  )
{
  unsigned
    int
      ui ;
  SUBJECT
    *subject_ptr ;
  char
    *subj,
    *short_help ;
  if (for_subjects)
  {
    /*
     * Enter for short help on 'subjects'
     */
    send_array_to_screen("\r\n",2) ;
    send_string_to_screen(
      "General structure of command line:\r\n\n"
      "SUBJECT PARAM_ID PARAM_VALUE PARAM_ID PARAM_VALUE PARAM_ID PARAM_VALUE ...",
      TRUE) ;
    send_array_to_screen("\r\n",2) ;
    /*
     * Description of special keys.
     */
    send_string_to_screen(
      "Special keys:\r\n"
      "Back-space - Delete previous character.\r\n"
      "Space      - Parameter separator. Also, complements current parameter.\r\n"
      "Tab        - same as \'Space\'.\r\n"
      "Ctrl-E     - Stop echo. Command line input characters are not echoed.\r\n"
      "Ctrl-F     - Echo mode. Default. Command line input characters are echoed.\r\n"
      "Ctrl-K     - Clear current command line.\r\n",
      TRUE) ;
    send_string_to_screen(
      "Ctrl-T     - Flush type-ahead buffer (stop script and continuous display).\r\n"
      "Escape     - Same as Ctrl-T.\r\n"
      "Ctrl-C     - Temporarily disable standard output (stop long print).\r\n"
      "?          - At the beginning of a clear line: General help;\r\n"
      "             Within a parameter: Detailed help on parameter;\r\n"
      "             After space: Short help on all relevant parameters.\r\n",
      TRUE) ;
    send_string_to_screen(
      "Short help for all subjects:\r\n"
      "============================",
      TRUE) ;
    subject_ptr = &Subjects_list[0] ;
    for (ui = 0 ; ui < Num_subjects ; ui++, subject_ptr++)
    {
      short_help = subject_ptr->subj_short_help ;
      subj = subject_ptr->subj_name ;
      send_string_to_screen(subj,TRUE) ;
      send_array_to_screen("  ",2) ;
      send_string_to_screen(short_help,TRUE) ;
    }
  }
  else
  {
    /*
     * Enter for short help on 'parameters'
     */
    unsigned
      int
        match_index,
        full_match,
        common_len,
        ui ;
    char
      found_names_list[MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT] ;
    char
      msg[80 * 2] ;
    char
      *parameter_name ;
    PARAM
      *parameter_ptr ;
    int
      num_params ;
    unsigned
      long
        current_mutex[NUM_ELEMENTS_MUTEX_CONTROL] ;
    unsigned
      int
        current_ordinal ;
    get_current_mutex_control(current_line,current_mutex) ;
    if (current_line->proc_state == (EXPECTING_PARAM_STRING | EXPECTING_PARAM_VAL))
    {
      /*
       * In this state, 'ordinal' does not change until the next parameter
       * has been selected while, in fact, we want here the 'ordinal' of
       * the parameter not yet entered.
       */
      current_ordinal =
        get_current_ordinal(current_line,current_mutex,1) ;
    }
    else
    {
      current_ordinal =
        get_current_ordinal(current_line,current_mutex,0) ;
    }
    /*
     * Search for ALL parameters fitting current 'mutex' mask on this line
     */
    num_params =
      search_params(
          "",
          0,
          TRUE,
          0,&match_index,&full_match,
          &parameter_name,&common_len,
          (char *)(current_line->line_subject->allowed_params),
          current_line->line_subject->num_allowed_params,
          sizeof(*(current_line->line_subject->allowed_params)),
          OFFSETOF(PARAM,par_name),found_names_list,
          TRUE,
          current_mutex,
          OFFSETOF(PARAM,mutex_control),
          (char **)&current_line->parameters[0],
          current_line->num_param_names,
          current_ordinal,
          OFFSETOF(PARAM,ordinal)
          ) ;
    if (num_params > 0)
    {
      sal_sprintf(
        msg,
          "Short help for all relevant parameters:\r\n"
          "=======================================") ;
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(msg,TRUE) ;
      for (ui = 0 ; ui < (unsigned int)num_params ; ui++)
      {
        search_params(
          "",
          0,
          FALSE,
          ui+1,&match_index,&full_match,
          &parameter_name,&common_len,
          (char *)(current_line->line_subject->allowed_params),
          current_line->line_subject->num_allowed_params,
          sizeof(*(current_line->line_subject->allowed_params)),
          OFFSETOF(PARAM,par_name),found_names_list,
          TRUE,
          current_mutex,
          OFFSETOF(PARAM,mutex_control),
          (char **)&current_line->parameters[0],
          current_line->num_param_names,
          current_ordinal,
          OFFSETOF(PARAM,ordinal)) ;
        parameter_ptr = &current_line->line_subject->allowed_params[match_index] ;
        send_string_to_screen(parameter_ptr->par_name,TRUE) ;
        send_array_to_screen("  ",2) ;
        send_string_to_screen(parameter_ptr->param_short_help,TRUE) ;
      }
    }
    else
    {
      /*
       * No match has been found.
       */
      sal_sprintf(
        msg,
          "*** No relevant parameters were found to display help for.") ;
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(msg,TRUE) ;
    }
  }
  send_array_to_screen("\r\n",2) ;
  return ;
}
/*****************************************************
*NAME
*  print_detailed_help
*TYPE: PROC
*DATE: 23/JAN/2002
*FUNCTION:
*  For subjects:
*  Go through the list of subjects on this system and
*  print detailed help on each subject that fits
*  the part of the subject already typed in.
*  For parameters:
*  Go through the list of parameters for the
*  current subject and print detailed help on
*  each parameter that fits the part of the
*  parameter already typed in.
*CALLING SEQUENCE:
*  print_detailed_help(current_line,for_subjects)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    int for_subjects -
*      Flag. If 'true' print help for subjects.
*      Otherwise, print help for parameters.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Printed short help text.
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  print_detailed_help(
    CURRENT_LINE *current_line,
    int          for_subjects
  )
{
  unsigned
    int
      match_index,
      full_match,
      common_len,
      num_loaded_this_field,
      ui,
      uj ;
  char
    *line,
    *short_help,
    *detailed_help,
    partial_field[MAX_SIZE_OF_SYMBOL] ;
  char
    found_names_list[MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT] ;
  const char
    *proc_name ;
  char
    msg[80 * 2] ;
  proc_name = "print_detailed_help" ;
  num_loaded_this_field = current_line->num_loaded_this_field ;
  line = &current_line->line_buf[
              current_line->start_index_this_field] ;
  memcpy(
    partial_field,
    line,
    num_loaded_this_field) ;
    /*
     * Null termination.
     */
  partial_field[num_loaded_this_field] = 0 ;
  if (for_subjects)
  {
    char
      *subj,
      *subject_name ;
    SUBJECT
      *subject_ptr ;
    PARAM
      *parameter_ptr ;
    int
      num_subjs ;
    unsigned
      int
        num_params ;
    num_subjs =
      search_params(
          line,
          num_loaded_this_field,
          TRUE,
          0,&match_index,&full_match,
          &subject_name,&common_len,
          (char *)&Subjects_list[0],
          get_num_subjs(),
          sizeof(Subjects_list[0]),
          OFFSETOF(SUBJECT,subj_name),found_names_list,
          FALSE,
          0,0,
          (char **)0,0,0,0
          ) ;
    if (num_subjs > 1)
    {
      sal_sprintf(
        msg,
          "Detailed help for all subjects beginning with \'%s\':\r\n"
          "========================================================",
        partial_field) ;
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(msg,TRUE) ;
      for (ui = 0 ; ui < (unsigned int)num_subjs ; ui++)
      {
        search_params(
          line,
          num_loaded_this_field,
          FALSE,
          ui+1,&match_index,&full_match,
          &subject_name,&common_len,
          (char *)&Subjects_list[0],
          get_num_subjs(),
          sizeof(Subjects_list[0]),
          OFFSETOF(SUBJECT,subj_name),
          found_names_list,
          FALSE,
          0,0,
          (char **)0,0,0,0) ;
        subject_ptr = &Subjects_list[match_index] ;
        detailed_help = subject_ptr->subj_detailed_help ;
        short_help = subject_ptr->subj_short_help ;
        subj = subject_ptr->subj_name ;
        send_string_to_screen(subj,TRUE) ;
        send_array_to_screen("  ",2) ;
        send_string_to_screen(short_help,TRUE) ;
        send_string_to_screen(detailed_help,TRUE) ;
        /*
         * Display short help on all parameters.
         */
        parameter_ptr = subject_ptr->allowed_params ;
        num_params = subject_ptr->num_allowed_params ;
        if (num_params)
        {
          send_string_to_screen(
                "Parameters:\r\n"
                "===========",
                TRUE) ;
        }
        else
        {
          send_string_to_screen(
                ">>>There are no parameters for this subject.\r\n",
                TRUE) ;
        }
        for (uj = 0 ; uj < num_params ; uj++, parameter_ptr++)
        {
          if (parameter_ptr->ordinal == 0 ||
              parameter_ptr->ordinal == 1)
          {/*
            *  Just the first and second ordinal should be printed.
            */
            send_string_to_screen(parameter_ptr->par_name,TRUE) ;
            send_array_to_screen("  ",2) ;
            send_string_to_screen(parameter_ptr->param_short_help,TRUE) ;
          }
        }
      }
    }
    else if (num_subjs == 1)
    {
      sal_sprintf(
        msg,
          "One subject found, beginning with \'%s\':\r\n"
          "===========================================",
        partial_field) ;
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(msg,TRUE) ;
      subject_ptr = &Subjects_list[match_index] ;
      detailed_help = subject_ptr->subj_detailed_help ;
      short_help = subject_ptr->subj_short_help ;
      subj = subject_ptr->subj_name ;
      send_string_to_screen(subj,TRUE) ;
      send_array_to_screen("  ",2) ;
      send_string_to_screen(short_help,TRUE) ;
      send_string_to_screen(detailed_help,TRUE) ;
      /*
       * Display short help on all parameters.
       */
      parameter_ptr = subject_ptr->allowed_params ;
      num_params = subject_ptr->num_allowed_params ;
      if (num_params)
      {
        send_string_to_screen(
              "Parameters:\r\n"
              "===========",
              TRUE) ;
      }
      else
      {
        send_string_to_screen(
              ">>>There are no parameters for this subject.\r\n",
              TRUE) ;
      }
      for (uj = 0 ; uj < num_params ; uj++, parameter_ptr++)
      {
        if (parameter_ptr->ordinal == 0 ||
            parameter_ptr->ordinal == 1)
        {/*
          *  Just the first and second ordinal should be printed.
          */
          send_string_to_screen(parameter_ptr->par_name,TRUE) ;
          send_array_to_screen("  ",2) ;
          send_string_to_screen(parameter_ptr->param_short_help,TRUE) ;
        }
      }
    }
    else
    {
      /*
       * No match has been found. Software error.
       */

      sal_sprintf(
        msg,
        "\n\r"
          "Failed to find line \'%s\' - Software error\r\n",
        line) ;
      gen_err(
        FALSE,TRUE,(int)0,ERROR,
        msg,proc_name,
        SVR_ERR,PRINT_DETAILED_HELP_ERR_01,FALSE,0,-1,FALSE) ;
    }
  }
  else
  {
    /*
     * Detailed help on parameters.
     */
    char
      *parameter_name ;
    PARAM
      *parameter_ptr ;
    int
      num_params ;
    unsigned
      long
        current_mutex[NUM_ELEMENTS_MUTEX_CONTROL] ;
    unsigned
      int
        current_ordinal ;
    get_current_mutex_control(current_line,current_mutex) ;
    if (current_line->proc_state == (EXPECTING_PARAM_STRING | EXPECTING_PARAM_VAL))
    {
      /*
       * In this state, 'ordinal' does not change until the next parameter
       * has been selected while, in fact, we want here the 'ordinal' of
       * the parameter not yet entered.
       */
      current_ordinal =
        get_current_ordinal(current_line,current_mutex,1) ;
    }
    else
    {
      current_ordinal =
        get_current_ordinal(current_line,current_mutex,0) ;
    }
    num_params =
      search_params(
          line,
          num_loaded_this_field,
          TRUE,
          0,&match_index,&full_match,
          &parameter_name,&common_len,
          (char *)(current_line->line_subject->allowed_params),
          current_line->line_subject->num_allowed_params,
          sizeof(*(current_line->line_subject->allowed_params)),
          OFFSETOF(PARAM,par_name),found_names_list,TRUE,
          current_mutex,
          OFFSETOF(PARAM,mutex_control),
          (char **)&current_line->parameters[0],
          current_line->num_param_names,
          current_ordinal,
          OFFSETOF(PARAM,ordinal)
          ) ;
    if (num_params > 1)
    {
      sal_sprintf(
        msg,
          "Detailed help for all parameters beginning with \'%s\':\r\n"
          "==========================================================",
        partial_field) ;
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(msg,TRUE) ;
      for (ui = 0 ; ui < (unsigned int)num_params ; ui++)
      {
        search_params(
          line,
          num_loaded_this_field,
          FALSE,
          ui+1,&match_index,&full_match,
          &parameter_name,&common_len,
          (char *)(current_line->line_subject->allowed_params),
          current_line->line_subject->num_allowed_params,
          sizeof(*(current_line->line_subject->allowed_params)),
          OFFSETOF(PARAM,par_name),
          found_names_list,
          TRUE,
          current_mutex,
          OFFSETOF(PARAM,mutex_control),
          (char **)&current_line->parameters[0],
          current_line->num_param_names,
          current_ordinal,
          OFFSETOF(PARAM,ordinal)) ;
        parameter_ptr = &current_line->line_subject->allowed_params[match_index] ;
        send_string_to_screen(parameter_ptr->par_name,TRUE) ;
        send_array_to_screen("  ",2) ;
        send_string_to_screen(parameter_ptr->param_short_help ,TRUE) ;
        send_string_to_screen(parameter_ptr->param_detailed_help,FALSE) ;
        send_array_to_screen("\r\n",2) ;
        send_string_to_screen(
            "Parameter values:\r\n"
            "=================",
            TRUE) ;
        send_string_to_screen(parameter_ptr->vals_detailed_help,TRUE) ;
      }
    }
    else if (num_params == 1)
    {
      sal_sprintf(
        msg,
          "One parameter found, beginning with \'%s\':\r\n"
          "===============================================",
        partial_field) ;
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(msg,TRUE) ;
      parameter_ptr = &current_line->line_subject->allowed_params[match_index] ;
      send_string_to_screen(parameter_ptr->par_name,TRUE) ;
      send_array_to_screen("  ",2) ;
      send_string_to_screen(parameter_ptr->param_short_help,TRUE) ;
      send_string_to_screen(parameter_ptr->param_detailed_help,FALSE) ;
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(
            "Parameter values:\r\n"
            "=================",
            TRUE) ;
      send_string_to_screen(parameter_ptr->vals_detailed_help,TRUE) ;
    }
    else
    {
      /*
       * No match has been found. Software error.
       */
      sal_sprintf(
        msg,
        "\n\r"
          "Failed to find line \'%s\' - Software error\r\n",
        line) ;
      gen_err(
        FALSE,TRUE,(int)0,ERROR,
        msg,proc_name,
        SVR_ERR,PRINT_DETAILED_HELP_ERR_01,FALSE,0,-1,FALSE) ;
    }
  }
  return ;
}
/*****************************************************
*NAME
*  compare_subjs_by_name
*TYPE: PROC
*DATE: 23/JAN/2002
*FUNCTION:
*  Compare names of two subjects. Return an integer
*  less than, equal to, or greater than zero if name
*  of first argument (subject_1) is lexicographically
*  smaller than, equal to, or greater than the name
*  of second argument (subject_2).
*CALLING SEQUENCE:
*  compare_subjs_by_name(subject_1,subject_2)
*INPUT:
*  SOC_SAND_DIRECT:
*    const void *subject_1 -
*      Pointer to structure of type SUBJECT. First
*      subject to compare.
*    const void *subject_2 -
*      Pointer to structure of type SUBJECT. Second
*      subject to compare.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Int. See decsription.
*  SOC_SAND_INDIRECT:
*    None
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  compare_subjs_by_name(
    const void *subject_1,
    const void *subject_2
  )
{
  int
    ret ;
  ret =
    strcmp(
      ((const SUBJECT *)subject_1)->subj_name,
      ((const SUBJECT *)subject_2)->subj_name
    ) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  compare_params_by_name
*TYPE: PROC
*DATE: 24/JAN/2002
*FUNCTION:
*  Compare names of two parameters. Return an integer
*  less than, equal to, or greater than zero if name
*  of first argument (parameter_1) is lexicographically
*  smaller than, equal to, or greater than the name
*  of second argument (parameter_2).
*CALLING SEQUENCE:
*  compare_params_by_name(parameter_1,parameter_2)
*INPUT:
*  SOC_SAND_DIRECT:
*    const void *parameter_1 -
*      Pointer to structure of type PARAM. First
*      parameter to compare.
*    const void *parameter_2 -
*      Pointer to structure of type PARAM. Second
*      parameter to compare.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Int. See description.
*  SOC_SAND_INDIRECT:
*
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  compare_params_by_name(
    const void *parameter_1,
    const void *parameter_2
  )
{
  int
    ret ;
  ret =
    strcmp(
      ((const PARAM *)parameter_1)->par_name,
      ((const PARAM *)parameter_2)->par_name
    ) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  compare_param_vals_by_name
*TYPE: PROC
*DATE: 28/JAN/2002
*FUNCTION:
*  Compare names of two parameter valuess. Return an
*  integer less than, equal to, or greater than zero
*  if name of first argument (parameter_1) is
*  lexicographically smaller than, equal to, or
*  greater than the name of second argument
*  (parameter_2).
*CALLING SEQUENCE:
*  compare_param_vals_by_name(
*            parameter_val_1,parameter_val_2)
*INPUT:
*  SOC_SAND_DIRECT:
*    const void *parameter_val_1 -
*      Pointer to structure of type PARAM_VAL_RULES.
*      First parameter to compare.
*    const void *parameter_val_2 -
*      Pointer to structure of type PARAM_VAL_RULES.
*      Second parameter to compare.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Int. See description.
*  SOC_SAND_INDIRECT:
*
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  compare_param_vals_by_name(
    const void *parameter_val_1,
    const void *parameter_val_2
  )
{
  int
    ret ;
  ret =
    strcmp(
      ((const PARAM_VAL_RULES *)parameter_val_1)->symb_val,
      ((const PARAM_VAL_RULES *)parameter_val_2)->symb_val
    ) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  search_params
*TYPE: PROC
*DATE: 24/JAN/2002
*FUNCTION:
*  Search the indicated list of array elements
*  (which, each, contains a 'name' that is a null
*  terminated string) for a 'name' starting with
*  the spacified input string. Note that case
*  does not count!
*CALLING SEQUENCE:
*  search_params(
*    reference_string,size_reference_string,
*    search_all,num_match,match_index,
*    full_match,parameter_name_ptr,
*    common_len,params_list,
*    element size,name_offest,found_names_list,
*    extra_search_on,reference_mask,
*    mask_offest,excluded_params_base,num_exc_parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *reference_string -
*      Pointer to the string to compare 'names' to.
*    unsigned int  size_reference_string -
*      Number of characters in reference_string.
*    int search_all -
*      Mode of search.
*      If 'TRUE' then
*      all matches are searched for. Direct output
*      is loaded by the number of occurrences and
*      'match_index' (as output) is loaded by the
*      index of the first match (local identifier
*      of the subject).
*      If 'FALSE' then
*      'num_matches' contains, on input, the number of
*      occurence to relate to and 'match_index' (output) is
*      loaded by the index of that match (local identifier
*      of the subject).
*    unsigned int  num_match -
*      Only meaningful for 'search_all == FALSE':
*      Number of match to get the index of (to be
*      used when there is more than one match).
*    unsigned int  *match_index -
*      Depends on 'search_all': Either index of first match
*      found for that string or index of specified match
*      (in 'num_matches') found for that string.
*    unsigned int  *full_match -
*      If a match was found for input string, then this
*      location is loaded as follows:
*      If input string fully matches the parameter then
*      it is loaded 'TRUE'. Otherwise, if match is only
*      partial, then 'FALSE' is loaded.
*    char          **parameter_name_ptr -
*      If a match was found for input string, then this
*      location is loaded by a pointer to the 'name'
*      within array element (null terminated string).
*    unsigned int  *common_len -
*      If a match was found for input string, then this
*      location is loaded by the number of characters
*      which are common to all the 'names' that
*      match the input string.
*    char *params_list -
*      Ordered list (by 'name') to search.
*    unsigned int num_parameters -
*      Number of elements on 'params_list'.
*    unsigned int element_size -
*      Number of bytes in each element on 'params_list'.
*    unsigned int name_offest -
*      Offset, in bytes, from the beginning of
*      each element on 'params_list', where pointer
*      to 'name' of that element resides.
*    char *found_names_list -
*      This is a pointer to an array of characters.
*      This procedure may assume that its size is
*      800 characters. This procedure loads it
*      by the list of names found. Each name ends
*      with cr/lf. The full string is null
*      terminated.
*    unsigned int extra_search_on -
*      Flag. If set 'true' then search only includes
*      parameters with a mask that has at least one
*      bit set at any of the bits on a reference mask.
*      Also, search only includes parameters which
*      are NOT on the provided pointed list of
*      pointers taken from 'params_list'.
*      Also, search only includes parameters whose
*      "ordinal" element is either '0' or LAST_ORDINAL or
*      a. Equal to the 'reference_ordinal' input parameter
*         of this procedure [If BIT(30) and BIT(29) on
*         reference_ordinal are both '0']
*      b. Greater than the 'reference_ordinal' input parameter
*         of this procedure [If BIT(30) on reference_ordinal
*         is '1' and BIT(29) is '0']
*      c. Greater than or equal to the 'reference_ordinal'
*         input parameter of this procedure [If BIT(30)
*         on reference_ordinal is '0' and BIT(29) is '1']
*      Note that in all cases, bits 30 and 29 are not
*      part of the comparison so a 29 bits variable is
*      actually compared.
*      Note: If BIT(30) is set and
*      'reference_ordinal'=LAST_ORDINAL then search is
*      transformed into 'Greater than or equal'
*      (as if BIT(29) were set)
*      If this parameter is 'false' then 'reference_mask',
*      'mask_offest', 'reference_ordinal',
*      'ordinal_offest', num_exc_parameters and
*      'excluded_params_base' are meaningless.
*    unsigned long *reference_mask -
*      Array of unsigned long variables.
*      Mask to be 'and'ed with each parameter's mask.
*      Search only includes parameters with non-zero
*      result. Number of elements in mask is assumed
*      to be fixed at NUM_ELEMENTS_MUTEX_CONTROL!!
*    unsigned int mask_offest -
*      Offset, in bytes, from the beginning of
*      each element on 'params_list', where mask
*      of that element resides. Number of elements
*      in mask is assumed to be fixed at
*      NUM_ELEMENTS_MUTEX_CONTROL!!
*    char **excluded_params_base -
*      Pointer to a list of pointers to elements
*      on 'params list'. These elements should be
*      excluded from search.
*    unsigned int num_exc_parameters -
*      Number of elements on 'excluded_params_base'.
*    unsigned long reference_ordinal -
*      "Ordinal" value to be compared with each
*      parameter's "ordinal". SEE REMARK in
*      'extra_search_on' above!!!
*    unsigned int ordinal_offest -
*      Offset, in bytes, from the beginning of
*      each element on 'params_list', where "ordinal"
*      of that element resides.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Number of matches found. For the "search_all == FALSE"
*      case, this would be '1' if there is occurrence
*      number 'num_match'.
*      If this value is '0' then no match was found
*      and '*match_index' and '*full_match' are meaningless.
*  SOC_SAND_INDIRECT:
*    Updated current_line.
*REMARKS:
*  Search should be made more efficient at later stages.
*SEE ALSO:
 */
int
  search_params(
    const char    *reference_string,
    unsigned int  size_reference_string,
    int           search_all,
    unsigned int  num_match,
    unsigned int  *match_index,
    unsigned int  *full_match,
    char    **parameter_name_ptr,
    unsigned int  *common_len,
    char          *params_list,
    unsigned int  num_parameters,
    unsigned int  element_size,
    unsigned int  name_offest,
    char          *found_names_list,
    unsigned int  extra_search_on,
    unsigned long *reference_mask,
    unsigned int  mask_offest,
    char          **excluded_params_base,
    unsigned int  num_exc_parameters,
    unsigned long reference_ordinal,
    unsigned int  ordinal_offest
  )
{
  int
    found,
    comp,
    ret ;
  char
    name_to_compare[MAX_SIZE_OF_SYMBOL],
    *prev_param_name,
    *param_name ;
  unsigned
    int
      go,
      ui,
      uj,
      uk,
      num_elements,
      c_len,
      param_ordinal,
      ordinal_equal,
      ordinal_greater,
      ordinal_greater_equal ;
  char
    **excluded_params,
    *param_ptr ;
  unsigned
    long
      compare,
      param_mask[NUM_ELEMENTS_MUTEX_CONTROL] ;
  ret = 0 ;
  prev_param_name = NULL;
  memcpy(name_to_compare,reference_string,size_reference_string) ;
  /*
   * Null termination.
   */
  name_to_compare[size_reference_string] = 0 ;
  param_ptr = &params_list[0] ;
  c_len = 0 ;
  found_names_list[0] = 0 ;
  num_elements = sizeof(param_mask) / sizeof(param_mask[0]) ;
  if (extra_search_on)
  {
    ordinal_equal =
      ordinal_greater =
        ordinal_greater_equal = 0 ;
    if (reference_ordinal & (BIT(30) | BIT(29)))
    {
      if (reference_ordinal & BIT(30))
      {
        if (reference_ordinal == LAST_ORDINAL)
        {
          ordinal_greater_equal = 1 ;
        }
        else
        {
          ordinal_greater = 1 ;
        }
      }
      else
      {
        ordinal_greater_equal = 1 ;
      }
    }
    else
    {
      ordinal_equal = 1 ;
    }
    /*
     * Make sure to compare only LS 29 bits (and bit 31, but this
     * is just formal since it is not set throughout this code)
     */
    reference_ordinal &= ~(BIT(30) | BIT(29)) ;
    if (search_all)
    {
      for (ui = 0 ; ui < num_parameters ; ui++, param_ptr += element_size)
      {
        param_name = *((char **)(&param_ptr[name_offest])) ;
        comp = d_strnicmp(param_name,name_to_compare,size_reference_string) ;
        if (comp == 0)
        {
          /*
           * Enter if a match was found for the input reference name.
           */
          memcpy(param_mask,
            (void *)(&param_ptr[mask_offest]),sizeof(param_mask)) ;
          param_ordinal = *((unsigned int *)(&param_ptr[ordinal_offest])) ;
          compare = 0 ;
          for (uk = 0 ; uk < num_elements ; uk++)
          {
            compare |= (reference_mask[uk] & param_mask[uk]) ;
          }
          go = 0 ;
          if (ordinal_equal)
          {
            if ((compare) &&
                ((param_ordinal == 0) ||
                  (param_ordinal == reference_ordinal))
              )
            {
              go = 1 ;
            }
          }
          else if (ordinal_greater)
          {
            if ((compare) &&
                ((param_ordinal == 0) ||
                  (param_ordinal > reference_ordinal))
              )
            {
              go = 1 ;
            }
          }
          else if (ordinal_greater_equal)
          {
            if ((compare) &&
                ((param_ordinal == 0) ||
                  (param_ordinal >= reference_ordinal))
              )
            {
              go = 1 ;
            }
          }
          if (go)
          {
            /*
             * Enter if 'and'ing of masks resulted in a non-zero value
             * and also "ordinal" is either '0' or equal to input
             * reference.
             * Now make sure that this parameter is not on the 'excluded'
             * list.
             */
            found = TRUE ;
            excluded_params = excluded_params_base ;
            for (uj = 0 ; uj < num_exc_parameters ; uj++)
            {
              if (*excluded_params++ == param_ptr)
              {
                /*
                 * Parameter is on the excluded list so do not count it
                 * in.
                 */
                found = FALSE ;
              }
            }
            if (found)
            {
              if (ret == 0)
              {
                /*
                 * Mark the first occurrence.
                 */
                *match_index = ui ;
                *parameter_name_ptr = param_name ;
                comp = strcmp(param_name,name_to_compare) ;
                if (comp == 0)
                {
                  *full_match = TRUE ;
                }
                else
                {
                  *full_match = FALSE ;
                }
                c_len = strlen(param_name) ;
                prev_param_name = param_name ;
              }
              else
              {
                /*
                 * If this is not the first match then get the
                 * largest common beginning characters for all
                 * matching names.
                 */
                c_len = str_max_match(prev_param_name,param_name,c_len) ;
              }
              strcpy(found_names_list,param_name) ;
              found_names_list += strlen(param_name) ;
              *found_names_list++ = '\r' ;
              *found_names_list++ = '\n' ;
              *found_names_list = 0 ;
              ret++ ;
            }
          }
        }
      }
    }
    else
    {
      for (ui = 0 ; ui < num_parameters ; ui++, param_ptr += element_size)
      {
        param_name = *((char **)(&param_ptr[name_offest])) ;
        comp = d_strnicmp(param_name,name_to_compare,size_reference_string) ;
        if (comp == 0)
        {
          /*
           * Enter if a match was found for the input reference name.
           */
          memcpy(param_mask,
            (void *)(&param_ptr[mask_offest]),sizeof(param_mask)) ;
          param_ordinal = *((unsigned int *)(&param_ptr[ordinal_offest])) ;
          compare = 0 ;
          for (uk = 0 ; uk < num_elements ; uk++)
          {
            compare |= (reference_mask[uk] & param_mask[uk]) ;
          }
          go = 0 ;
          if (ordinal_equal)
          {
            if ((compare) &&
                ((param_ordinal == 0) ||
                  (param_ordinal == reference_ordinal))
              )
            {
              go = 1 ;
            }
          }
          else if (ordinal_greater)
          {
            if ((compare) &&
                ((param_ordinal == 0) ||
                  (param_ordinal > reference_ordinal))
              )
            {
              go = 1 ;
            }
          }
          else if (ordinal_greater_equal)
          {
            if ((compare) &&
                ((param_ordinal == 0) ||
                  (param_ordinal >= reference_ordinal))
              )
            {
              go = 1 ;
            }
          }
          if (go)
          {
            /*
             * Enter if 'and'ing of masks resulted in a non-zero value.
             * Now make sure that this parameter is not on the 'excluded'
             * list.
             */
            found = TRUE ;
            excluded_params = excluded_params_base ;
            for (uj = 0 ; uj < num_exc_parameters ; uj++)
            {
              if (*excluded_params++ == param_ptr)
              {
                /*
                 * Parameter is on the excluded list so do not count it
                 * in.
                 */
                found = FALSE ;
              }
            }
            if (found)
            {
              num_match-- ;
              if (num_match == 0)
              {
                /*
                 * Mark the 'count'th occurrence.
                 */
                *match_index = ui ;
                *parameter_name_ptr = param_name ;
                comp = strcmp(param_name,name_to_compare) ;
                if (comp == 0)
                {
                  *full_match = TRUE ;
                }
                else
                {
                  *full_match = FALSE ;
                }
                c_len = strlen(param_name) ;
                prev_param_name = param_name ;
                ret = 1 ;
                strcpy(found_names_list,param_name) ;
                found_names_list += strlen(param_name) ;
                *found_names_list++ = '\r' ;
                *found_names_list++ = '\n' ;
                *found_names_list = 0 ;
                break ;
              }
            }
          }
        }
      }
    }
    *common_len = c_len ;
  }
  else
  {
    if (search_all)
    {
      for (ui = 0 ; ui < num_parameters ; ui++, param_ptr += element_size)
      {
        param_name = *((char **)(&param_ptr[name_offest])) ;
        comp = d_strnicmp(param_name,name_to_compare,size_reference_string) ;
        if (comp == 0)
        {
          if (ret == 0)
          {
            /*
             * Mark the first occurrence.
             */
            *match_index = ui ;
            *parameter_name_ptr = param_name ;
            comp = strcmp(param_name,name_to_compare) ;
            if (comp == 0)
            {
              *full_match = TRUE ;
            }
            else
            {
              *full_match = FALSE ;
            }
            c_len = strlen(param_name) ;
            prev_param_name = param_name ;
          }
          else
          {
            /*
             * If this is not the first match then get the
             * largest common beginning characters for all
             * matching names.
             */
            c_len = str_max_match(prev_param_name,param_name,c_len) ;
          }
          strcpy(found_names_list,param_name) ;
          found_names_list += strlen(param_name) ;
          *found_names_list++ = '\r' ;
          *found_names_list++ = '\n' ;
          *found_names_list = 0 ;
          ret++ ;
        }
      }
    }
    else
    {
      for (ui = 0 ; ui < num_parameters ; ui++, param_ptr += element_size)
      {
        param_name = *((char **)(&param_ptr[name_offest])) ;
        comp = d_strnicmp(param_name,name_to_compare,size_reference_string) ;
        if (comp == 0)
        {
          num_match-- ;
          if (num_match == 0)
          {
            /*
             * Mark the 'count'th occurrence.
             */
            *match_index = ui ;
            *parameter_name_ptr = param_name ;
            comp = strcmp(param_name,name_to_compare) ;
            if (comp == 0)
            {
              *full_match = TRUE ;
            }
            else
            {
              *full_match = FALSE ;
            }
            c_len = strlen(param_name) ;
            prev_param_name = param_name ;
            ret = 1 ;
            strcpy(found_names_list,param_name) ;
            found_names_list += strlen(param_name) ;
            *found_names_list++ = '\r' ;
            *found_names_list++ = '\n' ;
            *found_names_list = 0 ;
            break ;
          }
        }
      }
    }
    *common_len = c_len ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  get_blank_proc_state
*TYPE: PROC
*DATE: 23/JAN/2002
*FUNCTION:
*  Check parameter-value pairs entered on current line
*  and indicate what the next processing state should
*  be. Last character on the line is assumed to be
*  blank space.
*  A line in 'expecting_enter' state remains in that
*  state.
*CALLING SEQUENCE:
*  get_blank_proc_state(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Next processing state for this line.
*      In case of failure - returns -1
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Line is assumed to be of legitimate format (Examples:
*  1. If there are parameters on the line then there must
*     be subject.
*  2. No parameter appears more than once on the line.)
*SEE ALSO:
 */
int
  get_blank_proc_state(
    CURRENT_LINE *current_line
  )
{
  int
    num_params ;
  unsigned int
    match_index,
    full_match,
    common_len,
    current_ordinal ;
  char
    *param_name ;
  unsigned
    long
      current_mutex[NUM_ELEMENTS_MUTEX_CONTROL] ;
  /*
   * We assume here a maximal number of 30 parameters on any next
   * step. In theory, this may not be enough. To do it correctly,
   * we should put here MAX_PARAMS_PER_SUBJECT*MAX_SIZE_OF_SYMBOL
   * plus cr+lf for each, but this is crazy...
   */
  char
    *found_names_list = NULL;
  unsigned
    int
      param_index ;
  int
    ret ;
  SUBJECT
    *subject_ptr ;
  PARAM
    *parameter_ptr ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  PARAM
    *allowed_params ;
  if (current_line->proc_state == EXPECTING_ENTER)
  {
    ret = EXPECTING_ENTER ;
    goto get_next_exit ;
  }
  param_index = current_line->param_index ;
  if (param_index == 0)
  {
    /*
     * Enter if no parameter pairs have been fully
     * entered on this line.
     */
    if (current_line->subject_entered)
    {
      /*
       * Enter if a subject has been entered.
       * Then we should do the following:
       * 1. If no parameter are allowed
       *    then go to EXPECTING_ENTER.
       * 2. If no parameter string
       *    has been entered then go to EXPECTING_PARAM_STRING.
       */
      subject_ptr = current_line->line_subject ;
      if (subject_ptr->num_allowed_params == 0)
      {
        ret = EXPECTING_ENTER ;
      }
      else if (!(current_line->param_string_entered))
      {
        ret = EXPECTING_PARAM_STRING ;
      }
      else
      {
        /*
         * Enter if parameter string has been fully entered.
         * This means that a subject has been enterd, followed
         * by a parameter string.
         */
        match_index = current_line->current_param_match_index ;
        parameter_ptr = &subject_ptr->allowed_params[match_index] ;
        if (parameter_ptr->num_allowed_vals == 0)
        {
          /*
           * If this parameter has no values then
           * go to EXPECTING_PARAM_STRING (unless no more parameters
           * may go on this line, as per 'mutex').
           */
          get_current_mutex_control(current_line,current_mutex) ;
          current_ordinal =
            get_current_ordinal(current_line,current_mutex,0) ;
          allowed_params = current_line->line_subject->allowed_params ;
          found_names_list = sal_alloc(MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT, "get_blank_proc_state.found_names_list");
          if(found_names_list == NULL) {
            send_string_to_screen("Memory allocation failure\r\n",TRUE) ;
            ret = -1;
            goto get_next_exit;
          }
          /*
           * Search for all parameters with ordinal greater or
           * equal to 'current_ordinal'.
           */
          num_params =
            search_params(
              "",0,
              TRUE,
              0,&match_index,&full_match,
              &param_name,&common_len,
              (char *)(allowed_params),
              current_line->line_subject->num_allowed_params,
              sizeof(*allowed_params),
              OFFSETOF(PARAM,par_name),
              found_names_list,
              TRUE,
              current_mutex,
              OFFSETOF(PARAM,mutex_control),
              (char **)&current_line->parameters[0],
              current_line->num_param_names,
              current_ordinal,
              OFFSETOF(PARAM,ordinal)
              ) ;
          if (num_params)
          {
            ret = EXPECTING_PARAM_STRING ;
          }
          else
          {
            ret = EXPECTING_ENTER ;
          }
        }
        else
        {
          /*
           * Parameter does have values. Note that since 'param_index'
           * is '0', no value could have been entered.
           * ('current_line->num_param_values' must be zero)
           * If parameter does not have default
           * then go to EXPECTING_PARAM_VAL
           */
          if ((parameter_ptr->default_care & HAS_DEFAULT_MASK) == HAS_NO_DEFAULT)
          {
            /*
             * Enter if there is no default and some values have
             * been entered. If they are beyond the limit then
             */
            ret = EXPECTING_PARAM_VAL ;
          }
          else
          {
            /*
             * Enter if there is a default value for this parameter.
             * Since 'param_index' is 'zero' then the next field
             * is the first, right after the very first parameter.
             */
#if INCLUDE_ENHANCED_CLI
/* { */
            ret = (EXPECTING_PARAM_VAL | EXPECTING_PARAM_STRING) ;
/* } */
#else
/* { */
            ret = EXPECTING_PARAM_VAL ;
/* } */
#endif
          }
        }
      }
    }
    else
    {
      /*
       * If a subject has not been entered then we should
       * go to EXPECTING_SUBJECT.
       */
      ret = EXPECTING_SUBJECT ;
      goto get_next_exit ;
    }
  }
  else
  {
    /*
     * Enter if some parameters (one or more) have been
     * entered on this line.
     */
    subject_ptr = current_line->line_subject ;
    if (!(current_line->param_string_entered))
    {
      /*
       * If a new (next) parameter string has not yet
       * been entered then expect it, provided this line
       * can take more parameters.
       */
      if ((unsigned int)get_num_params_on_line(current_line) >=
                          subject_ptr->num_allowed_params)
      {
        /*
         * No more parameters can be entered on this line.
         */
        ret = EXPECTING_ENTER ;
      }
      else
      {
        /*
         * If this parameter has no values then
         * go to EXPECTING_PARAM_STRING (unless no more parameters
         * may go on this line, as per 'mutex_control').
         */
        get_current_mutex_control(current_line,current_mutex) ;
        current_ordinal =
          get_current_ordinal(current_line,current_mutex,0) ;
        allowed_params = current_line->line_subject->allowed_params ;
        found_names_list = sal_alloc(MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT, "get_blank_proc_state.found_names_list");
        if(found_names_list == NULL) {
          send_string_to_screen("Memory allocation failure\r\n",TRUE) ;
          ret = -1;
          goto get_next_exit;
        }
        /*
         * Search for all parameters with ordinal greater or
         * equal to 'current_ordinal'.
         */
        num_params =
          search_params(
            "",0,
            TRUE,
            0,&match_index,&full_match,
            &param_name,&common_len,
            (char *)(allowed_params),
            current_line->line_subject->num_allowed_params,
            sizeof(*allowed_params),
            OFFSETOF(PARAM,par_name),
            found_names_list,
            TRUE,
            current_mutex,
            OFFSETOF(PARAM,mutex_control),
            (char **)&current_line->parameters[0],
            current_line->num_param_names,
            current_ordinal,
            OFFSETOF(PARAM,ordinal)
            ) ;
        if (num_params)
        {
          ret = EXPECTING_PARAM_STRING ;
        }
        else
        {
          ret = EXPECTING_ENTER ;
        }
      }
    }
    else
    {
      /*
       * If parameter string has been entered
       * then expect values, provided this parameter can take
       * more values.
       */
      match_index = current_line->current_param_match_index ;
      parameter_ptr = &subject_ptr->allowed_params[match_index] ;
      if ((unsigned int)get_num_params_on_line(current_line) >=
                            subject_ptr->num_allowed_params)
      {
        /*
         * No more parameters can be entered on this line.
         */
        if (parameter_ptr->num_allowed_vals == 0)
        {
          /*
           * If this parameter has no values then
           * go to EXPECTING_ENTER.
           */
          ret = EXPECTING_ENTER ;
        }
        else
        {
          /*
           * Parameter does have values and no more parameters
           * can be entered on this line.
           */
          if (current_line->num_param_values == 0)
          {
            /*
             * Note that since 'num_param_values' is '0',
             * no value has been entered.
             * Since no more parameters can be entered,
             * go to EXPECTING_PARAM_VAL
             */
            ret = EXPECTING_PARAM_VAL ;
          }
          else
          {
            /*
             * Note that since 'num_param_values' is not '0',
             * some values have been entered.
             * The rules are:
             * 1. A symbolic value can appear only once
             * 2. There can be only one symbolic value per one parameter
             * 3. Numeric values may appear more than once depending on
             *    their definition in PARAM_VAL_RULES.
             * If parameter can take no more values
             * then go to EXPECTING_PARAM_VAL
             */
            unsigned
              int
                val_match_index,
                max_num_repeated ;
            PARAM_VAL_RULES
              *param_val_rules ;
            /*
             * Point 'param_val_pair' to the last entry fully loaded.
             * Note that this procedure is called after 'blank' has
             * been keyed in (so 'param_index' has been advanced).
             */
            param_val_pair =
                &current_line->param_val_pair[param_index - 1] ;
            val_match_index = param_val_pair->val_match_index ;
            param_val_rules =
                &parameter_ptr->allowed_vals[val_match_index] ;
            if ((param_val_rules->val_type & VAL_NUMERIC) == VAL_NUMERIC)
            {
              max_num_repeated =
                param_val_rules->val_descriptor.
                        val_num_descriptor.max_num_repeated ;
            }
            else
            {
              max_num_repeated = 1 ;
            }
            if (current_line->num_param_values >= max_num_repeated)
            {
              /*
               * Parameter can take no more values.
               */
              /*
               * No more parameters can be entered on this line.
               */
              ret = EXPECTING_ENTER ;
            }
            else
            {
              /*
               * Parameter can take more values.
               */
              /*
               * No more parameters can be entered on this line.
               */
              ret = EXPECTING_PARAM_VAL ;
            }
          }
        }
      }
      else
      {
        /*
         * Parameter string has been entered and
         * more parameters can be entered on this line.
         */
        if (parameter_ptr->num_allowed_vals == 0)
        {
          /*
           * If this parameter has no values then
           * go to EXPECTING_PARAM_STRING (unless no more parameters
           * may go on this line, as per 'mutex').
           */
          get_current_mutex_control(current_line,current_mutex) ;
          current_ordinal =
            get_current_ordinal(current_line,current_mutex,0) ;
          allowed_params = current_line->line_subject->allowed_params;
          found_names_list = sal_alloc(MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT, "get_blank_proc_state.found_names_list");
          if(found_names_list == NULL) {
            send_string_to_screen("Memory allocation failure\r\n",TRUE) ;
            ret = -1;
            goto get_next_exit;
          }
          /*
           * Search for all parameters with ordinal greater or
           * equal to 'current_ordinal'.
           */
          num_params =
            search_params(
              "",0,
              TRUE,
              0,&match_index,&full_match,
              &param_name,&common_len,
              (char *)(allowed_params),
              current_line->line_subject->num_allowed_params,
              sizeof(*allowed_params),
              OFFSETOF(PARAM,par_name),
              found_names_list,
              TRUE,
              current_mutex,
              OFFSETOF(PARAM,mutex_control),
              (char **)&current_line->parameters[0],
              current_line->num_param_names,
              current_ordinal,
              OFFSETOF(PARAM,ordinal)
              ) ;
          if (num_params)
          {
            ret = EXPECTING_PARAM_STRING ;
          }
          else
          {
            ret = EXPECTING_ENTER ;
          }
        }
        else
        {
          /*
           * Parameter does have values.
           */
          if (current_line->num_param_values == 0)
          {
            /*
             * Note that since 'num_param_values' is '0',
             * no value has been entered.
             * If parameter does not have default
             * then go to EXPECTING_PARAM_VAL
             */
            if ((parameter_ptr->default_care & HAS_DEFAULT_MASK) == HAS_NO_DEFAULT)
            {
              /*
               * Enter if there is no default and some values have
               * been entered. If they are beyond the limit then
               */
              ret = EXPECTING_PARAM_VAL ;
            }
            else
            {
              /*
               * Enter if there is a default value for this parameter.
               */
#if INCLUDE_ENHANCED_CLI
/* { */
              ret = (EXPECTING_PARAM_VAL | EXPECTING_PARAM_STRING) ;
/* } */
#else
/* { */
              ret = EXPECTING_PARAM_VAL ;
/* } */
#endif
            }
          }
          else
          {
            /*
             * Note that since 'num_param_values' is not '0',
             * some values have been entered.
             * The rules are:
             * 1. A symbolic value can appear only once
             * 2. There can be only one symbolic value per one parameter
             * 3. Numeric values may appear more than once depending on
             *    their definition in PARAM_VAL_RULES
             * If parameter can take no more values
             * then go to EXPECTING_PARAM_VAL
             */
            unsigned
              int
                val_match_index_a,
                max_num_repeated_a ;
            PARAM_VAL_RULES
              *param_val_rules ;
            /*
             * Point 'param_val_pair' to the last entry fully loaded.
             * Note that this procedure is called after 'blank' has
             * been keyed in (so 'param_index' has been advanced).
             */
            param_val_pair =
                &current_line->param_val_pair[param_index - 1] ;
            val_match_index_a = param_val_pair->val_match_index ;
            param_val_rules =
                &parameter_ptr->allowed_vals[val_match_index_a] ;
            if ((param_val_rules->val_type & VAL_NUMERIC) == VAL_NUMERIC)
            {
              max_num_repeated_a =
                param_val_rules->val_descriptor.
                        val_num_descriptor.max_num_repeated ;
            }
            else
            {
              max_num_repeated_a = 1 ;
            }
            /*
             * If this parameter has no more values then
             * go to EXPECTING_PARAM_STRING (unless no more parameters
             * may go on this line, as per 'mutex').
             */
            get_current_mutex_control(current_line,current_mutex) ;
            allowed_params = current_line->line_subject->allowed_params ;
            /*
             * Search for all parameters with ordinal greater
             * than 'current_ordinal'.
             * Note that a 'multi variables' parameter keeps beeing
             * 'current_ordinal' until next parameter comes in.
             */
            if (max_num_repeated_a > 1)
            {
              current_ordinal =
                get_current_ordinal(current_line,current_mutex,1) ;
            }
            else
            {
              current_ordinal =
                get_current_ordinal(current_line,current_mutex,0) ;
            }
            found_names_list = sal_alloc(MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT, "get_blank_proc_state.found_names_list");
            if(found_names_list == NULL) {
              send_string_to_screen("Memory allocation failure\r\n",TRUE) ;
              ret = -1;
              goto get_next_exit;
            }
            num_params =
              search_params(
                "",0,
                TRUE,
                0,&match_index,&full_match,
                &param_name,&common_len,
                (char *)(allowed_params),
                current_line->line_subject->num_allowed_params,
                sizeof(*allowed_params),
                OFFSETOF(PARAM,par_name),
                found_names_list,
                TRUE,
                current_mutex,
                OFFSETOF(PARAM,mutex_control),
                (char **)&current_line->parameters[0],
                current_line->num_param_names,
                current_ordinal,
                OFFSETOF(PARAM,ordinal)
                ) ;
            if (current_line->num_param_values >= max_num_repeated_a)
            {
              /*
               * Parameter can take no more values. If more
               * parameters can go on this line then expect them.
               * Otherwise, expect carriage return only.
               */
              if (num_params)
              {
                ret = EXPECTING_PARAM_STRING ;
              }
              else
              {
                ret = EXPECTING_ENTER ;
              }
            }
            else
            {
              /*
               * Parameter can take more values.
               */
              if (num_params)
              {
                ret = (EXPECTING_PARAM_VAL | EXPECTING_PARAM_STRING) ;
              }
              else
              {
                ret = EXPECTING_PARAM_VAL ;
              }
            }
          }
        }
      }
    }
  }
get_next_exit:
  if (found_names_list != NULL){
    sal_free(found_names_list);
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  get_current_mutex_control
*TYPE: PROC
*DATE: 07/MAR/2002
*FUNCTION:
*  Get current effective parameters mask (mutex_control)
*  taking all parameters on current line.
*  To be acceptable of this line, any extra parameter
*  must have at least one bit set on its 'mutex_control'
*  which is also set on that mask.
*CALLING SEQUENCE:
*  get_current_mutex_control(current_line,current_mutex)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE  *current_line -
*      Pointer to current prompt line.
*    unsigned long *current_mutex -
*      Pointer to unsigned long array. This procedure
*      loads pointed array with current effective mask.
*      Caller must make sure array is of, at least,
*      NUM_ELEMENTS_MUTEX_CONTROL elements!!
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error occurred and indirect
*      output is not meaningful.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Line is assumed to be of legitimate format (Examples:
*  1. If there are parameters on the line then there must
*     be subject.
*  2. No parameter appears more than once on the line.)
*SEE ALSO:
 */
int
  get_current_mutex_control(
    CURRENT_LINE *current_line,
    unsigned long *current_mutex
  )
{
  unsigned
    int
      num_elements,
      uj,
      num_param_names,
      ui ;
  unsigned long
    current_mutex_control[NUM_ELEMENTS_MUTEX_CONTROL] ;
  PARAM
    *parameter_ptr ;
  PARAM
    **parameters ;
  int
    ret ;
  ret = 0 ;
  num_param_names = current_line->num_param_names ;
  /*
   * If no parameter name have been fully
   * entered on this line then return an
   * 'all ones' mask.
   */
  memset(current_mutex_control,0xff,sizeof(current_mutex_control)) ;
  num_elements = sizeof(current_mutex_control) / sizeof(current_mutex_control[0]) ;
  if (num_param_names > 0)
  {
    /*
     * Enter if some parameter names (one or more) have been
     * entered on this line.
     */
    parameters = current_line->parameters ;
    for (ui = 0 ; ui < num_param_names ; ui++)
    {
      parameter_ptr = *parameters++ ;
      for (uj = 0 ; uj < num_elements ; uj++)
      {
        current_mutex_control[uj] &= parameter_ptr->mutex_control[uj] ;
      }
    }
  }
  memcpy(current_mutex,current_mutex_control,sizeof(current_mutex_control)) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_current_ordinal
*TYPE: PROC
*DATE: 06/MAY/2002
*FUNCTION:
*  Get smallest "ordinal" element of all parameters
*  which are acceptable (larger "ordinal" than
*  already in use) but NOT already displayed on
*  this line.
*  To be acceptable on this line, any extra parameter
*  must have "ordinal" which is equal to this value
*  (i.e. equal to smallest "ordinal").
*  Note: If last parameter on the line may accept
*  more parameter values than already entered then
*  it remains the 'current ordinal'.
*CALLING SEQUENCE:
*  get_current_ordinal(current_line,current_mutex)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE  *current_line -
*      Pointer to current prompt line.
*    unsigned long *current_mutex -
*      Pointer to array of unsigned long variables.
*      Array is assumed to have NUM_ELEMENTS_MUTEX_CONTROL
*      meaningful elements.
*      Current effective parameters mask
*      (mutex_control)taking into account all
*      parameters on current line.
*      To be acceptable of this line, any extra
*      parameter must have at least one bit set
*      on its 'mutex_control' which is also set
*      on that mask.
*    unsigned int ignore_multi_val -
*      Flag. If set to a non-zero value then calculation
*      of 'current_ordinal' does not take multi
*      value parameters into account (That is,
*      this procedure returns the 'ordinal' of the next
*      parameter irrespective of whether current
*      parameter may take on more values.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int -
*      Value of smallest "ordinal" not yet in use.
*      This should be a non-zero value.
*      If no parameter with non-zero "ordinal"
*      is found (or there is no subject on the
*      line), a value of '0' is returned.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Parameters which have "ordinal" element equal
*  to '0' are assumed to not be included under the
*  "ordinal" rules.
*  A parameter which is already on the line may still
*  be acceptable if it is the last and may accept
*  more parameter values than already entered.
*SEE ALSO:
 */
unsigned
  int
    get_current_ordinal(
      CURRENT_LINE  *current_line,
      unsigned long *current_mutex,
      unsigned int  ignore_multi_val
    )
{
  unsigned
    int
      max_num_repeated,
      ret,
      smallest_ordinal,
      base_ordinal,
      ordinal,
      found,
      ui,
      num_elements,
      uj,
      num_params_on_line,
      param_index,
      num_allowed_params ;
  unsigned
    long
      compare ;
  PARAM
    *last_parameter_ptr,
    *parameter_ptr,
    *allowed_params ;
  PARAM_VAL_RULES
    *allowed_vals ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  ret = 0 ;
  if (!(current_line->subject_entered))
  {
    goto gecu_exit ;
  }
  base_ordinal = 0 ;
  num_elements = NUM_ELEMENTS_MUTEX_CONTROL ;
  num_params_on_line = current_line->num_param_names ;
  /*
   * find the largest "ordinal" on the line.
   */
  for (ui = 0 ; ui < num_params_on_line ; ui++)
  {
    parameter_ptr = current_line->parameters[ui] ;
    if (parameter_ptr->ordinal > base_ordinal)
    {
      base_ordinal = parameter_ptr->ordinal ;
    }
  }
  if (!ignore_multi_val && num_params_on_line)
  {
    /*
     * If last parameter on the line may accept
     * more parameter values than already entered then
     * it remains the 'current ordinal'.
     * This may only happen for numeric parameters.
     */
    last_parameter_ptr = current_line->parameters[num_params_on_line - 1] ;
    if (last_parameter_ptr->ordinal != 0)
    {
      if (last_parameter_ptr->num_numeric_vals)
      {
        /*
         * Enter if there is a numeric entry for this parameter.
         */
        if (current_line->num_param_values)
        {
          /*
           * Enter if there are values for this last parameter.
           */
          if (current_line->param_string_entered)
          {
            /*
             * Enter if last parameter is still holding (i.e. no new parameter
             * has been started and only values of this multi-values numeric
             * parameter have been entered).
             */
            param_index = current_line->param_index - 1 ;
            param_val_pair = &(current_line->param_val_pair[param_index]) ;
            if ((param_val_pair->param_value.val_type & VAL_NUMERIC) == VAL_NUMERIC)
            {
              /*
               * Last entry was a numeric entry.
               * Check whether more numeric values may be entered for
               * this parameter.
               */
              allowed_vals =
                &(last_parameter_ptr->allowed_vals[last_parameter_ptr->numeric_index]) ;
              max_num_repeated =
                (unsigned int)allowed_vals->
                  val_descriptor.val_num_descriptor.max_num_repeated ;
              if (max_num_repeated > current_line->num_param_values)
              {
                /*
                 * If more numeric values may still be entered then last used "ordinal"
                 * is still valid.
                 */
                ret = last_parameter_ptr->ordinal ;
                goto gecu_exit ;
              }
            }
          }
        }
      }
    }
  }
  /*
   * If 'last ordinal' has been encountered then pretend it has not
   * to allow other parameters of the same 'ordinal' to be accepted.
   */
  if (base_ordinal == LAST_ORDINAL)
  {
    base_ordinal = LAST_ORDINAL - 1 ;
  }
  smallest_ordinal = LAST_ORDINAL + 1 ;
  allowed_params = current_line->line_subject->allowed_params ;
  num_allowed_params = current_line->line_subject->num_allowed_params ;
  found = FALSE ;
  for (ui = 0 ; ui < num_allowed_params ; ui++)
  {
    ordinal = allowed_params->ordinal ;
    if (ordinal != 0)
    {
      compare = 0 ;
      for (uj = 0 ; uj < num_elements ; uj++)
      {
        compare |= (current_mutex[uj] & allowed_params->mutex_control[uj]) ;
      }
      if (compare)
      {
        if (!is_params_on_line(allowed_params->par_name,current_line))
        {
          /*
           * Reach here if parameter is not already on the line,
           * it is not excluded by the 'mutex' mechanism and it
           * is in the "ordinal" game.
           */
          if ((smallest_ordinal > ordinal) && (base_ordinal < ordinal))
          {
            smallest_ordinal = ordinal ;
            found = TRUE ;
          }
        }
      }
    }
    allowed_params++ ;
  }
  if (found)
  {
    ret = smallest_ordinal ;
  }
  else
  {
    ret = 0 ;
  }
gecu_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  is_char_legitimate_text
*TYPE: PROC
*DATE: 10/FEB/2002
*FUNCTION:
*  Check whether input character is legitimate for a
*  free text field or not (alphanumeric and '_').
*CALLING SEQUENCE:
*  is_char_legitimate_text(in_char)
*INPUT:
*  SOC_SAND_DIRECT:
*    char in_char -
*      character to test.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then character is legitimate for a
*      free text field.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Free text field can take one of:
*    alphanumeric, '_','.','\', '/', ':', '-','''
*  Note that BLANK_SPACE is also considred a legitimate
*  character for text field (this is for the case
*  where the user wishes to cretae text field with
*  embedded spaces using quotation mark).
*SEE ALSO:
 */
int
  is_char_legitimate_text(
    char in_char
  )
{
  int
    ret ;
  if (isalnum((int)((unsigned int)in_char)) ||
        (in_char == '_')         ||
        (in_char == BLANK_SPACE) ||
        (in_char == '/')  ||
        (in_char == '\\') ||
        (in_char == ':')  ||
        (in_char == '-')  ||
        (in_char == '.')  ||
        (in_char == '\'') ||
        (in_char == ',')
      )
  {
    ret = TRUE ;
  }
  else
  {
    ret = FALSE ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  is_field_legitimate_text
*TYPE: PROC
*DATE: 10/FEB/2002
*FUNCTION:
*  Check whether last field entered on command line
*  is the beginning of a legitimate text field or not.
*  Use to check on-line input field (while being
*  entered).
*CALLING SEQUENCE:
*  is_field_legitimate_text(current_line,err_mag)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    char         **err_mag -
*      Pointer to char pointer. Store in pointed location
*      any error message, in case an erro has been
*      found. Memory is static and does not need to
*      be freed.
*    unsigned long partial -
*      Flag.
*      If 'TRUE' then input free-text string may be
*        partial (i.e. user has not yet filled the
*        whole address.) Check legitimacy so far.
*      If 'FALSE' then input string must be
*        a complete legitimate text field. This
*        means that if it starts with a quotation mark,
*        it must also end with a quotation mark.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then field is the beginning of
*      a legitimate text field.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Text fields start with one of:
*  '"','_','.','\','/',':','-','\'' alphanumeric.
*  It is assumed that at least one character has been
*  entered on the current field.
*  BLANK_SPACE is also acceptable if field has started
*  with QUOTATION_MARK.
*SEE ALSO:
 */
int
  is_field_legitimate_text(
    CURRENT_LINE  *current_line,
    char          **err_msg,
    unsigned long partial
  )
{
  int
    ret ;
  char
    *line_buf ;
  unsigned
    int
      max_num_chars,
      index,
      ui,
      num_loaded_this_field ;
  static
    char
      error_text[80*3] ;
  PARAM
    *parameter_ptr ;
  PARAM_VAL_RULES
    *param_val_rules ;
  const char
    *proc_name ;
  proc_name = "is_field_legitimate_text" ;

  ui = 0;

  num_loaded_this_field =
    current_line->num_loaded_this_field ;
  line_buf =
    &current_line->line_buf[
            current_line->start_index_this_field] ;
  ret = FALSE ;
  parameter_ptr =
    &current_line->line_subject->
      allowed_params[current_line->current_param_match_index] ;
  param_val_rules =
      &parameter_ptr->allowed_vals[parameter_ptr->text_index] ;
  max_num_chars =
      param_val_rules->val_descriptor.val_text_descriptor.max_num_chars ;

  if (max_num_chars > MAX_SIZE_OF_TEXT_VAR)
  {
    /*
     * Global maximum of text size.
     * Probably new text variable mismatch.
     */
    sal_sprintf(error_text, "Probably new text variable mismatch.\n"
                        "SW configed it to %d.\n"
                        "Maximum alload is %d.\n"
                        "Look at define MAX_SIZE_OF_TEXT_VAR." ,
            max_num_chars, MAX_SIZE_OF_TEXT_VAR);
    gen_err(FALSE, FALSE, FALSE, FALSE, error_text, proc_name,
            SVR_WRN, FIELD_LEGITIMATE_TEXT_ERR_01, TRUE, 0, -1, FALSE) ;
    max_num_chars = MAX_SIZE_OF_TEXT_VAR ;
  }
  if (num_loaded_this_field > max_num_chars)
  {
    /*
     * Enter if an error was encountered.
     */
    sal_sprintf(error_text,"Can not accept text: ") ;
    index = strlen(error_text) ;
    memcpy(&error_text[index],line_buf,num_loaded_this_field) ;
    error_text[index + num_loaded_this_field] = 0 ;
    index = strlen(error_text) ;
    sal_sprintf(&error_text[index],"\r\n") ;
    index = strlen(error_text) ;
    sal_sprintf(&error_text[index],
      "Maximal number of characters in this text field is: %u.\r\n",
      (unsigned short)max_num_chars) ;
    *err_msg = error_text ;
  }
  else if (num_loaded_this_field == 0)
  {
    sal_sprintf(error_text,"Line processing stopped at:\r\n") ;
    index = strlen(error_text) ;
    memcpy(&error_text[index],
            current_line->line_buf,
            current_line->start_index_this_field + ui) ;
    error_text[index + current_line->start_index_this_field + ui] = 0 ;
    index = strlen(error_text) ;
    sal_sprintf(&error_text[index],"\r\n") ;
    index = strlen(error_text) ;
    sal_sprintf(&error_text[index],
      "Illegal field length (=0)"
      "\r\n") ;
    *err_msg = error_text ;
    goto isfi_exit ;
  }
  else
  {
    ui = 0 ;
    if (line_buf[ui] == QUOTATION_MARK)
    {
      ui++ ;
      if (!partial)
      {
        if (num_loaded_this_field < 2)
        {
          /*
           * Enter if field is too short.
           */
          sal_sprintf(error_text,"Line processing stopped at:\r\n") ;
          index = strlen(error_text) ;
          memcpy(&error_text[index],
                  current_line->line_buf,
                  current_line->start_index_this_field + ui) ;
          error_text[index + current_line->start_index_this_field + ui] = 0 ;
          index = strlen(error_text) ;
          sal_sprintf(&error_text[index],"\r\n") ;
          index = strlen(error_text) ;
          sal_sprintf(&error_text[index],
            "Illegal field: No matching QUOTATION_MARK"
            "\r\n") ;
          *err_msg = error_text ;
          goto isfi_exit ;
        }
        if (line_buf[num_loaded_this_field - 1] != QUOTATION_MARK)
        {
          /*
           * Enter if field starts with quotation_mark
           * and does not end with it.
           */
          sal_sprintf(error_text,"Line processing stopped at:\r\n") ;
          index = strlen(error_text) ;
          memcpy(&error_text[index],
                  current_line->line_buf,
                  current_line->start_index_this_field + ui) ;
          error_text[index + current_line->start_index_this_field + ui] = 0 ;
          index = strlen(error_text) ;
          sal_sprintf(&error_text[index],"\r\n") ;
          index = strlen(error_text) ;
          sal_sprintf(&error_text[index],
            "Illegal field: No matching QUOTATION_MARK"
            "\r\n") ;
          *err_msg = error_text ;
          goto isfi_exit ;
        }
        num_loaded_this_field-- ;
      }
    }
    ret = TRUE ;
    for ( ; ui < num_loaded_this_field ; ui++)
    {
      if (!is_char_legitimate_text(line_buf[ui]))
      {
        /*
         * Enter if an error was encountered.
         */
        sal_sprintf(error_text,"Line processing stopped at:\r\n") ;
        index = strlen(error_text) ;
        memcpy(&error_text[index],
                current_line->line_buf,
                current_line->start_index_this_field + ui) ;
        error_text[index + current_line->start_index_this_field + ui] = 0 ;
        index = strlen(error_text) ;
        sal_sprintf(&error_text[index],"\r\n") ;
        index = strlen(error_text) ;
        sal_sprintf(&error_text[index],
          "Illegal character. Must be alphanumeric, dot or underline\r\n") ;
        *err_msg = error_text ;
        ret = FALSE ;
        goto isfi_exit ;
      }
    }
  }
isfi_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  is_char_numeric_starter
*TYPE: PROC
*DATE: 29/JAN/2002
*FUNCTION:
*  Check whether input character is a starter of a
*  numeric field or not (digit, '-' or '+').
*CALLING SEQUENCE:
*  is_char_numeric_starter(in_char)
*INPUT:
*  SOC_SAND_DIRECT:
*    char in_char -
*      character to test.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then character is starter of numeric
*      field.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Numeric fileds start with one of: '+', '-', digit.
*SEE ALSO:
 */
int
  is_char_numeric_starter(
    char in_char
  )
{
  int
    ret ;
  if (isdigit((int)((unsigned int)in_char)) ||
        (in_char == '-') ||
        (in_char == '+')
      )
  {
    ret = TRUE ;
  }
  else
  {
    ret = FALSE ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  is_last_field_numeric
*TYPE: PROC
*DATE: 27/JAN/2002
*FUNCTION:
*  Check whether last field entered on command line
*  is numeric or not.
*CALLING SEQUENCE:
*  is_last_field_numeric(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then last field is numeric.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Numeric fileds start with one of: '+', '-', digit.
*  It is assumed that at least one character has been
*  entered on the current field.
*SEE ALSO:
 */
int
  is_last_field_numeric(
    CURRENT_LINE *current_line
  )
{
  int
    ret ;
  char
    first_char ;
  first_char = current_line->line_buf[
                      current_line->start_index_this_field] ;
  if (is_char_numeric_starter(first_char))
  {
    ret = TRUE ;
  }
  else
  {
    ret = FALSE ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  index_of_last_space
*TYPE: PROC
*DATE: 28/JAN/2002
*FUNCTION:
*  Get the inedx of the last blank space entered
*  on the command line. Note that spaces within
*  quotation marks do not count (free text).
*CALLING SEQUENCE:
*  index_of_last_space(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int -
*      Index, in current_line->line_buf, of last space.
*      If there is none then index of one character,
*      beyond the last entered, is returned.
*      If the line already has one quotation mark
*      character on it then space is search only
*      before that character.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
unsigned int
  index_of_last_space(
    CURRENT_LINE *current_line
  )
{
  unsigned int
    quotation_mark_entered,
    line_buf_index,
    ret ;
  int
    ii ;
  char
    *line_buf ;
  line_buf_index = current_line->line_buf_index ;
  ret = line_buf_index ;
  if (line_buf_index > 0)
  {
    quotation_mark_entered = current_line->quotation_mark_entered ;
    line_buf = &current_line->line_buf[line_buf_index - 1] ;
    for (ii = (line_buf_index - 1) ; ii >= 0 ; ii--, line_buf--)
    {
      if (quotation_mark_entered)
      {
        if (*line_buf == QUOTATION_MARK)
        {
          quotation_mark_entered = FALSE ;
        }
      }
      else
      {
        if (*line_buf == QUOTATION_MARK)
        {
          quotation_mark_entered = TRUE ;
        }
        else if (*line_buf == BLANK_SPACE)
        {
          ret = (unsigned int)ii ;
          break ;
        }
      }
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  convert_numeric_field
*TYPE: PROC
*DATE: 28/JAN/2002
*FUNCTION:
*  Convert input numeric field to unsigned long value.
*CALLING SEQUENCE:
*  convert_numeric_field(current_line,
*                  err_mag,value,partial)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    char         **err_msg -
*      Pointer to char pointer. Store in pointed location
*      any error message, in case an erro has been
*      found. Memory is local static memory and does
*      not need to freed.
*    unsigne dlong *value -
*      Pointer to unsigned long. Store in pointed location
*      the converted value. Meaningful only if direct
*      output is zero.
*    unsigned long partial -
*      Flag.
*      If 'TRUE' then input numeric string may be
*        partial (i.e. user has not yet filled the
*        whole field.) Check legitimacy so far.
*      If 'FALSE' then input string must be
*        a complete legitimate numeric field. This
*        means that it should conform to all legitimacy
*        checks.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then field could not be converted
*      because some error has been encountered. Error
*      condition is detailed at memory pointed by
*      err_msg.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Numeric fileds start with one of: '+', '-', digit.
*  If this is a hex number, it should start with 0x
*  or 0X. Hex numbers have, at the most, 8 hex digits.
*  It is assumed that at least one character has been
*  entered on the current field.
*SEE ALSO:
 */
int
  convert_numeric_field(
    CURRENT_LINE  *current_line,
    char          **err_msg,
    unsigned long *value,
    unsigned long partial
  )
{
  int
    ret ;
  char
    *c_ptr,
    *d_ptr,
    in_str[MAX_SIZE_OF_SYMBOL] ;
  static
    char
      error_text[80*4] ;
  unsigned
    int
      hex_field,
      sign_prefix,
      negative_prefix,
      ui,
      num_loaded_this_field ;
  ret = TRUE ;
  negative_prefix = 0;
  num_loaded_this_field = current_line->num_loaded_this_field ;
  if (num_loaded_this_field == 0)
  {
    sal_sprintf(error_text,
            "No characters in input field.") ;
    goto convert_numeric_field_exit ;
  }
  memcpy(in_str,&current_line->line_buf[
            current_line->start_index_this_field],num_loaded_this_field) ;
  in_str[num_loaded_this_field] = 0 ;
  if (strlen(in_str) != num_loaded_this_field)
  {
    sal_sprintf(error_text,
            "End of string (null) character detected within input field.\r\n") ;
    goto convert_numeric_field_exit ;
  }
  hex_field = FALSE ;
  if (num_loaded_this_field > 2)
  {
    if ((memcmp("0x",in_str,2) == 0) || (memcmp("0X",in_str,2) == 0))
    {
      hex_field = TRUE ;
    }
  }
  if (hex_field)
  {
    /*
     * Enter if this is a hex digit.
     */
    if (num_loaded_this_field > 10)
    {
      sal_sprintf(error_text,
            "Field length is %d (%s)\r\n"
            "A maximum of 8 hex digits is allowed in HEX field.\r\n",
            num_loaded_this_field - 2,in_str) ;
      goto convert_numeric_field_exit ;
    }
    else
    {
      /*
       * Field starts with 0X or 0x and has less than 8 digits.
       */
      c_ptr = &in_str[2] ;
      for (ui = 2 ; ui < num_loaded_this_field ; ui++,c_ptr++)
      {
        if (!isxdigit((int)((unsigned int)(*c_ptr))))
        {
          sal_sprintf(error_text,
                "HEX field (%s) contains non hex digits (\'%c\') at offset %u.\r\n",
                in_str,*c_ptr,(unsigned short)ui) ;
          goto convert_numeric_field_exit ;
        }
      }
      /*
       * All digits are pure hex. Now convert to unsigned long
       * and exit. Note that no errors are now expected!
       */
      c_ptr = &in_str[2] ;
      *value = (unsigned long)strtoul(c_ptr,NULL,16) ;
    }
  }
  else
  {
    /*
     * Enter if this is a signed integer.
     */
    unsigned
      int
        num_digits ;
    unsigned
      long
        temp_ul ;
    char
      tmp_str[MAX_SIZE_OF_SYMBOL] ;

    num_digits = 0;
    c_ptr = &in_str[0] ;
    if (*c_ptr == '-')
    {
      negative_prefix = TRUE ;
      sign_prefix = TRUE ;
      c_ptr = &in_str[1] ;
      num_digits = num_loaded_this_field - 1 ;
    }
    else if (*c_ptr == '+')
    {
      sign_prefix = TRUE ;
      negative_prefix = FALSE ;
      c_ptr = &in_str[1] ;
      num_digits = num_loaded_this_field - 1 ;
    }
    else
    {
      sign_prefix = FALSE ;
    }
    if (sign_prefix)
    {
      if (num_digits > 10)
      {
        sal_sprintf(error_text,
              "Numeric field (%s) contains more than 10 digits beyond sign (%u).\r\n",
              in_str,(unsigned short)num_digits) ;
        goto convert_numeric_field_exit ;
      }
      else if (num_digits == 0)
      {
        sal_sprintf(error_text,
              "Numeric field (%s) without digits beyond sign.\r\n",in_str) ;
        goto convert_numeric_field_exit ;
      }
    }
    else if (isdigit((int)((unsigned int)(*c_ptr))))
    {
      if (num_loaded_this_field > 10)
      {
        sal_sprintf(error_text,
              "Numeric field (%s) contains more than 10 digits (%u).\r\n",
              in_str,(unsigned short)num_loaded_this_field) ;
        goto convert_numeric_field_exit ;
      }
      num_digits = num_loaded_this_field ;
    }
    else
    {
      sal_sprintf(error_text,
            "Field (%s) is neither HEX (starting with 0x) nor numeric\r\n"
            "(starting with \'+\', \'-\' or decimal digit. Starts with \'%c\'.\r\n",
            in_str,*c_ptr) ;
      goto convert_numeric_field_exit ;
    }
    /*
     * Reach here if it is a numeric field with or without sign prefix.
     * 'c_ptr' points to the first digit and 'num_digits' contains
     * the number of digits in the filed.
     */
    d_ptr = c_ptr ;
    for (ui = 0 ; ui < num_digits ; ui++,c_ptr++)
    {
      if (!isdigit((int)((unsigned int)(*c_ptr))))
      {
        sal_sprintf(error_text,
              "Numeric field (%s) contains non decimal digits (\'%c\')\r\n"
              "at offset %u.\r\n",
              in_str,*c_ptr,(unsigned short)ui) ;
        goto convert_numeric_field_exit ;
      }
    }
    if (num_digits == 10)
    {
      c_ptr = d_ptr ;
      /*
       * Now check that value is within limits of signed
       * long (-2147483648,+2147483647)
       */
      memcpy(tmp_str,c_ptr,num_digits - 1) ;
      tmp_str[num_digits - 1] = 0 ;
      temp_ul = (unsigned long)strtoul(tmp_str,NULL,10) ;
      if (temp_ul > 214748364)
      {
        sal_sprintf(error_text,
            "Numeric field (%s) beyond \'signed long\' limits"
            " (-2147483648,+2147483647).\r\n",in_str) ;
        goto convert_numeric_field_exit ;
      }
      else if (temp_ul == 214748364)
      {
        if (sign_prefix)
        {
          if (negative_prefix)
          {
            if (c_ptr[9] > '8')
            {
              sal_sprintf(error_text,
                "Negative numeric field (%s) beyond \'signed long\' limits"
                " (-2147483648).\r\n",in_str) ;
              goto convert_numeric_field_exit ;
            }
          }
          else
          {
            if (c_ptr[9] > '7')
            {
              sal_sprintf(error_text,
                "Positive numeric field (%s) beyond \'signed long\' limits"
                " (+2147483647).\r\n",in_str) ;
              goto convert_numeric_field_exit ;
            }
          }
        }
        else
        {
          if (c_ptr[9] > '7')
          {
            sal_sprintf(error_text,
              "Positive numeric field (%s) beyond signed"
              " \'signed long\' (+2147483647).\r\n",in_str) ;
            goto convert_numeric_field_exit ;
          }
        }
      }
    }
    /*
     * Numeric field is fully legitimate. Convert it
     * to unsigned long
     */
    *value = (unsigned long)strtoul(in_str,NULL,0) ;
  }
  /*
   * At this point, *value contains the variable in 'unsigned long'
   * format. Now carry out range checking.
   */
  {
    /*
     * Now make sure numeric value is within limits.
     */
    PARAM
      *parameter_ptr ;
    PARAM_VAL_RULES
      *param_val_rules ;
    VAL_NUM_DESCRIPTOR
      *val_num_descriptor ;
    long
      in_var ;
    in_var = (long)(*value) ;
    parameter_ptr =
      &current_line->line_subject->
        allowed_params[current_line->current_param_match_index] ;
    param_val_rules =
        &parameter_ptr->allowed_vals[parameter_ptr->numeric_index] ;
    val_num_descriptor =
      &param_val_rules->val_descriptor.val_num_descriptor ;
    if (val_num_descriptor->val_numeric_attributes & HAS_MIN_VALUE)
    {
      /*
       * Enter if numeric variable has a minimum value.
       */
      if (in_var < val_num_descriptor->val_min)
      {
        sal_sprintf(error_text,
            "*** Numeric field (%s) is below assigned minimum"
            " (%ld, 0x%08lX).\r\n",
            in_str,
            val_num_descriptor->val_min,
            (unsigned long)val_num_descriptor->val_min) ;
        goto convert_numeric_field_exit ;
      }
    }
    if (val_num_descriptor->val_numeric_attributes & HAS_MAX_VALUE)
    {
      /*
       * Enter if numeric variable has a maximum value.
       */
      if (in_var > val_num_descriptor->val_max)
      {
        sal_sprintf(error_text,
            "*** Numeric field (%s) is above assigned maximum"
            " (%ld, 0x%08lX).\r\n",
            in_str,
            val_num_descriptor->val_max,
            (unsigned long)val_num_descriptor->val_max
            ) ;
        goto convert_numeric_field_exit ;
      }
    }
    if (val_num_descriptor->val_numeric_attributes & POSITIVE_VALUE)
    {
      /*
       * Enter if numeric variable must be positive.
       */
      if (in_var < 0)
      {
        sal_sprintf(error_text,
            "*** Numeric field (%s) must be positive...\r\n",
          in_str
          ) ;
        goto convert_numeric_field_exit ;
      }
    }
    if (val_num_descriptor->val_numeric_attributes & NEGATIVE_VALUE)
    {
      /*
       * Enter if numeric variable must be positive.
       */
      if (in_var >= 0)
      {
        sal_sprintf(error_text,
            "*** Numeric field (%s) must be strictly negative...\r\n",
          in_str
          ) ;
        goto convert_numeric_field_exit ;
      }
    }
    if (val_num_descriptor->val_checker)
    {
      /*
       * Enter if there is a 'value checker' procedure
       * to call.
       */
      int
        err_val ;
      unsigned
        int
          index ;
      err_val =
        val_num_descriptor->
          val_checker((void *)current_line,(unsigned long)in_var,error_text,partial) ;
      if (err_val)
      {
        index = strlen(error_text) ;
        sal_sprintf(&error_text[index],
            "*** Numeric field (%s) not confirmed by validating procedure"
            " (Error code %d).\r\n",in_str,err_val) ;
        goto convert_numeric_field_exit ;
      }
    }
    ret = FALSE ;
    goto convert_numeric_field_exit ;
  }
convert_numeric_field_exit:
  *err_msg = error_text ;
  return (ret) ;
}
/*****************************************************
*NAME
*  is_field_numeric_starter
*TYPE: PROC
*DATE: 29/JAN/2002
*FUNCTION:
*  Check whether last field entered on command line
*  is the beginning of a numeric field or not. Use
*  to check on-line input field (while being
*  entered).
*CALLING SEQUENCE:
*  is_field_numeric_starter(current_line,err_mag)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    char         **err_mag -
*      Pointer to char pointer. Store in pointed location
*      any error message, in case an erro has been
*      found. Memory is static and does not need to
*      be freed.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then field is the beginning of
*      a legitimate numeric field.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Numeric fields start with one of: '+', '-', digit.
*  Hex fields start with '0x' or '0X'.
*  It is assumed that at least one character has been
*  entered on the current field.
*SEE ALSO:
 */
int
  is_field_numeric_starter(
    CURRENT_LINE *current_line,
    char         **err_msg
  )
{
  int
    ret ;
  char
    first_char ;
  char
    *line_buf ;
  static
    char
      error_text[160] ;
  first_char =
    current_line->line_buf[
            current_line->start_index_this_field] ;
  line_buf =
    &current_line->line_buf[
            current_line->start_index_this_field] ;
  if (current_line->num_loaded_this_field == 1)
  {
    /*
     * Enter if only one character has been entered so far.
     */
    if (is_char_numeric_starter(first_char))
    {
      ret = TRUE ;
    }
    else
    {
      sal_sprintf(error_text,
        "Field does not start with a digit, \'-\', or \'+\'.\r\n") ;
      *err_msg = error_text ;
      ret = FALSE ;
    }
  }
  else if (current_line->num_loaded_this_field == 2)
  {
    /*
     * Enter if only two characters have been entered so far.
     */
    if (isdigit((int)((unsigned int)first_char)))
    {
      /*
       * Enter if first character is pure digit.
       */
      if ((memcmp("0x",line_buf,2) == 0) || (memcmp("0X",line_buf,2) == 0))
      {
        /*
         * If the first characters are "0x" or "0X" then it is
         * the legitimate beginning of a HEX field.
         */
        ret = TRUE ;
      }
      else if (isdigit((int)((unsigned int)line_buf[1])))
      {
        /*
         * Enter if it is not the legitimate beginning of
         * a HEX field but the second character is a decimal
         * digit. This is the legitimate beginning of an
         * integer field.
         */
        ret = TRUE ;
      }
      else
      {
        /*
         * Enter if it is not the legitimate beginning of
         * a HEX field and it is not the legitimate
         * beginning of an integer field.
         */
        sal_sprintf(error_text,
          "Field does not start as HEX (\'0x\') or signed integer.\r\n") ;
        *err_msg = error_text ;
        ret = FALSE ;
      }
    }
    else if ((first_char == '-') || (first_char == '+'))
    {
      /*
       * Enter if first character is a sign character.
       */
      if (isdigit((int)((unsigned int)line_buf[1])))
      {
        /*
         * Enter if the second character is a decimal
         * digit. This is the legitimate beginning of an
         * integer field.
         */
        ret = TRUE ;
      }
      else
      {
        /*
         * Enter if first character is a sign character and
         * the second is not a decimal digit. This is not
         * the legitimate beginning of an integer field.
         */
        sal_sprintf(error_text,
          "Non decimal after sign character.\r\n") ;
        *err_msg = error_text ;
        ret = FALSE ;
      }
    }
    else
    {
      /*
       * Enter if first character is not a decimal digit nor
       * a sign character. This is not the legitimate
       * beginning of a numeric field.
       */
      ret = FALSE ;
    }
  }
  else
  {
    /*
     * More than 2 characters have been entered.
     */
    unsigned
      long
        ul_value ;
    if (convert_numeric_field(current_line,err_msg,&ul_value,TRUE))
    {
      /*
       * Enter if an error was encountered while trying to
       * convert this numeric field.
       * The error message is pointed to by *err_msg.
       */
      ret = FALSE ;
    }
    else
    {
      ret = TRUE ;
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  proc_state_to_ascii
*TYPE: PROC
*DATE: 27/JAN/2002
*FUNCTION:
*  Convert line processing state to ascii presentation.
*CALLING SEQUENCE:
*  proc_state_to_ascii(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    char * -
*      Pointer to the null terminated string displaying
*      line's state.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Return parameter must point to static memory!
*SEE ALSO:
 */
const char
  *proc_state_to_ascii(
    CURRENT_LINE *current_line
  )
{
  const char
    *ret ;

  ret = "";
  switch (current_line->proc_state)
  {
    case EXPECTING_PASSWORD:
    {
      ret = "EXPECTING_PASSWORD" ;
      break ;
    }
    case RECEIVING_PASSWORD:
    {
      ret = "RECEIVING_PASSWORD" ;
      break ;
    }
    case EXPECTING_SUBJECT:
    {
      ret = "EXPECTING_SUBJECT" ;
      break ;
    }
    case RECEIVING_SUBJECT:
    {
      ret = "RECEIVING_SUBJECT" ;
      break ;
    }
    case EXPECTING_PARAM_STRING:
    {
      ret = "EXPECTING_PARAM_STRING" ;
      break ;
    }
    case (EXPECTING_PARAM_STRING | EXPECTING_PARAM_VAL):
    {
      ret = "EXPECTING_PARAM_STRING | EXPECTING_PARAM_VAL" ;
      break ;
    }
    case RECEIVING_PARAM_STRING:
    {
      ret = "RECEIVING_PARAM_STRING" ;
      break ;
    }
    case EXPECTING_PARAM_VAL:
    {
      ret = "EXPECTING_PARAM_VAL" ;
      break ;
    }
    case RECEIVING_PARAM_VAL:
    {
      ret = "RECEIVING_PARAM_VAL" ;
      break ;
    }
    case INSERT_MODE:
    {
      ret = "INSERT_MODE" ;
      break ;
    }
    case EXPECTING_ENTER:
    {
      ret = "EXPECTING_ENTER" ;
      break ;
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  param_val_pair_to_ascii
*TYPE: PROC
*DATE: 30/JAN/2002
*FUNCTION:
*  Get ascii presentation of the indicated element
*  in the 'param_val_pair' array on 'current_line'.
*CALLING SEQUENCE:
*  param_val_pair_to_ascii(
            current_line,index,presentation)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    unsigned int index -
*      Index of element to present (in array).
*      If index is beyond array dimension,
*      then an error is indicated on direct
*      output.
*    char **presentation -
*      Pointer to a pointer to the string. This
*      program loads into pointed memory the
*      pointer to the required ascii presentation
*      (null terminated).
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error occurred,
*      there is no meaningful output string.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None. Caller function must 'free' allocated memory!!!!
*SEE ALSO:
 */
int
  param_val_pair_to_ascii(
    CURRENT_LINE *current_line,
    unsigned int index,
    char         **presentation
  )
{
  int
    ret ;
  const char
    *not_loaded,
    *end_ptr ;
  static
    char
      out_str[160] ;
  unsigned
    int
      str_index ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  ret = FALSE ;
  if (index >=
        ((sizeof(current_line->param_val_pair) /
                  sizeof(current_line->param_val_pair[0]))- 1))
  {
    ret = TRUE ;
    goto pvpt_exit ;
  }
  param_val_pair = &current_line->param_val_pair[index] ;
  not_loaded = "[NOT LOADED]" ;
  out_str[0] = 0 ;
  str_index = 0 ;
  sal_sprintf(&out_str[str_index],"%02u) ",(unsigned short)index) ;
  str_index = strlen(out_str) ;
  if (param_val_pair->par_name == (char *)0)
  {
    sal_sprintf(&out_str[str_index],"par_name=%s ",not_loaded) ;
  }
  else
  {
    /*
     * Search for ending null in first 20 characters of string.
     * If there is none then string has not been loaded.
     */
    end_ptr = (char *)memchr(param_val_pair->par_name,0,20) ;
    if (end_ptr == (char *)0)
    {
      sal_sprintf(&out_str[str_index],"par_name=%s ",not_loaded) ;
    }
    else
    {
      if (strlen(param_val_pair->par_name) == 0)
      {
        sal_sprintf(&out_str[str_index],"par_name=[NONE] ") ;
      }
      else
      {
        sal_sprintf(&out_str[str_index],
                  "par_name=\'%s\' ",param_val_pair->par_name) ;
      }
    }
  }
  str_index = strlen(out_str) ;
  sal_sprintf(&out_str[str_index],
                "param_match_index=%u ",
                          (unsigned short)param_val_pair->param_match_index) ;
  str_index = strlen(out_str) ;
  sal_sprintf(&out_str[str_index],
                "\r\n    param_value: ") ;
  str_index = strlen(out_str) ;
  switch (param_val_pair->param_value.val_type & VAL_TYPES_MASK)
  {
    case VAL_NONE:
    {
      sal_sprintf(&out_str[str_index],
                "VAL_NONE ") ;
      break ;
    }
    case VAL_SYMBOL:
    {
      if (param_val_pair->param_value.value.string_value == (char *)0)
      {
        sal_sprintf(&out_str[str_index],"VAL_SYMBOL %s ",not_loaded) ;
      }
      else
      {
        /*
         * Search for ending null in first 20 characters of string.
         * If there is none then string has not been loaded.
         */
        end_ptr =
          (char *)memchr(
              param_val_pair->param_value.value.string_value,0,20) ;
        if (end_ptr == (char *)0)
        {
          sal_sprintf(&out_str[str_index],"VAL_SYMBOL %s ",not_loaded) ;
        }
        else
        {
          if (strlen(param_val_pair->param_value.value.string_value) == 0)
          {
            sal_sprintf(&out_str[str_index],"VAL_SYMBOL [NONE] ") ;
          }
          else
          {
            sal_sprintf(&out_str[str_index],
                  "VAL_SYMBOL \'%s\' ",
                    param_val_pair->param_value.value.string_value) ;
          }
        }
      }
      break ;
    }
    case VAL_TEXT:
    {
      if (strlen(param_val_pair->param_value.value.val_text) == 0)
      {
        sal_sprintf(&out_str[str_index],"VAL_TEXT %s ",not_loaded) ;
      }
      else
      {
        sal_sprintf(&out_str[str_index],
                  "VAL_TEXT \'%s\' ",
                    param_val_pair->param_value.value.val_text) ;
      }
      break ;
    }
    case VAL_IP:
    {
      sal_sprintf(&out_str[str_index],"VAL_IP \'") ;
      str_index = strlen(out_str) ;
      sprint_ip(&out_str[str_index],param_val_pair->param_value.value.ip_value, FALSE) ;
      str_index = strlen(out_str) ;
      sal_sprintf(&out_str[str_index],"\' ") ;
      break ;
    }
    default:
    case VAL_NUMERIC:
    {
      sal_sprintf(&out_str[str_index],
                "VAL_NUMERIC %lu (=0x%08lX=%ld) ",
                  param_val_pair->param_value.value.ulong_value,
                  param_val_pair->param_value.value.ulong_value,
                  (long)param_val_pair->param_value.value.ulong_value) ;
      break ;
    }
  }
  str_index = strlen(out_str) ;
  sal_sprintf(&out_str[str_index],
                "\r\n    ") ;
  str_index = strlen(out_str) ;
  sal_sprintf(&out_str[str_index],
                "val_match_index=%u ",
                          (unsigned short)param_val_pair->val_match_index) ;
  str_index = strlen(out_str) ;
  sal_sprintf(&out_str[str_index],
                "in_val_index=%u ",
                          (unsigned short)param_val_pair->in_val_index) ;
  str_index = strlen(out_str) ;
  *presentation = out_str ;
pvpt_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  is_field_legitimate_ip
*TYPE: PROC
*DATE: 10/FEB/2002
*FUNCTION:
*  Check whether last field entered on command line
*  is the beginning of a legitimate IP address field
*  or not.
*  Use to check on-line input field (while being
*  entered).
*CALLING SEQUENCE:
*  is_field_legitimate_ip(current_line,err_mag)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*    char         **err_mag -
*      Pointer to char pointer. Store in pointed location
*      any error message, in case an erro has been
*      found. Memory is static and does not need to
*      be freed.
*    unsigned long partial -
*      Flag.
*      If 'TRUE' then input string (par1) may be
*        partial (i.e. user has not yet filled the
*        whole address.) Check legitimacy so far.
*      If 'FALSE' then input string (par1) must be
*        a complete legitimate ip address/mask (four
*        hex digits, separated by dots).
*    unsigned long *ip_addr -
*      Only meaningful if
*      'partial' is 'false' and return value is non-zero.
*      Points to memory location to be loaded by
*      the conversion of the input (ASCII
*      representation) to hex (unsigned long)).
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then field is the beginning of
*      a legitimate IP address field.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  IP address format is xxx.xxx.xxx.xxx where 'x'
*  stands for s decimal digit and xxx is smaller than
*  256.
*SEE ALSO:
 */
int
  is_field_legitimate_ip(
    CURRENT_LINE  *current_line,
    char          **err_msg,
    unsigned long partial,
    unsigned long *ip_addr
  )
{
  int
    is_a_digit,
    ret ;
  char
    *line_buf ;
  char
    in_ip_str[MAX_IP_STRING],
    cc,
    *cc_ptr ;
  unsigned
    int
      max_num_chars,
      num_loaded_this_field ;
  unsigned long
    err_index ;
  unsigned long
    orig_len,
    ending_dot,
    ip_item,
    num_ip_chars,
    end_ip_digit,
    len,
    digit_index,
    ip_digit,
    jl ;
  static
    char
      error_text[160] ;
  num_loaded_this_field =
    current_line->num_loaded_this_field ;
  line_buf =
    &current_line->line_buf[
            current_line->start_index_this_field] ;
  ret = FALSE ;
  max_num_chars = 4*3+3 ;
  if (num_loaded_this_field > max_num_chars)
  {
    /*
     * Enter if an error was encountered.
     */
    sal_sprintf(error_text,
      "Maximal number of characters in IP address field is: %u.\r\n",
      (unsigned short)max_num_chars) ;
    *err_msg = error_text ;
  }
  else
  {
    memcpy(in_ip_str,line_buf,num_loaded_this_field) ;
    in_ip_str[num_loaded_this_field] = 0 ;
    if (!(partial))
    {
      *ip_addr = 0 ;
    }
    ret = TRUE ;
    orig_len = len = strlen(in_ip_str) ;
    if (in_ip_str[len - 1] != '.')
    {
      /*
       * ADD ENDING DOT IF THERE IS NOT ONE
       * ALREADY.
       */
      UTILS_STRCAT_SAFE(in_ip_str,".", MAX_IP_STRING) ;
      len++ ;
      ending_dot = FALSE ;
    }
    else
    {
      ending_dot = TRUE ;
    }
    cc_ptr = in_ip_str ;
    ip_item = 0 ;
    for (jl = 0,
          digit_index = 0,
          num_ip_chars = 0 ; jl < len ; jl++)
    {
      cc = *cc_ptr ;
      is_a_digit = isdigit((int)cc) ;
      if (cc == '.')
      {
        end_ip_digit = TRUE ;
      }
      else
      {
        end_ip_digit = FALSE ;
      }
      if (!is_a_digit && !end_ip_digit)
      {
        /*
         * AN ILLEGAL CHARACTER. ONLY DIGITS AND '.' ALLOWED.
         */
        ret = FALSE ;
        err_index = jl ;
        sal_sprintf(error_text,
          "Illegal character at offset %u of IP address field.\r\n",
          (unsigned short)err_index) ;
        *err_msg = error_text ;
        goto ilip_exit ;
      }
      if ((digit_index > 2) && !end_ip_digit)
      {
        /*
         * TOO MANY DIGITS IN ONE BLOCK.
         */
        ret = FALSE ;
        err_index = jl ;
        sal_sprintf(error_text,
          "Too many digits at offset %u of IP address field.\r\n",
          (unsigned short)err_index) ;
        *err_msg = error_text ;
        goto ilip_exit ;
      }
      /*
       * AT THIS POINT, CHARACTER IS EITHER A DIGIT OR
       * A '.'
       */
      if ((digit_index == 2) && is_a_digit)
      {
        char tmp;
        /*
         * CHECK FOR OVERFLOW.
         */
        /* we want to take only 3 characters */
        tmp = in_ip_str[jl - digit_index + 3];
        in_ip_str[jl - digit_index + 3] = 0;
        ip_digit = sal_atoi(&in_ip_str[jl - digit_index]);
        in_ip_str[jl - digit_index + 3] = tmp;
        /*
         * MAXIMAL VALUE ALLOWED IS 255.
         */
        if (ip_digit > 255)
        {
          ret = FALSE ;
          err_index = jl - digit_index ;
          sal_sprintf(error_text,
            "IP byte too large at offset %u of IP address field.\r\n",
            (unsigned short)err_index) ;
          *err_msg = error_text ;
          goto ilip_exit ;
        }
      }
      /*
       * AT THIS POINT, EITHER IT IS A '.' OR INDEX
       * OF CHARACTER WITHIN IP DIGIT IS LESS THAN 2.
       */
      if (end_ip_digit)
      {
        /*
         * ENTER IF END OF CURRENT BLOCK HAS
         * BEEN ENCOUNTERED.
         * WHEN NUMBER OF DIGITS IS LESS THAN TWO,
         * THERE IS NO DANGER OF OVERFLOW.
         * TWO DOTS, ONE FOLLOWING THE OTHER, ARE
         * NOT ALLOWED.
         * AFTER THE FOURTH HEX DIGIT, A DOT IS NOT
         * ALLOWED.
         */
        if (digit_index == 0)
        {
          ret = FALSE ;
          err_index = jl ;
          sal_sprintf(error_text,
            "Two consecutive dots at offset %u of IP address field.\r\n",
            (unsigned short)err_index) ;
          *err_msg = error_text ;
          goto ilip_exit ;
        }
        num_ip_chars++ ;
        if ((num_ip_chars == sizeof(ip_item)) && ending_dot)
        {
          ret = FALSE ;
          err_index = jl ;
          sal_sprintf(error_text,
            "Dot after fourth byte at offset %u of IP address field.\r\n",
            (unsigned short)err_index) ;
          *err_msg = error_text ;
          goto ilip_exit ;
        }
        ip_digit = sal_atoi(&in_ip_str[jl - digit_index]);
        ip_item = ((ip_item << 8) | (ip_digit & 0xFF)) ;
        digit_index = 0 ;
      }
      else
      {
        /*
         * REGULAR DIGIT HANDLING.
         */
        digit_index++ ;
      }
      cc_ptr++ ;
    }
    if (!(partial))
    {
      if (num_ip_chars != sizeof(ip_item))
      {
        ret = FALSE ;
        err_index = orig_len ;
        sal_sprintf(error_text,
            "Less than 4 bytes at offset %u of IP address field.\r\n",
            (unsigned short)err_index) ;
        *err_msg = error_text ;
        goto ilip_exit ;
      }
      else
      {
        *ip_addr = ip_item ;
      }
    }
  }
ilip_exit:
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  space_receiving_param_val
*TYPE: PROC
*DATE: 04/FEB/2002
*FUNCTION:
*  Handle a space key (or 'enter' key) entered on
*  'current_line' while PARMETER VALUE is being
*  received.
*CALLING SEQUENCE:
*  space_receiving_param_val(
*        current_line,enter_case)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*      For the 'enter' case,
*      this is the line to process. Otherwise,
*      this is the cuurently displayed line.
*    int enter_case -
*      Flag. If non-zero then handle the case of
*      'enter' key. Otherwise, handle the 'space'
*      case.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error occurred. For the
*      'enter' case, this means that line can not
*      be processes.
*  SOC_SAND_INDIRECT:
*    Updated *current_line
*    Error messages printed to screen.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  space_receiving_param_val(
    CURRENT_LINE *current_line,
    int          enter_case
  )
{
  int
    ret ;
  int
    num_params ;
  unsigned int
    match_index,
    full_match,
    common_len ;
  const char
    *param_name ;
  char
    *numeirc_param_name ;
  PARAM
    *parameter_ptr ;
  PARAM_VAL_RULES
    *allowed_vals ;
  unsigned int
    num_vals ;
  char
    in_char ;
  char
    *found_names_list = NULL;
  in_char = ' ' ;
  ret = TRUE ;
  allowed_vals = NULL;
  /*
   * Move to next parameter (or to next parameter value).
   */
  parameter_ptr =
      &current_line->line_subject->
          allowed_params[current_line->current_param_match_index] ;
  num_vals = parameter_ptr->num_allowed_vals ;
  {
    /*
     * If field starts with QUOTATION_MARK then make sure
     * it is free text.
     */
    if (current_line->line_buf[
            current_line->start_index_this_field] == QUOTATION_MARK)
    {
      if (parameter_ptr->num_text_vals == 0)
      {
        /*
         * Enter if there are no free text values for current parameter.
         * Getting here, under this condition, can happen only
         * if 'line_mode' is set.
         * However, handle as if it could happen in both modes.
         */
        char
          line_err_msg[320+MAX_SIZE_OF_SYMBOL] ;
        sal_sprintf(line_err_msg,
            "\r\n\n"
            "*** No text values for this parameter (%s). Only text fields\r\n"
            "    may start with a QUOTATION_MARK (\'",
            parameter_ptr->par_name) ;
        send_string_to_screen(line_err_msg,FALSE) ;
        send_array_to_screen(&current_line->line_buf[
                  current_line->start_index_this_field],
                  current_line->num_loaded_this_field) ;
        sal_sprintf(line_err_msg,"\')...\r\n\n") ;
        send_string_to_screen(line_err_msg,TRUE) ;
        if (!enter_case)
        {
          if (!is_line_mode_set())
          {
            redisplay_line(current_line) ;
          }
        }
        goto spar_exit ;
      }
    }
  }
  if (parameter_ptr->num_ip_vals != 0)
  {
    /*
     * Field is IP address.
     */
    char
      *err_msg ;
    PARAM_VAL_PAIR
        *param_val_pair ;
    unsigned
      long
        ip_value ;
    allowed_vals = &(parameter_ptr->allowed_vals[parameter_ptr->ip_index]) ;
    if (!is_field_legitimate_ip(current_line,&err_msg,FALSE,&ip_value))
    {
      /*
       * Enter if an error was encountered while trying to
       * convert this IP address field.
       * The error message is pointed to by err_msg.
       */
      if ((!enter_case) && (!is_line_mode_set()))
      {
        send_array_to_screen("\r\n",2) ;
        msg_and_redisplay(err_msg,current_line) ;
      }
      else
      {
        send_array_to_screen("\r\n\n",3) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else
    {
      /*
       * Enter if IP address is legitimate.
       */
      /*
       * Store IP address into next param-value pair.
       */
      if (!is_line_mode_set())
      {
        /*
         * Actually send to screen only if echo mode is on.
         */
        if (is_echo_mode_on())
        {
          send_char_to_screen(in_char) ;
        }
      }
      current_line->
        line_buf[current_line->line_buf_index] = in_char ;
      current_line->line_buf_index++ ;
      current_line->num_loaded++ ;
      /*
       * Since there can only be one value when IP address is used,
       * 'match_index' must be '0' (first and only element in array).
       */
      match_index = 0 ;
      /*
       * Starting a new field.
       */
      current_line->num_loaded_this_field = 0 ;
      current_line->start_index_this_field =
                      current_line->line_buf_index ;
      param_val_pair =
        &current_line->param_val_pair[
                          current_line->param_index] ;
      param_name = param_val_pair->par_name =
        (current_line->parameters[current_line->num_param_names - 1]->par_name) ;
      param_val_pair->param_match_index =
              current_line->current_param_match_index ;
      param_val_pair->val_match_index = match_index ;
      param_val_pair->param_value.val_type = allowed_vals->val_type ;
      param_val_pair->
              param_value.value.ip_value = ip_value ;
      param_val_pair->in_val_index =
              current_line->num_param_values ;
      current_line->param_string_entered = FALSE ;
      current_line->param_index++ ;
      current_line->num_param_values++ ;
      current_line->proc_state =
                get_blank_proc_state(current_line) ;
      if (current_line->proc_state < 0) {
        ret = TRUE;
        goto spar_exit ;
      }
      if (enter_case)
      {
        /*
         * Following a legitimate IP address value, it is
         * unlikely that parameter value would be expected,
         * but check anyway...
         */
        if (current_line->proc_state == EXPECTING_PARAM_VAL)
        {
          /*
           * If a value is expected then processing can not go on
           * and line must be entered again, properly.
           */
          char
            err_msg[160+MAX_SIZE_OF_SYMBOL] ;
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Cannot process line. More values expected...\r\n\n"
            "List of values for this parameter (\'%s\'):",param_name) ;
          send_string_to_screen(err_msg,TRUE) ;
          send_string_to_screen(
              parameter_ptr->vals_detailed_help,TRUE) ;
        }
        else
        {
          /*
           * Since a parameter is expected (and not a value),
           * try to process line.
           */
          ret = FALSE ;
        }
      }
      else
      {
        ret = FALSE ;
      }
    }
  }
  /*
   * If there are free text values for this field then no
   * other types are expected (i.e. free text does not
   * mix with numeric of symbolic).
   */
  else if (parameter_ptr->num_text_vals != 0)
  {
    char
      *err_msg ;
    /*
     * Field is free text.
     * In non line-mode, it must be legitimate at this stage.
     * However, to unify reduction, test legitimacy.
     */
    allowed_vals = &(parameter_ptr->allowed_vals[parameter_ptr->text_index]) ;
    if (!is_field_legitimate_text(current_line,&err_msg,FALSE))
    {
      /*
       * Enter if an error was encountered while trying to
       * convert this IP address field.
       * The error message is pointed to by err_msg.
       */
      if ((!enter_case) && (!is_line_mode_set()))
      {
        send_array_to_screen("\r\n",2) ;
        msg_and_redisplay(err_msg,current_line) ;
      }
      else
      {
        send_array_to_screen("\r\n\n",3) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else
    {
      char
        *line_buf ;
      unsigned
        int
          num_loaded_this_field ;
      PARAM_VAL_PAIR
          *param_val_pair ;
      /*
       * Store text into next param-value pair.
       */
      num_loaded_this_field =
        current_line->num_loaded_this_field ;
      line_buf =
        &current_line->line_buf[
              current_line->start_index_this_field] ;
      if (current_line->quotation_mark_entered)
      {
        /*
         * Cannot take space when there is an open quotation
         * mark pending.
         */
        char
          err_msg[80*2] ;
        sal_sprintf(err_msg,
          "*** Cannot process line. Must close open quotation first...\r\n\n") ;
        if ((!enter_case) && (!is_line_mode_set()))
        {
          send_array_to_screen("\r\n",2) ;
          msg_and_redisplay(err_msg,current_line) ;
        }
        else
        {
          send_array_to_screen("\r\n\n",3) ;
          send_string_to_screen(err_msg,TRUE) ;
        }
      }
      else
      {
        if (!is_line_mode_set())
        {
          /*
           * Actually send to screen only if echo mode is on.
           */
          if (is_echo_mode_on())
          {
            send_char_to_screen(in_char) ;
          }
        }
        current_line->
          line_buf[current_line->line_buf_index] = in_char ;
        current_line->line_buf_index++ ;
        current_line->num_loaded++ ;
        /*
         * Since there can only be one value when free text is used,
         * 'match_index' must be '0' (first and only element in array).
         */
        match_index = 0 ;
        /*
         * Starting a new field.
         */
        current_line->num_loaded_this_field = 0 ;
        current_line->start_index_this_field =
                        current_line->line_buf_index ;
        param_val_pair =
          &current_line->param_val_pair[
                            current_line->param_index] ;
        param_name = param_val_pair->par_name =
          current_line->parameters[
                  current_line->num_param_names - 1]->par_name ;
        param_val_pair->param_match_index =
                current_line->current_param_match_index ;
        param_val_pair->val_match_index = match_index ;
        param_val_pair->param_value.val_type = allowed_vals->val_type ;
        memcpy(
          param_val_pair->
                param_value.value.val_text,line_buf,
                num_loaded_this_field) ;
        param_val_pair->
          param_value.value.val_text[num_loaded_this_field] = 0 ;
        param_val_pair->in_val_index =
                current_line->num_param_values ;
        current_line->param_string_entered = FALSE ;
        current_line->param_index++ ;
        current_line->num_param_values++ ;
        current_line->proc_state =
                  get_blank_proc_state(current_line) ;
        if (current_line->proc_state < 0) {
          ret = TRUE;
          goto spar_exit ;
        }
        if (enter_case)
        {
          /*
           * Following a legitimate free text value, it is
           * unlikely that parameter value would be expected,
           * but check anyway...
           */
          if (current_line->proc_state == EXPECTING_PARAM_VAL)
          {
            /*
             * If a value is expected then processing can not go on
             * and line must be entered again, properly.
             */
            char
              err_msg[160+MAX_SIZE_OF_SYMBOL] ;
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Cannot process line. More values expected...\r\n\n"
              "List of values for this parameter (\'%s\'):",param_name) ;
            send_string_to_screen(err_msg,TRUE) ;
            send_string_to_screen(
                parameter_ptr->vals_detailed_help,TRUE) ;
          }
          else
          {
            /*
             * Since a parameter is expected (and not a value),
             * try to process line.
             */
            ret = FALSE ;
          }
        }
        else
        {
          ret = FALSE ;
        }
      }
    }
  }
  else if (is_last_field_numeric(current_line))
  {
    /*
     * Field is numeric.
     */
    char
      *err_msg ;
    unsigned
      long
        ul_value ;
    allowed_vals = &(parameter_ptr->allowed_vals[parameter_ptr->numeric_index]) ;
    if (parameter_ptr->num_numeric_vals == 0)
    {
      /*
       * Enter if there are no numeric parameters.
       * Getting here, under this condition, can happen only
       * if 'line_mode' is set.
       * However, handle as if it could happen in both modes.
       */
      char
        line_err_msg[320+MAX_SIZE_OF_SYMBOL] ;
      sal_sprintf(line_err_msg,
          "\r\n\n"
          "*** No numeric values for this parameter (%s), (starting with \'",
          parameter_ptr->par_name) ;
      send_string_to_screen(line_err_msg,FALSE) ;
      send_array_to_screen(&current_line->line_buf[
                current_line->start_index_this_field],
                current_line->num_loaded_this_field) ;
      sal_sprintf(line_err_msg,"\')...\r\n\n") ;
      send_string_to_screen(line_err_msg,TRUE) ;
      if (!enter_case)
      {
        if (!is_line_mode_set())
        {
          redisplay_line(current_line) ;
        }
      }
      goto spar_exit ;
    }
    found_names_list = sal_alloc(MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT, "space_receiving_param_val.found_names_list");
    if(found_names_list == NULL) {
      send_string_to_screen("Memory allocation failure\r\n",TRUE) ;
      ret = TRUE;
      goto spar_exit ;
    }
    /*
     * Make sure to locate the right PARAM_VAL_RULES for
     * this numeric signed integer field.
     * This search has already been carried out when the
     * first character of this field has been entered
     * so we know it must be successful. It is done
     * to get 'match_index'.
     */
    num_params =
      search_params(
          SYMB_NAME_NUMERIC,
          strlen(SYMB_NAME_NUMERIC),
          TRUE,
          0,&match_index,&full_match,
          &numeirc_param_name,&common_len,
          (char *)(parameter_ptr->allowed_vals),
          num_vals,
          sizeof(*(parameter_ptr->allowed_vals)),
          OFFSETOF(PARAM_VAL_RULES,symb_val),
          found_names_list,
          FALSE,
          0,0,
          (char **)0,0,0,0
          ) ;
    if (convert_numeric_field(current_line,&err_msg,&ul_value,FALSE))
    {
      /*
       * Enter if an error was encountered while trying to
       * convert this numeric field.
       * The error message is pointed to by err_msg.
       */
      if ((!enter_case) && (!is_line_mode_set()))
      {
        send_array_to_screen("\r\n",2) ;
        msg_and_redisplay(err_msg,current_line) ;
      }
      else
      {
        send_array_to_screen("\r\n\n",3) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else
    {
     /*
       * Enter if no error was encountered while trying to
       * convert this numeric field.
       * 'ul_value' contains the value entered in unsigned
       * long format.
       */
      PARAM_VAL_PAIR
        *param_val_pair ;
      if (!is_line_mode_set())
      {
        /*
         * Actually send to screen only if echo mode is on.
         */
        if (is_echo_mode_on())
        {
          send_char_to_screen(in_char) ;
        }
      }
      current_line->
        line_buf[current_line->line_buf_index] = in_char ;
      current_line->line_buf_index++ ;
      current_line->num_loaded++ ;
      /*
       * Starting a new field.
       */
      current_line->num_loaded_this_field = 0 ;
      current_line->start_index_this_field =
                      current_line->line_buf_index ;
      param_val_pair =
        &current_line->param_val_pair[
                          current_line->param_index] ;
      param_val_pair->par_name =
        current_line->parameters[
                current_line->num_param_names - 1]->par_name ;
      param_val_pair->param_match_index =
              current_line->current_param_match_index ;
      param_val_pair->val_match_index = match_index ;
      param_val_pair->param_value.val_type = allowed_vals->val_type ;
      param_val_pair->
              param_value.value.ulong_value = ul_value ;
      param_val_pair->in_val_index =
              current_line->num_param_values ;
      current_line->param_index++ ;
      current_line->num_param_values++ ;
      current_line->proc_state =
                get_blank_proc_state(current_line) ;
      if (current_line->proc_state < 0) {
        ret = TRUE;
        goto spar_exit ;
      }
      if (current_line->proc_state == EXPECTING_ENTER)
      {
        current_line->param_string_entered = FALSE ;
      }
      else if (current_line->proc_state == EXPECTING_PARAM_STRING)
      {
        current_line->param_string_entered = FALSE ;
      }
      else if (current_line->proc_state == (EXPECTING_PARAM_STRING | EXPECTING_PARAM_VAL))
      {
        PARAM_VAL_PAIR
          *param_val_pair_x;
        /*
         * Since next entry may contain another value of the same
         * parameter. Prepare it. Mainly for monitoring.
         */
        param_val_pair_x =
          &current_line->param_val_pair[
                            current_line->param_index] ;
        param_val_pair_x->par_name = param_val_pair->par_name ;
        param_val_pair_x->param_match_index = param_val_pair->param_match_index ;
      }
      else if (current_line->proc_state == EXPECTING_PARAM_VAL)
      {
        /*
         * Enter if next expected entry is another param value.
         * This may only happen for multi-valued numerical parameters.
         * Cross check for consistency.
         * Since next entry may contain another value of the same
         * parameter. Prepare it. Mainly for monitoring.
         */
        PARAM_VAL_PAIR
          *param_val_pair_x;
        unsigned
          int
            max_num_repeated,
            num_params_on_line,
            param_index ;
        PARAM
          *last_parameter_ptr ;
        PARAM_VAL_RULES
          *last_allowed_vals ;
        param_index = current_line->param_index - 1 ;
        param_val_pair = &(current_line->param_val_pair[param_index]) ;
        num_params_on_line = current_line->num_param_names ;
        last_parameter_ptr = current_line->parameters[num_params_on_line - 1] ;
        last_allowed_vals =
            &(last_parameter_ptr->allowed_vals[last_parameter_ptr->numeric_index]) ;
        max_num_repeated =
            (unsigned int)last_allowed_vals->
              val_descriptor.val_num_descriptor.max_num_repeated ;
        if (max_num_repeated <= current_line->num_param_values)
        {
          char
            err_msg[240+MAX_SIZE_OF_SYMBOL] ;
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Cannot handle line. System expectes more values while\r\n"
            "    Number of values (%u) larger than/equal to allowed (%u)...\r\n\n"
            "    For this parameter (\'%s\'). Probably SW error",
            current_line->num_param_values,max_num_repeated,numeirc_param_name) ;
          send_string_to_screen(err_msg,TRUE) ;
          goto spar_exit ;
        }
        param_val_pair_x =
          &current_line->param_val_pair[
                            current_line->param_index] ;
        param_val_pair_x->par_name = param_val_pair->par_name ;
        param_val_pair_x->param_match_index = param_val_pair->param_match_index ;
      }
      /*
       * At this poing, we shall try to process the line.
       */
      ret = FALSE ;
      if (enter_case)
      {
        /*
         * Following a legitimate numeric value, make sure
         * the flag of 'new parameter string entered' is reset
         * so as to 'close' the list of 'param_val_pair' (last
         * entry invalid).
         */
        current_line->param_string_entered = FALSE ;
      }
    }
  }
  else
  {
    /*
     * Field is symbolic.
     */
    /*
     * This is the handling of a symbolic value.
     */
    if (parameter_ptr->num_symbolic_vals == 0)
    {
      /*
       * Enter if there are no symbolic values.
       * Getting here, under this condition, can happen only
       * if 'line_mode' is set.
       * However, handle as if it could happen in both modes.
       */
      char
        line_err_msg[320+MAX_SIZE_OF_SYMBOL] ;
      sal_sprintf(line_err_msg,
          "\r\n\n"
          "*** No symbolic values for this parameter (%s), (starting with \'",
          parameter_ptr->par_name) ;
      send_string_to_screen(line_err_msg,FALSE) ;
      send_array_to_screen(&current_line->line_buf[
                current_line->start_index_this_field],
                current_line->num_loaded_this_field) ;
      sal_sprintf(line_err_msg,"\')...\r\n\n") ;
      send_string_to_screen(line_err_msg,TRUE) ;
      if (!enter_case)
      {
        if (!is_line_mode_set())
        {
          redisplay_line(current_line) ;
        }
      }
      goto spar_exit ;
    }
    found_names_list = sal_alloc(MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT, "space_receiving_param_val.found_names_list");
    if(found_names_list == NULL) {
      send_string_to_screen("Memory allocation failure\r\n",TRUE) ;
      ret = TRUE;
      goto spar_exit ;
    }
    /*
     * Check whether there is a symbolic field in the
     * list of values attached to the parameter
     * currently active.
     */
    num_params =
      search_params(
          &current_line->line_buf[
              current_line->start_index_this_field],
          current_line->num_loaded_this_field,
          TRUE,
          0,&match_index,&full_match,
          &numeirc_param_name,&common_len,
          (char *)(parameter_ptr->allowed_vals),
          num_vals,
          sizeof(*(parameter_ptr->allowed_vals)),
          OFFSETOF(PARAM_VAL_RULES,symb_val),
          found_names_list,
          FALSE,
          0,0,
          (char **)0,0,0,0
          ) ;
    if ((num_params > 1) && (!full_match))
    {
      char
        err_msg[240+MAX_SIZE_OF_SYMBOL] ;
      char
        tmp_str[MAX_SIZE_OF_SYMBOL] ;
      complete_last_field(numeirc_param_name,current_line,TRUE,common_len) ;
      memcpy(tmp_str,&current_line->line_buf[
                current_line->start_index_this_field],
                  current_line->num_loaded_this_field) ;
      tmp_str[current_line->num_loaded_this_field] = 0 ;
      if (enter_case)
      {
        sal_sprintf(err_msg,
          "\r\n\n"
          "*** Can not process line: More than one symbolic value\r\n"
          "    starting with \'%s\'...\r\n\n"
          "List of values for this parameter (\'%s\'):",
          tmp_str,parameter_ptr->par_name) ;
        send_string_to_screen(err_msg,TRUE) ;
        help_for_match(
            &current_line->line_buf[current_line->start_index_this_field],
            current_line->num_loaded_this_field,
            parameter_ptr) ;
        send_array_to_screen("\r\n",2) ;
      }
      else
      {
        sal_sprintf(err_msg,
          "\r\n\n"
          "*** Ambiguous. More than one symbolic value starting with \'%s\'.\r\n\n"
          "List of matching values for this parameter (\'%s\'):",
          tmp_str,parameter_ptr->par_name) ;
        send_string_to_screen(err_msg,TRUE) ;
        help_for_match(
            &current_line->line_buf[current_line->start_index_this_field],
            current_line->num_loaded_this_field,
            parameter_ptr) ;
        if (!is_line_mode_set())
        {
          msg_and_redisplay("",current_line) ;
        }
      }
    }
    else if (num_params == 0)
    {
      /*
       * Enter if there are no symbolic values as specified on the line.
       * This can happen only if 'line_mode' is set.
       * However, handle as if it could happen in both modes.
       */
      char
        err_msg[320+MAX_SIZE_OF_SYMBOL] ;
      sal_sprintf(err_msg,
          "\r\n\n"
          "*** No symbolic value starting with \'"
          ) ;
      send_string_to_screen(err_msg,FALSE) ;
      send_array_to_screen(&current_line->line_buf[
                current_line->start_index_this_field],
                current_line->num_loaded_this_field) ;
      sal_sprintf(err_msg,
          "\' has been found...\r\n\n"
          "List of values for this parameter (\'%s\'):",
          parameter_ptr->par_name) ;
      send_string_to_screen(err_msg,TRUE) ;
      send_string_to_screen(
                parameter_ptr->vals_detailed_help,TRUE) ;
      if (!enter_case)
      {
        if (!is_line_mode_set())
        {
          redisplay_line(current_line) ;
        }
      }
    }
    else
    {
      /*
       * Exactly one symbolic value was found.
       */
      int
        err ;
      err = FALSE ;
      if (!full_match)
      {
        if (!is_line_mode_set())
        {
          /*
           * If there is not a full match then complete it
           * and add space.
           */
          err =
            complete_last_field(numeirc_param_name,current_line,FALSE,0) ;
        }
      }
      if (!err)
      {
       /*
         * Enter if an no error was encountered while handling
         * this symbolic field.
         */
        PARAM_VAL_PAIR
          *param_val_pair ;
        VAL_SYMB_DESCRIPTOR
          *val_symb_descriptor ;
        char
          err_msg[160+MAX_SIZE_OF_SYMBOL] ;
        val_symb_descriptor =
          &parameter_ptr->allowed_vals[match_index].
                  val_descriptor.val_symb_descriptor ;
        if (val_symb_descriptor->val_checker)
        {
          /*
           * Enter if there is a 'value checker' procedure
           * to call.
           */
          err =
            val_symb_descriptor->
              val_checker((void *)current_line,
                    (unsigned long)val_symb_descriptor->numeric_equivalent,
                    err_msg,FALSE) ;
          if (err)
          {
            send_string_to_screen(err_msg,TRUE) ;
            if (!enter_case)
            {
              if (!is_line_mode_set())
              {
                redisplay_line(current_line) ;
              }
            }
            goto spar_exit ;
          }
        }
        if (!is_line_mode_set())
        {
          /*
           * Actually send to screen only if echo mode is on.
           */
          if (is_echo_mode_on())
          {
            send_char_to_screen(in_char) ;
          }
        }
        current_line->
          line_buf[current_line->line_buf_index] = in_char ;
        current_line->line_buf_index++ ;
        current_line->num_loaded++ ;
        /*
         * Starting a new field.
         */
        current_line->num_loaded_this_field = 0 ;
        current_line->start_index_this_field =
                        current_line->line_buf_index ;
        param_val_pair =
          &current_line->param_val_pair[
                            current_line->param_index] ;
        param_val_pair->par_name =
          current_line->parameters[
                  current_line->num_param_names - 1]->par_name ;
        param_val_pair->param_match_index =
                current_line->current_param_match_index ;
        param_val_pair->val_match_index = match_index ;
        allowed_vals = &(parameter_ptr->allowed_vals[match_index]) ;
        param_val_pair->param_value.val_type = allowed_vals->val_type ;
        param_val_pair->
                param_value.value.string_value = allowed_vals->symb_val ;
        param_val_pair->
          param_value.numeric_equivalent =
            allowed_vals->
              val_descriptor.val_symb_descriptor.numeric_equivalent ;
        param_val_pair->in_val_index =
                current_line->num_param_values ;
        current_line->param_string_entered = FALSE ;
        current_line->param_index++ ;
        current_line->num_param_values++ ;
        current_line->proc_state =
                  get_blank_proc_state(current_line) ;
        if (current_line->proc_state < 0) {
          ret = TRUE;
          goto spar_exit ;
        }
        if (enter_case)
        {
          /*
           * Following a legitimate symbolic value, it is
           * unlikely that parameter value would be expected,
           * but check anyway...
           */
          if (current_line->proc_state == EXPECTING_PARAM_VAL)
          {
            /*
             * If a value is expected then processing can not go on
             * and line must be entered again, properly.
 */
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Cannot process line. More values expected...\r\n\n"
              "List of values for this parameter (\'%s\'):",numeirc_param_name) ;
            send_string_to_screen(err_msg,TRUE) ;
            send_string_to_screen(
                parameter_ptr->vals_detailed_help,TRUE) ;
          }
          else
          {
            /*
             * Since a parameter is expected (and not a value),
             * try to process line.
             */
            ret = FALSE ;
          }
        }
        else
        {
          /*
           * This is not the case of 'enter' so go on processing.
           */
          ret = FALSE ;
        }
      }
      else
      {
        ret = FALSE ;
      }
    }
  }
spar_exit:
  if (ret == FALSE)
  {
    /*
     * If this parameter is specified as last on line then
     * go to 'EXPECTED_ENTER' state.
     */
    if (allowed_vals->val_type & LAST_VAL_ON_LINE)
    {
       current_line->proc_state = EXPECTING_ENTER ;
    }
  }
  if(found_names_list != NULL) {
    sal_free(found_names_list);
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  space_receiving_param_string
*TYPE: PROC
*DATE: 04/FEB/2002
*FUNCTION:
*  Handle a space key (or 'enter' key) entered on
*  'current_line' while PARMETER STRING is being
*  received.
*CALLING SEQUENCE:
*  space_receiving_param_string(
*        current_line,enter_case,line_to_process)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*      For the 'enter' case,
*      this is the line to process. Otherwise,
*      this is the cuurently displayed line.
*    int enter_case -
*      Flag. If non-zero then handle the case of
*      'enter' key. Otherwise, handle the 'space'
*      case.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error occurred. For the
*      'enter' case, this means that line can not
*      be processes.
*  SOC_SAND_INDIRECT:
*    Updated *current_line
*    Error messages printed to screen.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  space_receiving_param_string(
    CURRENT_LINE *current_line,
    int          enter_case
  )
{
  int
    ret ;
  /*
   * Move to parameter value (or to next parameter).
   */
  int
    num_params ;
  unsigned
    int
      match_index,
      full_match,
      common_len,
      current_ordinal ;
  char
    *param_name ;
  unsigned
    long
      current_mutex[NUM_ELEMENTS_MUTEX_CONTROL] ;
  char
        *found_names_list = NULL ;
  ret = TRUE ;
  get_current_mutex_control(current_line,current_mutex) ;
  current_ordinal =
    get_current_ordinal(current_line,current_mutex,0) ;
  found_names_list = sal_alloc(MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT, "space_receiving_param_string.found_names_list");
  if(found_names_list == NULL) {
    send_string_to_screen("Memory allocation failure\r\n",TRUE) ;
    ret = TRUE;
    goto spre_exit ;
  }
  num_params =
    search_params(
        &current_line->line_buf[
              current_line->start_index_this_field],
        current_line->num_loaded_this_field,
        TRUE,
        0,&match_index,&full_match,
        &param_name,&common_len,
        (char *)(current_line->line_subject->allowed_params),
        current_line->line_subject->num_allowed_params,
        sizeof(*(current_line->line_subject->allowed_params)),
        OFFSETOF(PARAM,par_name),
        found_names_list,
        TRUE,
        current_mutex,
        OFFSETOF(PARAM,mutex_control),
        (char **)&current_line->parameters[0],
        current_line->num_param_names,
        current_ordinal,
        OFFSETOF(PARAM,ordinal)
        ) ;
  if ((num_params > 1) && (!full_match))
  {
    char
      err_msg[320+MAX_SIZE_OF_SYMBOL] ;
    char
      tmp_str[MAX_SIZE_OF_SYMBOL] ;
    complete_last_field(param_name,current_line,TRUE,common_len) ;
    memcpy(tmp_str,&current_line->line_buf[
                current_line->start_index_this_field],
                current_line->num_loaded_this_field) ;
    tmp_str[current_line->num_loaded_this_field] = 0 ;
    sal_sprintf(err_msg,
        "\r\n"
        "List of matching parameters:") ;
    if ((!enter_case) && (!is_line_mode_set()))
    {
      msg_and_redisplay(found_names_list,current_line) ;
    }
    else
    {
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(found_names_list,TRUE) ;
    }
    goto spre_exit ;
  }
  else if (num_params == 0)
  {
    /*
     * Enter if there are no parameters as specified on the line.
     * This can happen only if 'line_mode' is set.
     * However, handle as if it could happen in both modes.
     */
    char
      err_msg[320+MAX_SIZE_OF_SYMBOL] ;
    PARAM
      *parameter_ptr ;
    unsigned
      long
        handled,
        max_num_repeated ;
    /*
     * If this 'parameter' starts with a digit then it may be
     * that the user tries to enter an extra number for the
     * previous parameter string, for which more than one
     * number-value has been entered.
     */
    handled = FALSE ;
    if (is_last_field_numeric(current_line))
    {
      if ((current_line->param_index > 1) && (current_line->num_param_names))
      {
        /*
         * Enter if at least one parameter has already been entered and
         * at least two values have been entered.
         */
        if ((current_line->param_val_pair[
                current_line->param_index - 1].param_value.val_type & VAL_NUMERIC) == VAL_NUMERIC)
        {
          /*
           * Enter if last value has been numeric.
           * Point to corresponding (previous) parameter.
           */
          parameter_ptr = current_line->parameters[current_line->num_param_names - 1] ;
          max_num_repeated =
              parameter_ptr->allowed_vals[parameter_ptr->numeric_index].
                            val_descriptor.val_num_descriptor.max_num_repeated ;
          if (current_line->param_val_pair[
                      current_line->param_index - 1].in_val_index >= (max_num_repeated - 1))
          {
            /*
             * Enter if previous value has been the last allowed on its
             * parameter (multiple numeric values).
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Maximal number of numeric values for \'%s\' is %lu."
                "\r\n",
                parameter_ptr->par_name,max_num_repeated) ;
            send_string_to_screen(err_msg,FALSE) ;
            send_string_to_screen("*** Attempt to interpret \'",FALSE) ;
            send_array_to_screen(&current_line->line_buf[
                      current_line->start_index_this_field],
                      current_line->num_loaded_this_field) ;
            send_string_to_screen(
                "\' as a parameter string has failed...\r\n\n"
                "List of parameters for this subject:",
                TRUE) ;
            send_string_to_screen(current_line->line_subject->params_ascii_list,TRUE) ;
            handled = TRUE ;
          }
        }
      }
    }
    if (!handled)
    {
      sal_sprintf(err_msg,
          "\r\n\n"
          "*** No parameters starting with \'"
          ) ;
      send_string_to_screen(err_msg,FALSE) ;
      send_array_to_screen(&current_line->line_buf[
                current_line->start_index_this_field],
                current_line->num_loaded_this_field) ;
      sal_sprintf(err_msg,
          "\' has been found...\r\n\n"
          "List of parameters for this subject:") ;
      send_string_to_screen(err_msg,TRUE) ;
      send_string_to_screen(current_line->line_subject->params_ascii_list,TRUE) ;
    }
    if (!enter_case)
    {
      if (!is_line_mode_set())
      {
        redisplay_line(current_line) ;
      }
    }
    goto spre_exit ;
  }
  {
    /*
     * Exactly one parameter was found.
     */
    int
      err ;
    char
      err_msg[80*4] ;
    /*
     * We assume that this size of 'err_msg' is sufficient for
     * all the cases below. No further checking!
     */
    err = FALSE ;
    if (!full_match)
    {
      if (!is_line_mode_set())
      {
        /*
         * If there is not a full match then complete it
         * and add space.
         */
        err =
          complete_last_field(param_name,current_line,FALSE,0) ;
        /*
         * Note that this procedure may print an error message
         * to the screen in case an error is encountered.
         */
        if (err)
        {
          goto spre_exit ;
        }
      }
    }
    if (is_params_on_line(param_name,current_line))
    {
      sal_sprintf(err_msg,
            "\r\n"
            "*** This parameter (\'%s\') already appears on this line.\r\n"
            "    Replace or delete...\r\n",
            param_name) ;
      if (enter_case)
      {
        send_array_to_screen("\r\n",2) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        if (!is_line_mode_set())
        {
          msg_and_redisplay(
            err_msg,
            current_line) ;
        }
        else
        {
          send_array_to_screen("\r\n",2) ;
          send_string_to_screen(err_msg,TRUE) ;
        }
      }
      goto spre_exit ;
    }
    /*
     * Check whether parameter may be added to other parameters
     * already on current line.
     * This is redundant since there is only one parameter
     * on this line (but safe...)
     */
    if (is_params_mutex(match_index,current_line,err_msg))
    {
      if (enter_case)
      {
        send_array_to_screen("\r\n",2) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        if (!is_line_mode_set())
        {
          msg_and_redisplay(
            err_msg,
            current_line) ;
        }
        else
        {
          send_array_to_screen("\r\n",2) ;
          send_string_to_screen(err_msg,TRUE) ;
        }
      }
      goto spre_exit ;
    }
    {
      /*
       * Following a parameter string there may be one of the following:
       *   a. Parameter value (if this parameter pair does have a value)
       *   b. Another parameter string (if this parameter pair does
       *      not have a value OR it has a default value).
       */
      PARAM
        *parameter_ptr ;
      PARAM_VAL_PAIR
        *param_val_pair ;
      char
        in_char ;
      in_char = ' ' ;
      parameter_ptr =
        &current_line->line_subject->allowed_params[match_index] ;
      if (parameter_ptr->val_checker)
      {
        /*
         * Enter if there is a 'value checker' procedure
         * to call.
         */
        err =
          parameter_ptr->
            val_checker((void *)current_line,
                  (unsigned long)parameter_ptr->param_id,
                  err_msg,FALSE) ;
        if (err)
        {
          if ((!enter_case) && (!is_line_mode_set()))
          {
            msg_and_redisplay(err_msg,current_line) ;
          }
          else
          {
            send_array_to_screen("\r\n",2) ;
            send_string_to_screen(err_msg,TRUE) ;
          }
          goto spre_exit ;
        }
      }
      current_line->num_param_values = 0 ;
      if (parameter_ptr->num_allowed_vals == 0)
      {
        /*
         * If there are no values for this parameter pair
         * then it must be followed by another parameter string
         * or by carriage return (if no more parameters
         * may be entered on this line)
         */
        current_line->parameters[
          current_line->num_param_names] = parameter_ptr ;
        current_line->num_param_names++ ;
        current_line->param_string_entered = FALSE ;
        param_val_pair =
          &current_line->param_val_pair[current_line->param_index] ;
        param_val_pair->par_name = parameter_ptr->par_name ;
        param_val_pair->param_match_index= match_index ;
        param_val_pair->in_val_index = 0 ;
        param_val_pair->param_value.val_type = VAL_NONE ;
        current_line->param_index++ ;
        /*
         * Redundant but safe.
         */
        param_val_pair =
          &current_line->param_val_pair[current_line->param_index] ;
        param_val_pair->par_name = "" ;
        param_val_pair->in_val_index = 0 ;
        param_val_pair->param_value.val_type = VAL_NONE ;
        /*
         * Now find out the proper next state (see remark at entry
         * to this clause).
         */
        get_current_mutex_control(current_line,current_mutex) ;
        current_ordinal =
          get_current_ordinal(current_line,current_mutex,0) ;
        num_params =
          search_params(
            "",0,
            TRUE,
            0,&match_index,&full_match,
            &param_name,&common_len,
            (char *)(current_line->line_subject->allowed_params),
            current_line->line_subject->num_allowed_params,
            sizeof(*(current_line->line_subject->allowed_params)),
            OFFSETOF(PARAM,par_name),
            found_names_list,
            TRUE,
            current_mutex,
            OFFSETOF(PARAM,mutex_control),
            (char **)&current_line->parameters[0],
            current_line->num_param_names,
            current_ordinal,
            OFFSETOF(PARAM,ordinal)
            ) ;
        if (num_params)
        {
          current_line->
            proc_state = EXPECTING_PARAM_STRING ;
        }
        else
        {
          current_line->
            proc_state = EXPECTING_ENTER ;
        }
      }
      else if ((parameter_ptr->default_care & HAS_DEFAULT_MASK) == HAS_NO_DEFAULT)
      {
        /*
         * If this parameter pair does have values and
         * if there is no default value then value must be keyed
         * in by the user.
         */
          current_line->
            proc_state = EXPECTING_PARAM_VAL ;
        current_line->param_string_entered = TRUE ;
        current_line->parameters[
          current_line->num_param_names] = parameter_ptr ;
        current_line->num_param_names++ ;
        current_line->current_param_match_index = match_index ;
        param_val_pair =
          &current_line->param_val_pair[current_line->param_index] ;
        param_val_pair->par_name = parameter_ptr->par_name ;
        param_val_pair->param_match_index = match_index ;
        param_val_pair->in_val_index = 0 ;
        param_val_pair->param_value.val_type = VAL_NONE ;
      }
      else
      {
#if INCLUDE_ENHANCED_CLI
/* { */
        /*
         * If this parameter pair does have values and
         * there is a default value then either a value
         * can follow or another parameter string.
         * Updating of the other parameters, which are
         * handled above, can not be done here since
         * processing state is still fuzzy. They will be
         * updated when the first character of the next field
         * is entered.
         */
        current_line->
          proc_state = (EXPECTING_PARAM_VAL | EXPECTING_PARAM_STRING) ;
/* } */
#else
/* { */
        /*
         * If this parameter pair does have values a value
         * must follow.
         */
        current_line->
          proc_state = EXPECTING_PARAM_VAL ;
/* } */
#endif
        current_line->param_string_entered = TRUE ;
        current_line->parameters[
          current_line->num_param_names] = parameter_ptr ;
        current_line->num_param_names++ ;
        current_line->current_param_match_index = match_index ;
        param_val_pair =
          &current_line->param_val_pair[current_line->param_index] ;
        param_val_pair->par_name = parameter_ptr->par_name ;
        param_val_pair->param_match_index = match_index ;
        param_val_pair->in_val_index = 0 ;
        param_val_pair->param_value.val_type = VAL_NONE ;
      }
      if (!is_line_mode_set())
      {
        /*
         * Actually send to screen only if echo mode is on.
         */
        if (is_echo_mode_on())
        {
          send_char_to_screen(in_char) ;
        }
      }
      current_line->
        line_buf[current_line->line_buf_index] = in_char ;
      current_line->line_buf_index++ ;
      current_line->num_loaded++ ;
      /*
       * Starting a new field.
       */
      current_line->num_loaded_this_field = 0 ;
      current_line->
        start_index_this_field = current_line->line_buf_index ;
      if (enter_case)
      {
        if (current_line->proc_state == EXPECTING_PARAM_VAL)
        {
          /*
           * If a value is expected then processing can not go on
           * and line must be entered again, properly.
           */
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Cannot process line. This parameter expects values...\r\n\n"
            "List of values for this parameter (\'%s\'):",param_name) ;
          send_string_to_screen(err_msg,TRUE) ;
          send_string_to_screen(
              parameter_ptr->vals_detailed_help,TRUE) ;
        }
        else
        {
          /*
           * Since a parameter is expected (and not a value),
           * try to process line.
           */
          ret = FALSE ;
        }
      }
      else
      {
        ret = FALSE ;
      }
    }
  }
spre_exit:
  if (found_names_list != NULL) {
    sal_free(found_names_list);
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  space_while_receiving_subject
*TYPE: PROC
*DATE: 04/FEB/2002
*FUNCTION:
*  Handle a space key (or 'enter' key) entered on
*  'current_line' while SUBJECT is being received.
*CALLING SEQUENCE:
*  space_while_receiving_subject(
*            current_line,enter_case)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*      For the 'enter' case,
*      this is the line to process. Otherwise
*      this is the current prompt line to display.
*    int enter_case -
*      Flag. If non-zero then handle the case of
*      'enter' key. Otherwise, handle the 'space'
*      case.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error occurred. For the
*      'enter' case, this means that line can not
*      be processes.
*  SOC_SAND_INDIRECT:
*    Updated *current_line
*    Error messages printed to screen.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  space_while_receiving_subject(
    CURRENT_LINE *current_line,
    int          enter_case
  )
{
  int
    ret ;
  /*
   * Move from subject to first parameter.
   */
  int
    num_subjs ;
  unsigned int
    match_index,
    full_match,
    common_len ;
  char
    *subject_name ;
  char
    *found_names_list = NULL ;
  ret = TRUE ;
  found_names_list = sal_alloc(MAX_SIZE_OF_SYMBOL*MAX_PARAMS_PER_SUBJECT, "space_while_receiving_subject.found_names_list");
  if(found_names_list == NULL) {
    send_string_to_screen("Memory allocation failure\r\n",TRUE) ;
    ret = TRUE;
    return ret;
  }
  num_subjs =
    search_params(
        &current_line->line_buf[
              current_line->start_index_this_field],
        current_line->num_loaded_this_field,
        TRUE,
        0,&match_index,&full_match,
        &subject_name,&common_len,
        (char *)&Subjects_list[0],
        get_num_subjs(),
        sizeof(Subjects_list[0]),
        OFFSETOF(SUBJECT,subj_name),
        found_names_list,
        FALSE,
        0,0,(char **)0,0,0,0
        ) ;
  if ((num_subjs > 1) && (!full_match))
  {
    char
      err_msg[320+MAX_SIZE_OF_SYMBOL] ;
    char
      tmp_str[MAX_SIZE_OF_SYMBOL] ;
    complete_last_field(subject_name,current_line,TRUE,common_len) ;
    memcpy(tmp_str,&current_line->line_buf[
              current_line->start_index_this_field],
                current_line->num_loaded_this_field) ;
    tmp_str[current_line->num_loaded_this_field] = 0 ;
    sal_sprintf(err_msg,
      "\r\n\n"
      "*** Ambiguous. Cannot process line. More than one\r\n"
      "    subject starting with \'%s\'...\r\n\n"
      "List of subjects in system:",tmp_str) ;
    send_string_to_screen(err_msg,TRUE) ;
    send_string_to_screen(get_subj_list_ptr(),TRUE) ;
    if ((!enter_case) && (!is_line_mode_set()))
    {
      redisplay_line(current_line) ;
    }
  }
  else if (num_subjs == 0)
  {
    /*
     * Enter if there are no subjects as specified on the line.
     * This can happen only if 'line_mode' is set.
     * However, handle as if it could happen in both modes.
     */
    char
      err_msg[320+MAX_SIZE_OF_SYMBOL] ;
    sal_sprintf(err_msg,
        "\r\n\n"
        "*** No subject starting with \'"
        ) ;
    send_string_to_screen(err_msg,FALSE) ;
    send_array_to_screen(&current_line->line_buf[
              current_line->start_index_this_field],
              current_line->num_loaded_this_field) ;
    sal_sprintf(err_msg,
        "\' has been found...\r\n\n"
        "List of subjects in system:") ;
    send_string_to_screen(err_msg,TRUE) ;
    send_string_to_screen(get_subj_list_ptr(),TRUE) ;
    if (!enter_case)
    {
      if (!is_line_mode_set())
      {
        redisplay_line(current_line) ;
      }
    }
  }
  else
  {
    /*
     * Exactly one subject was found.
     */
    int
      err ;
    SUBJECT
      *subject_ptr ;
    /*
     * If there is not a full match then complete it
     * and add space.
     */
    err = FALSE ;
    if (!full_match)
    {
      if (!is_line_mode_set())
      {
        err =
          complete_last_field(subject_name,current_line,FALSE,0) ;
        /*
         * In case of error, 'complete_last_field' prints
         * error message and redisplays line.
         */
      }
    }
    if (!err)
    {
      char
        in_char ;
      in_char = ' ' ;
      current_line->
        line_buf[current_line->line_buf_index] = in_char ;
      if (!is_line_mode_set())
      {
        /*
         * Actually send to screen only if echo mode is on.
         */
        if (is_echo_mode_on())
        {
          send_char_to_screen(in_char) ;
        }
      }
      subject_ptr = &Subjects_list[match_index] ;
      current_line->subject_entered = TRUE ;
      current_line->line_subject_name = subject_ptr->subj_name ;
      current_line->line_subject = subject_ptr ;
      current_line->line_buf_index++ ;
      current_line->num_loaded++ ;
      /*
       * Starting a new field.
       */
      current_line->num_loaded_this_field = 0 ;
      current_line->
        start_index_this_field = current_line->line_buf_index ;
      if (subject_ptr->num_allowed_params == 0)
      {
        /*
         * If there are no parameters for this subject,
         * only 'enter' is expected.
         */
        ret = FALSE ;
        current_line->proc_state = EXPECTING_ENTER ;
        if (!enter_case)
        {
          if (!is_line_mode_set())
          {
            msg_and_redisplay(
                "\r\n"
                "*** No parameters are expecetd for this subject."
                " Hit 'enter' or change line...\r\n",
                current_line) ;
          }
          else
          {
            char
              err_msg[80*2] ;
            sal_sprintf(err_msg,
                "\r\n"
                "*** No parameters are expecetd for this subject (%s).\r\n"
                "    Change line...",
                current_line->line_subject_name) ;
            send_string_to_screen(err_msg,TRUE) ;
          }
        }
        else
        {
          /*
           * Nothing to do here. Calling procedure
           * May process the line.
           */
        }
      }
      else
      {
        current_line->proc_state = EXPECTING_PARAM_STRING ;
        if (!enter_case)
        {
          ret = FALSE ;
        }
        else
        {
          if (subject_ptr->num_params_without_default)
          {
            /*
             * Enter if this subject has parameters and at least one
             * of them does not have a default value.
             */
            if (!(current_line->line_subject->may_stand_alone))
            {
              /*
               * This subject is not acceptable as stand alone
               * on command line!
               */
              char
                err_msg[320+MAX_SIZE_OF_SYMBOL] ;
              char
                tmp_str[MAX_SIZE_OF_SYMBOL] ;
              memcpy(tmp_str,&current_line->line_buf[
                        current_line->start_index_this_field],
                          current_line->num_loaded_this_field) ;
              tmp_str[current_line->num_loaded_this_field] = 0 ;
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** Cannot process line. This subject expects parameters...\r\n\n"
                "List of parameters for this subject (\'%s\'):",
                current_line->line_subject_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              send_string_to_screen(
                  current_line->line_subject->params_ascii_list,TRUE) ;
            }
            else
            {
              /*
               * This subject is acceptable as stand alone on command line!
               */
              ret = FALSE ;
            }
          }
          else
          {
            /*
             * Enter if this subject has parameters and they all
             * have default values. There is a point in trying to
             * process line.
             */
            ret = FALSE ;
          }
        }
      }
    }
  }
  sal_free(found_names_list);
  return (ret) ;
}

/*****************************************************
*NAME
*  display_param_val_pairs
*TYPE: PROC
*DATE: 30/JAN/2002
*FUNCTION:
*  print all valid entries in 'param_val_pair' array
*  on 'current_line' plus one entry beyond.
*CALLING SEQUENCE:
*  display_param_val_pairs(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error occurred.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  display_param_val_pairs(
    CURRENT_LINE *current_line
  )
{
  int
    ret ;
  unsigned
    int
      ui ;
  int
    err ;
  char
    *cur_line ;
  ret = FALSE ;
  send_array_to_screen("\r\n\n",3) ;
  for (ui = 0 ; ui <= current_line->param_index ; ui++)
  {
    err = param_val_pair_to_ascii(current_line,ui,&cur_line) ;
    if (err)
    {
      ret = TRUE ;
      break ;
    }
    send_string_to_screen(cur_line,TRUE) ;
  }
  return (ret) ;
}

/*****************************************************
*NAME
*  display_line_info
*TYPE: PROC
*DATE: 18/FEB/2002
*FUNCTION:
*  Print/dump info concerning 'current_line' (state and
*  input variables).
*CALLING SEQUENCE:
*  display_line_info(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error occurred.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  display_line_info(
    CURRENT_LINE *current_line
  )
{
  int
    ret ;
  /*
   * Print selected debug information. Debug aid only.
   */
  char
    debug_info[80*6] ;
  unsigned
    int
      info_index ;
  unsigned
    int
      no_parameters ;
  ret = FALSE ;
  display_param_val_pairs(current_line) ;
  debug_info[0] = 0 ;
  /*
   * Index to the ending null character.
   */
  info_index = 0 ;
  sal_sprintf(&debug_info[info_index],"\r\n") ;
  info_index = strlen(debug_info) ;

  UTILS_STRCAT_SAFE(debug_info,proc_state_to_ascii(current_line), 80*6) ;

  info_index = strlen(debug_info) ;
  sal_sprintf(&debug_info[info_index],"\r\n") ;
  info_index = strlen(debug_info) ;
  if (current_line->num_param_names)
  {
    sal_sprintf(
      &debug_info[info_index],"Last valid parameter=\'%s\'.",
        current_line->parameters[
              current_line->num_param_names - 1]->par_name) ;
    no_parameters = FALSE ;
  }
  else
  {
    UTILS_STRCAT_SAFE(debug_info,"No parameters.", 80*6) ;
    no_parameters = TRUE ;
  }
  info_index = strlen(debug_info) ;
  if (current_line->num_param_values)
  {
    sal_sprintf(
      &debug_info[info_index]," Num values=%u.",
        (unsigned short)current_line->num_param_values) ;
  }
  else
  {
    UTILS_STRCAT_SAFE(debug_info," No values.", 80*6) ;
  }
  info_index = strlen(debug_info) ;
  sal_sprintf(
      &debug_info[info_index],"\r\nParam_index=%u.",
        (unsigned short)current_line->param_index) ;
  info_index = strlen(debug_info) ;
  if (!no_parameters)
  {
    if (current_line->param_string_entered)
    {
      sal_sprintf(
        &debug_info[info_index],
          " Parameter string has been fully entered.") ;
    }
    else
    {
      UTILS_STRCAT_SAFE(debug_info," Parameter string started but not completed.", 80*6) ;
    }
    info_index = strlen(debug_info) ;
  }
  sal_sprintf(&debug_info[info_index],"\r\n") ;
  info_index = strlen(debug_info) ;
  send_string_to_screen(debug_info,TRUE) ;
exit:
  return (ret) ;
}


#define USE_SIMPLE_PROMPT 0
/**/
STATIC char*
  get_prompt_string()
{
  return PROMPT_STRING;
}


/*****************************************************
*NAME
*  init_current_line
*TYPE: PROC
*DATE: 31/JAN/2002
*FUNCTION:
*  Initialize 'current_line' structure.
*CALLING SEQUENCE:
*  init_current_line(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to current prompt line structure.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    Initialized *current_line.
*REMARKS:
*  None
*SEE ALSO:
 */
void
  init_current_line(
    CURRENT_LINE *current_line
  )
{
  /*
   * Initialize structures related to current input line.
   */
  current_line->max_chars_on_line = MAX_CHARS_ON_LINE ;
  current_line->max_chars_on_screen_line = MAX_CHARS_ON_SCREEN_LINE ;
  current_line->line_buf_size = sizeof(current_line->line_buf) ;
  current_line->proc_state = EXPECTING_SUBJECT ;
  current_line->prompt_string = get_prompt_string() ;
  current_line->line_buf_index = 0 ;
  current_line->num_loaded = 0 ;
  current_line->start_index_this_field = 0 ;
  current_line->num_loaded_this_field = 0 ;
  current_line->subject_entered = FALSE ;
  current_line->param_index = 0 ;
  current_line->param_string_entered = FALSE ;
  current_line->num_param_values = 0 ;
  current_line->num_param_names = 0 ;
  current_line->scheduled_num_char = 0 ;
  current_line->quotation_mark_entered = 0 ;
  memset(current_line->param_val_pair,0,sizeof(current_line->param_val_pair)) ;
  return ;
}
/*
 * Object: line_mode
 * {
 */
static
  unsigned
    int
      Line_mode_flag ;
void
  clear_line_mode(
    void
  )
{
  Line_mode_flag = 0 ;
  /*
   * Going back to 'single character mode' also
   * clears fifo since 'current line' structures are
   * not properly loaded for 'single character mode'.
   */
  init_history_fifo() ;
}
void
  set_line_mode(
    void
  )
{
  Line_mode_flag = 1 ;
}
unsigned
  int
    is_line_mode_set(
      void
    )
{
  return (Line_mode_flag) ;
}

#if !(defined(LINUX) || defined(UNIX)) && !defined(__DUNE_GTO_BCM_CPU__)
/* { */
int
  line_mode_from_nv(
    unsigned char *line_mode
  )
{
  int
    ret ;
  unsigned
    int
      size,
      offset ;
  STATUS
    status ;
  ret = 0 ;
  offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                ee_block_03.un_ee_block_03.block_03.line_mode)) ;
  size = sizeof(((EE_AREA *)0)->
                ee_block_03.un_ee_block_03.block_03.line_mode) ;
  status = nvRamGet((char *)line_mode,(int)size,(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
  }
  return (ret) ;
}
/*
 * }
 */
/*
 * Object: base / option registers 4 / 5
 * long_vlas is an array of 4 longs (32bit * 4 = 16 bytes)
 * {
 */
int base_and_option_registers_from_nv(unsigned long *long_vals)
{
  int           ret;
  unsigned int  size, offset;
  STATUS        status ;
  ret = 0 ;
  offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                ee_block_02.un_ee_block_02.block_02.base_register_04)) ;
  size = sizeof(((EE_AREA *)0)->
                ee_block_02.un_ee_block_02.block_02.base_register_04) ;
  status = nvRamGet((char *)long_vals,(int)(size*4),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
  }
  return (ret) ;
}

#endif /* } __DUNE_HRP__ */

/*
 * }
 */
/*
 * Object: Input file (for CLI interpreter)
 * This object is related to the special mode where a local
 * file is made (temporarily) the input channel of the main
 * CLI interpreter (handle_next_char())
 * {
 */
static
  unsigned
    int
      File_input_mode = 0 ;
void
  clear_file_input_mode(
    void
  )
{
  File_input_mode = 0 ;
  return ;
}
void
  set_file_input_mode(
    void
  )
{
  File_input_mode = 1 ;
  return ;
}
unsigned
  int
    is_file_input_mode(
      void
    )
{
  return (File_input_mode) ;
}
/*
 * File descriptor of file which is to be used as input to CLI.
 * Only meaningful if File_input_mode is non-zero.
 */
static
  int
    Cli_input_fd = -1 ;
/*
 * Name of CLI file which has been last closed.
 */
static
  char
    Cli_file_name[MAX_FILENAME_LENGTH * 2] ;
void
  set_cli_input_fd(
    int file_descriptor
  )
{
  Cli_input_fd = file_descriptor ;
  return ;
}
int
  get_cli_input_fd(
    void
  )
{
  return (Cli_input_fd) ;
}
void
  close_cli_input_fd(
    void
  )
{
  STATUS
    status ;
  status = ioctl(Cli_input_fd,FIOGETNAME,(long)Cli_file_name) ;
  if (status == ERROR)
  {
    Cli_file_name[0] = 0 ;
  }
#if !DUNE_BCM
  close(Cli_input_fd) ;
#else
  assert(0);
#endif
  Cli_input_fd = -1 ; 
  set_cli_times_available() ;
  return ;
}
/*
 * Time marks taken at beginning and end of execution of
 * downloaded CLI file. In time resolution of 10 ms.
 */
static
  unsigned
    long
      Cli_start_time_10ms = 0 ;
static
  unsigned
    long
      Cli_end_time_10ms = 0 ;
static
  int
    Cli_times_available = 0 ;
void
  set_cli_times_available(
    void
  )
{
  Cli_times_available = TRUE ;
  return ;
}
void
  clear_cli_times_available(
    void
  )
{
  Cli_times_available = FALSE ;
  return ;
}
int
  are_cli_times_available(
    void
  )
{
  return (Cli_times_available) ;
}
void
  set_cli_start_time(
    unsigned long time_from_startup
  )
{
  Cli_start_time_10ms = time_from_startup ;
  return ;
}
void
  set_cli_end_time(
    unsigned long time_from_startup
  )
{
  Cli_end_time_10ms = time_from_startup ;
  return ;
}
/*
 * Get elapsed time during execution of CLI downloaded file
 * in units of 10ms. If no measurement has been taken then
 * direct output is non-zero.
 */
int
  get_cli_elapsed_time(
    unsigned long *elapsed_time,
    char          **file_name
  )
{
  int
    ret ;
  if (Cli_times_available)
  {
    *elapsed_time = (Cli_end_time_10ms - Cli_start_time_10ms) ;
    *file_name = Cli_file_name ;
    ret = 0 ;
  }
  else
  {
    ret = 1 ;
  }
  return (ret) ;
}
/*
 * }
 */
/*
 * Object: page_print
 * {
 */
static
  unsigned
    int
      Num_lines_printed ;
static
  unsigned
    int
      Num_lines_per_page ;
static
  unsigned
    int
      Page_mode_flag ;
static
  unsigned
    int
      Abort_print_flag ;
void
  clear_abort_print_flag(
    void
  )
{
  Abort_print_flag = 0 ;
}
void
  set_abort_print_flag(
    void
  )
{
  Abort_print_flag = 1 ;
}
unsigned
  int
    is_abort_print_flag_set(
      void
    )
{
  return (Abort_print_flag) ;
}
void
  clear_page_mode(
    void
  )
{
  Page_mode_flag = 0 ;
}
void
  set_page_mode(
    void
  )
{
  Page_mode_flag = 1 ;
}
unsigned
  int
    is_page_mode_set(
      void
    )
{
  return (Page_mode_flag) ;
}

#if !(defined(LINUX) || defined(UNIX)) && !defined(__DUNE_GTO_BCM_CPU__)
/* { */
int
  page_lines_from_nv(
    unsigned char *page_lines
  )
{
  int
    ret ;
  unsigned
    int
      size,
      offset ;
  STATUS
    status ;
  ret = 0 ;
  offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                ee_block_03.un_ee_block_03.block_03.page_lines)) ;
  size = sizeof(((EE_AREA *)0)->
                ee_block_03.un_ee_block_03.block_03.page_lines) ;
  status = nvRamGet((char *)page_lines,(int)size,(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
  }
  return (ret) ;
}
int
  page_mode_from_nv(
    unsigned char *page_mode
  )
{
  int
    ret ;
  unsigned
    int
      size,
      offset ;
  STATUS
    status ;
  ret = 0 ;
  offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                ee_block_03.un_ee_block_03.block_03.page_mode)) ;
  size = sizeof(((EE_AREA *)0)->
                ee_block_03.un_ee_block_03.block_03.page_mode) ;
  status = nvRamGet((char *)page_mode,(int)size,(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
  }
  return (ret) ;
}
#endif /*} LINUX */

void
  init_page_print(
    unsigned int lines_per_page
  )
{
  Abort_print_flag = 0 ;
  Num_lines_printed = 0 ;
  Num_lines_per_page = lines_per_page ;
  return ;
}
void
  set_lines_per_page(
    unsigned int lines_per_page
  )
{
  Num_lines_per_page = lines_per_page ;
  return ;
}
void
  clear_num_lines_printed(
    void
  )
{
  Num_lines_printed = 0 ;
  return ;
}
void
  inc_num_lines_printed(
    unsigned int add_lines
  )
{
  Num_lines_printed += add_lines ;
  return ;
}
/*
 * Indicate by how many lines there will be page
 * overflow if input parameter lines are added.
 */
unsigned int
  get_page_overflow(
    unsigned int add_lines
  )
{
  unsigned
    int
      ret ;
  if ((Num_lines_printed + add_lines) > Num_lines_per_page)
  {
    ret = (Num_lines_printed + add_lines) - Num_lines_per_page ;
  }
  else
  {
    ret = 0 ;
  }
  return (ret) ;
}
/*
 * Indicate how many lines current page can take
 * before overflow.
 */
unsigned int
  get_page_left(
    void
  )
{
  unsigned
    int
      ret ;
    ret = Num_lines_per_page - Num_lines_printed ;
  return (ret) ;
}
/*
 * }
 */
/*
 * {
 */
/*****************************************************
*NAME
*  get_num_of_chars
*  reset_num_of_chars
*  add_block_of_chars
*  clear_lines_from_chars
*TYPE: UTIL
*DATE: 30/JUL/2002
*FUNCTION:
*  Counts block of chars, and return the number of lines
*  seen in these blocks.
*CALLING SEQUENCE:
*  get_num_of_chars()
*  reset_num_of_chars()
*  add_block_of_chars(size)
*  clear_lines_from_chars()
*INPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*   clear_lines_from_chars - returns the number of full lines
*   it cleared from the total
*  SOC_SAND_INDIRECT:
*    the constant MAX_CHARS_ON_LINE (=79)
*REMARKS:
* Used by get_array_num_lines(), which calls add_block
* for every block of chars without \n it seess, calls
* reset whenever a \n is encountered, and calls clear_lines
* before it has to decide the number of rows it encountered.
*SEE ALSO:get_array_num_lines
 */
unsigned int Num_of_chars = 0;
/*
 */
unsigned int get_num_of_chars()
{
  return Num_of_chars;
}
/*
 */
void reset_num_of_chars()
{
  Num_of_chars = 0;
}
/*
 */
void add_block_of_chars(unsigned int size)
{
  Num_of_chars += size;
}
/*
 */
unsigned int clear_lines_from_chars()
{
  unsigned int bla = (Num_of_chars / (MAX_CHARS_ON_LINE + 1));
  Num_of_chars  -= (MAX_CHARS_ON_LINE + 1) * bla;
  return bla;
}
/*
 * }
 */
/*****************************************************
*NAME
*  get_array_num_lines
*TYPE: PROC
*DATE: 30/JUL/2002
*FUNCTION:
*  Get number of lines on input array.
*CALLING SEQUENCE:
*  get_array_num_lines(in_arr,size_arr)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *in_arr -
*      Text to inspect for number of lines.
*    unsigned int size_arr -
*      Number of characters in input array.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Number of lines in text (number of 'line feed'
*      characters in text), starting from 0.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Calculates both \n and full lines it sees (80 chars).
*  The full lines counter is reset every time a \n is met.
*  The calculation is not precise, and might be tricked
*  with \r, but currently is working fine.
*SEE ALSO:add_block_of_chars, clear_lines_from_chars
 */
int
  get_array_num_lines(
    char         *in_arr,
    unsigned int size_arr
  )
{
  int
    ret;
  unsigned int no_of_lines;
  char
    *old_c_ptr,
    *new_c_ptr ;
  old_c_ptr = new_c_ptr = in_arr ;
  ret = 0 ;
  while (TRUE)
  {
    new_c_ptr = memchr(old_c_ptr,(int)'\n',size_arr) ;
    if (new_c_ptr != (char *)0)
    {
      new_c_ptr++ ;
      size_arr -= (new_c_ptr - old_c_ptr) ;
      old_c_ptr = new_c_ptr ;
      ret++ ;
    }
    else
    {
      if (old_c_ptr > in_arr)
      {
        reset_num_of_chars();
      }
      add_block_of_chars(size_arr);
      break ;
    }
  }
  no_of_lines = clear_lines_from_chars();
  return (ret + no_of_lines);
}
/*****************************************************
*NAME
*  get_ptr_of_lf
*TYPE: PROC
*DATE: 03/APR/2002
*FUNCTION:
*  Get pointer to one character beyond the specified
*  line-feed character on input array.
*CALLING SEQUENCE:
*  get_ptr_of_lf(in_arr,lf_number,size_arr)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *in_arr -
*      Array to inspect for number of line feed
*      characters.
*    unsigned int lf_number -
*      Number of line-feed characters to look for.
*      If this number is '0' then address of first
*      character is returned. If there are not that
*      many line-feed characters in the input
*      string then a null pointer is returned.
*    unsigned int size_arr -
*      Number of characters to look at on input array.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Number of lines in array (number of 'line feed'
*      characters in array).
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
char
  *get_ptr_of_lf(
    char         *in_arr,
    unsigned int lf_number,
    unsigned int size_arr
  )
{
  char
    *ret ;
  char
    *old_c_ptr,
    *new_c_ptr ;

  new_c_ptr = old_c_ptr = in_arr ;
  if (lf_number == 0)
  {
    goto gpol_exit ;
  }
  while (TRUE)
  {
    new_c_ptr = memchr(old_c_ptr,(int)'\n',size_arr) ;
    if (new_c_ptr != (char *)0)
    {
      lf_number-- ;
      new_c_ptr++ ;
      size_arr -= (new_c_ptr - old_c_ptr) ;
      if (lf_number == 0)
      {
        break ;
      }
      old_c_ptr = new_c_ptr ;
    }
    else
    {
      break ;
    }
  }
gpol_exit:
  ret = new_c_ptr ;
  return (ret) ;
}
/*****************************************************
*NAME
*  ask_get
*TYPE: PROC
*DATE: 02/APR/2002
*FUNCTION:
*  Display a text line (question asking for response)
*  to the user on the console, and wait for response.
*CALLING SEQUENCE:
*  ask_get(ask_text,expect)
*INPUT:
*  SOC_SAND_DIRECT:
*    char * ask_text -
*      Text to display on screen. See remarks.
*    unsigned int expect -
*      Kind of response to expect:
*        EXPECT_YES_NO -
*          Wait for the user to hit 'y' or 'n'
*          and return with '0' for 'no' and
*          any other number for 'yes'.
*        EXPECT_CONT_ABORT -
*          Wait for the user to hit 'space' or any
*          other key (standing for 'continue' and
*          'abort', respectively) and return with '0'
*          for 'abort' and any other number for
*          'continue'.
*        EXPECT_ANY -
*          Wait for the user to hit any character
*          and return with the ASCII value of that
*          character.
*        EXPECT_CR -
*          Wait for the user to hit 'carriage return'
*          in order to continue and return with a
*          value of '1'.
*        Otherwise -
*          Same as EXPECT_YES_NO.
*    unsigned int one_line_mode -
*      Flag. If 'true' then one line question is presented
*      and is erased after user's respose. Otherwise,
*      the question text may be longer, may contain a
*      few lines, and is not erased after user's response.
*    unsigned int temporary_unsafe -
*      Flag. If 'true' then this procedure moves the
*      calling task from 'safe' state (if it is in that
*      state) to 'unsafe' for the period of waiting
*      for user's keyboard response. A task in 'Safe'
*      state is protected against deletion.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      See 'expect' above.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  In 'one_line_mode', this text must be short (less
*  than one line long) and contain no line-feed characters.
*  In any case, text should contain the proper request
*  for resposne.
*
*  Note that calling task's state may be temporarily
*  changed by this procedure.
*SEE ALSO:
 */
int
  ask_get(
    char         *ask_text,
    unsigned int expect,
    unsigned int one_line_mode,
    unsigned int temporary_unsafe
  )
{
  int
    err,
    ret ;
  unsigned
    int
      num_to_send ;
  char
    cc,
    ask_line[MAX_ONE_LINE_ASK + 1],
    ask_string[MAX_CHARS_ON_LINE + 3 + 5] ;
  unsigned
    int
      task_was_safe ;
  const char
    *proc_name ;
  proc_name = "ask_get" ;
  ret = 0 ;
  task_was_safe = FALSE;

  if (one_line_mode)
  {
    strncpy(ask_line,ask_text,MAX_ONE_LINE_ASK) ;
    ask_line[MAX_ONE_LINE_ASK] = 0 ;
    ask_text = &ask_line[0] ;
  }
  num_to_send = strlen(ask_text) ;
  if (temporary_unsafe)
  {
    if (d_is_task_safe())
    {
      task_was_safe = TRUE ;
      d_taskUnsafe() ;
    }
    else
    {
      task_was_safe = FALSE ;
    }
  }
  if (num_to_send)
  {
    err =
      d_write(d_ioGlobalStdGet(STD_IN),ask_text,num_to_send,TRUE) ;
    if (err == ERROR)
    {
      gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 1 - ",proc_name,
            SVR_WRN,ASK_GET_ERR_01,FALSE,0,-1,FALSE) ;
      goto asge_exit ;
    }
  }
  switch (expect)
  {
    case EXPECT_CONT_ABORT:
    {
      while (TRUE)
      {
        cc = sys_getch() ;
        if (cc == ' ')
        {
          ret = 1 ;
          strcpy(ask_string,"CONTINUE") ;
          num_to_send = strlen(ask_string) ;
          err =
            d_write(d_ioGlobalStdGet(STD_IN),ask_string,num_to_send,TRUE) ;
          if (err == ERROR)
          {
            gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 5 - ",proc_name,
                  SVR_WRN,ASK_GET_ERR_05,FALSE,0,-1,FALSE) ;
            goto asge_exit ;
          }
        }
        else
        {
          ret = 0 ;
          strcpy(ask_string,"ABORT") ;
          num_to_send = strlen(ask_string) ;
          err =
            d_write(d_ioGlobalStdGet(STD_IN),ask_string,num_to_send,TRUE) ;
          if (err == ERROR)
          {
            gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 5 - ",proc_name,
                  SVR_WRN,ASK_GET_ERR_05,FALSE,0,-1,FALSE) ;
            goto asge_exit ;
          }
        }
        if (one_line_mode)
        {
          strcpy(ask_string,"\r") ;
          fill_blanks(ask_string,MAX_CHARS_ON_SCREEN_LINE,0) ;
          UTILS_STRCAT_SAFE(ask_string,"\r", MAX_CHARS_ON_LINE + 3 + 5) ;
        }
        else
        {
          strcpy(ask_string,"\r\n") ;
        }
        num_to_send = strlen(ask_string) ;
        err =
          d_write(d_ioGlobalStdGet(STD_IN),ask_string,num_to_send,TRUE) ;
        if (err == ERROR)
        {
          gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 2 - ",proc_name,
                SVR_WRN,ASK_GET_ERR_02,FALSE,0,-1,FALSE) ;
          goto asge_exit ;
        }
        goto asge_exit ;
      }
      break ;
    }
    case EXPECT_ANY:
    {
      ret = (int)((unsigned int)sys_getch()) ;
      break ;
    }
    case EXPECT_CR:
    {
      while (TRUE)
      {
        cc = sys_getch() ;
        if ((cc == '\r') || (cc == '\n'))
        {
          ret = 1 ;
          strcpy(ask_string,"Carriage Return") ;
          num_to_send = strlen(ask_string) ;
          err =
            d_write(d_ioGlobalStdGet(STD_IN),ask_string,num_to_send,TRUE) ;
          if (err == ERROR)
          {
            gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 5 - ",proc_name,
                  SVR_WRN,ASK_GET_ERR_05,FALSE,0,-1,FALSE) ;
            goto asge_exit ;
          }
          if (one_line_mode)
          {
            strcpy(ask_string,"\r") ;
            fill_blanks(ask_string,MAX_CHARS_ON_SCREEN_LINE,0) ;
            UTILS_STRCAT_SAFE(ask_string,"\r", MAX_CHARS_ON_LINE + 3 + 5) ;
          }
          else
          {
            strcpy(ask_string,"\r\n") ;
          }
          num_to_send = strlen(ask_string) ;
          err =
            d_write(d_ioGlobalStdGet(STD_IN),ask_string,num_to_send,TRUE) ;
          if (err == ERROR)
          {
            gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 2 - ",proc_name,
                  SVR_WRN,ASK_GET_ERR_02,FALSE,0,-1,FALSE) ;
            goto asge_exit ;
          }
        }
      }
      break ;
    }
    default:
    case EXPECT_YES_NO:
    {
      while (TRUE)
      {
        cc = sys_getch() ;
        if ((cc == 'y') || (cc == 'Y') || (cc == 'n') || (cc == 'N'))
        {
          if ((cc == 'y') || (cc == 'Y'))
          {
            ret = 1 ;
            strcpy(ask_string,"YES") ;
            num_to_send = strlen(ask_string) ;
            err =
              d_write(d_ioGlobalStdGet(STD_IN),ask_string,num_to_send,TRUE) ;
            if (err == ERROR)
            {
              gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 5 - ",proc_name,
                    SVR_WRN,ASK_GET_ERR_05,FALSE,0,-1,FALSE) ;
              goto asge_exit ;
            }
          }
          else
          {
            ret = 0 ;
            strcpy(ask_string,"NO") ;
            num_to_send = strlen(ask_string) ;
            err =
              d_write(d_ioGlobalStdGet(STD_IN),ask_string,num_to_send,TRUE) ;
            if (err == ERROR)
            {
              gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 5 - ",proc_name,
                    SVR_WRN,ASK_GET_ERR_05,FALSE,0,-1,FALSE) ;
              goto asge_exit ;
            }
          }
          if (one_line_mode)
          {
            strcpy(ask_string,"\r") ;
            fill_blanks(ask_string,MAX_CHARS_ON_SCREEN_LINE,0) ;
            UTILS_STRCAT_SAFE(ask_string,"\r", MAX_CHARS_ON_LINE + 3 + 5) ;
          }
          else
          {
            strcpy(ask_string,"\r\n") ;
          }
          num_to_send = strlen(ask_string) ;
          err =
            d_write(d_ioGlobalStdGet(STD_IN),ask_string,num_to_send,TRUE) ;
          if (err == ERROR)
          {
            gen_err(FALSE,TRUE,(int)err,ERROR,"ask_get 2 - ",proc_name,
                  SVR_WRN,ASK_GET_ERR_02,FALSE,0,-1,FALSE) ;
            goto asge_exit ;
          }
          goto asge_exit ;
        }
      }
      break ;
    }
  }
asge_exit:
  if (temporary_unsafe)
  {
    if (task_was_safe)
    {
      d_taskSafe() ;
    }
  }
exit:
  return (ret) ;
}
#if DEBUG_AID
/* { */
/*****************************************************
*NAME
*  aa_value_checker
*TYPE: PROC
*DATE: 18/FEB/2002
*FUNCTION:
*  Example for procedure for checking numeric value.
*CALLING SEQUENCE:
*  aa_value_checker(numeric_value,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long numeric_value -
*      Value to check.
*    char *err_msg -
*      This output is only meaningful when return value
*      is non-zero. Pointer to string to load with
*      explanation of why validation of input parameter
*      has failed.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Int -
*      If non zero then value is not valid.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  aa_value_checker(
    unsigned long numeric_value,
    char     *err_msg
  )
{
  int
    ret ;
  ret = 0 ;
  err_msg[0] = 0 ;
  if ((long)numeric_value > 8)
  {
    ret = 1 ;
    sal_sprintf(err_msg,
      "*** aa_value_checker error.\r\n") ;
  }
  return (ret) ;
}
/* } */
#endif
