/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * INCLUDE FILES:
 */
#if !DUNE_BCM

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
/* } */
#endif /* !DUNE_BCM */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/ui_defx.h>
#include <appl/dpp/UserInterface/ui_defi.h>

/*
 * Static variables.
 * {
 */
/*
 * }
 */
/*****************************************************
*NAME
*  cut_text_params_quotation_marks
*TYPE: PROC
*DATE: 19/MARCH/2003
*FUNCTION:
*  Search all valid entries in 'param_val_pair' array
*  on 'current_line' which are with 'free text' values
*  and take enclosing quotation marks off.
*CALLING SEQUENCE:
*  cut_text_params_quotation_marks(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to operate on.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then operation could NOT be
*      carried out.
*  SOC_SAND_INDIRECT:
*    See 'current_line'.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  cut_text_params_quotation_marks(
    CURRENT_LINE *current_line
  )
{
  int
    ret ;
  char
    *s_ptr,
    *d_ptr ;
  PARAM_VAL
    *text_param ;
  unsigned
    int
      ui,
      uj,
      text_len ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  ret = 0 ;
  param_val_pair = current_line->param_val_pair ;
  for (ui = 0 ; ui < current_line->param_index ; ui++, param_val_pair++)
  {
    if ((param_val_pair->param_value.val_type & VAL_TEXT_QUOTATION) == VAL_TEXT)
    {
      text_param = &(param_val_pair->param_value) ;
      if (text_param->value.val_text[0] == QUOTATION_MARK)
      {
        text_len = strlen(text_param->value.val_text) - 2 ;
        /*
         * Copy, dropping enclosing quotation marks.
         */
        s_ptr = &(text_param->value.val_text[1]) ;
        d_ptr = &(text_param->value.val_text[0]) ;
        for (uj = 0 ; uj < text_len ; uj++)
        {
          *d_ptr++ = *s_ptr++ ;
        }
        /*
         * Add ending NULL.
         */
        *d_ptr = 0 ;
        /*
         * Mark text value as string which has been taken quotations off.
         */
        param_val_pair->param_value.val_type |= VAL_TEXT_QUOTATION ;
      }
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  paste_text_params_quotation_marks
*TYPE: PROC
*DATE: 19/MARCH/2003
*FUNCTION:
*  Search all valid entries in 'param_val_pair' array
*  on 'current_line' which are with 'free text with
*  enclosing quotations taken out' values
*  and add enclosing quotation marks.
*CALLING SEQUENCE:
*  paste_text_params_quotation_marks(current_line)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to operate on.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then operation could NOT be
*      carried out.
*  SOC_SAND_INDIRECT:
*    See 'current_line'.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  paste_text_params_quotation_marks(
    CURRENT_LINE *current_line
  )
{
  int
    ret ;
  char
    *s_ptr,
    *d_ptr ;
  PARAM_VAL
    *text_param ;
  unsigned
    int
      ui,
      uj,
      text_len ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  ret = 0 ;
  param_val_pair = current_line->param_val_pair ;
  for (ui = 0 ; ui < current_line->param_index ; ui++, param_val_pair++)
  {
    if ((param_val_pair->param_value.val_type & VAL_TEXT_QUOTATION) == VAL_TEXT_QUOTATION)
    {
      text_param = &(param_val_pair->param_value) ;
      text_len = strlen(text_param->value.val_text) ;
      /*
       * Make sure there is place for extra two quotation marks. Take
       * ending null into account.
       */
      if ((text_len + 2 + 1) > sizeof(text_param->value.val_text))
      {
        ret = 1 ;
        break ;
      }
      /*
       * Copy, adding enclosing quotation marks.
       */
      s_ptr = &(text_param->value.val_text[text_len - 1]) ;
      d_ptr = &(text_param->value.val_text[text_len + 2]) ;
      /*
       * Add ending NULL.
       */
      *d_ptr-- = 0 ;
      /*
       * Add ending quotation mark.
       */
      *d_ptr-- = QUOTATION_MARK ;
      for (uj = 0 ; uj < text_len ; uj++)
      {
        *d_ptr-- = *s_ptr-- ;
      }
      /*
       * Add leading quotation mark.
       */
      *d_ptr = QUOTATION_MARK ;
      /*
       * Mark text value as string without handled quotations.
       */
      param_val_pair->param_value.val_type &= ~VAL_TEXT_QUOTATION ;
      param_val_pair->param_value.val_type |= VAL_TEXT ;
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  search_param_val_pairs
*TYPE: PROC
*DATE: 30/JAN/2002
*FUNCTION:
*  Search all valid entries in 'param_val_pair' array
*  on 'current_line' for the n'th occurrence of a
*  specific parameter.
*CALLING SEQUENCE:
*  search_param_val_pairs(current_line,
                    match_index,param_id,ocr_num)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to search.
*    unsigned int *match_index -
*      On output, this procedure loads memory
*      pointed by this parameter by index of
*      occurrence no. 'ocr_num' of this
*      parameter in array 'param_val_pair' on
*      'current_line'. (only meaningful if return
*      value is 'false')
*    int param_id -
*      Identifier of the parameter to search for.
*    unsigned int ocr_num -
*      number of occurrence to look for. Starting
*      from '1'.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then specific occerrence of
*      this parameter could NOT be found.
*  SOC_SAND_INDIRECT:
*    See 'match_index'.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  search_param_val_pairs(
    CURRENT_LINE *current_line,
    unsigned int *match_index,
    int          param_id,
    unsigned int ocr_num
  )
{
  int
    current_param_id,
    ret ;
  unsigned
    int
      ocr_cnt,
      ui ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  PARAM
    *allowed_params ;
  ret = TRUE ;
  allowed_params =
        current_line->line_subject->allowed_params ;
  param_val_pair = current_line->param_val_pair ;
  ocr_cnt = 1 ;
  for (ui = 0 ; ui < current_line->param_index ; ui++)
  {
    current_param_id = allowed_params[param_val_pair[ui].param_match_index].param_id ;
    if (current_param_id == param_id)
    {
      if (ocr_cnt == ocr_num)
      {
        ret = FALSE ;
        *match_index = ui ;
        break ;
      }
      ocr_cnt++ ;
    }
  }
  if (ret)
  {
    /*
     * Check the last entry only if parameter string has already been
     * entered.
     */
    if (current_line->param_string_entered)
    {
      current_param_id = allowed_params[param_val_pair[ui].param_match_index].param_id ;
      if (current_param_id == param_id)
      {
        if (ocr_cnt == ocr_num)
        {
          *match_index = ui ;
          ret = FALSE ;
        }
      }
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  get_val_of
*TYPE: PROC
*DATE: 24/FEB/2002
*FUNCTION:
*  Search all valid entries in 'param_val_pair' array
*  on 'current_line' for the n'th occurrence of a
*  specific parameter and get its value (either
*  from the line itself of from the default
*  value on this parameters description).
*CALLING SEQUENCE:
*  get_val_of(current_line,
                    match_indicator,param_id,
*                   ocr_num,param_val)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to search.
*    int *match_indicator -
*      On output, this procedure loads memory
*      pointed by this parameter by index of
*      occurrence no. 'ocr_num' of this
*      parameter in array 'param_val_pair' on
*      'current_line'. If parameter value could not
*      be found on the line then a negative
*      value is returned. (only meaningful if return
*      value is 'false')
*    int param_id -
*      Identifier of the parameter to search for.
*    unsigned int ocr_num -
*      Number of occurrence to look for. Starting
*      from '1'.
*    PARAM_VAL **param_val -
*      On Output, location to store pointer to
*      found value of parameter. (only meaningful
*      if return value is 'false')
*    unsigned long param_type -
*      Indication on the kind of parameter caller
*      expects. Must be a mask made out of a combination
*      of VAL_NUMERIC, VAL_SYMBOL, VAL_TEXT, VAL_IP.
*    char *err_msg -
*      On Output, location to store description
*      of error condition. Caller should
*      prepare place for up to 160 characters,
*      including ending null. Error text always
*      ends with CR-LF (only meaningful
*      if return value is 'TRUE').
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then specific occerrence of
*      this parameter could NOT be found.
*  SOC_SAND_INDIRECT:
*    See 'match_index'.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  get_val_of(
    CURRENT_LINE  *current_line,
    int           *match_indicator,
    int           param_id,
    unsigned int  ocr_num,
    PARAM_VAL     **param_val,
    unsigned long param_type,
    char          *err_msg
  )
{
  int
    ret ;
  PARAM_VAL_PAIR
    *param_val_pair ;
  unsigned
    int
      match_index ;
  PARAM
    *parameter_ptr,
    *allowed_params ;
  unsigned
    int
      ui,
      num_allowed_params ;
  allowed_params =
      current_line->line_subject->allowed_params ;
  num_allowed_params =
      current_line->line_subject->num_allowed_params ;
  ret = TRUE ;
  err_msg[0] = 0 ;
  if (search_param_val_pairs(current_line,&match_index,param_id,ocr_num) == FALSE)
  {
    /*
     * Enter if parameter has been found on this line.
     */
    param_val_pair = &current_line->param_val_pair[match_index] ;
    if (param_val_pair->param_value.val_type == VAL_NONE)
    {
      /*
       * No value found. Use default.
       */
      parameter_ptr = &allowed_params[param_val_pair->param_match_index] ;
      if ((parameter_ptr->default_care & HAS_DEFAULT_MASK) == HAS_DEFAULT)
      {
        if (param_type & parameter_ptr->default_val->val_type)
        {
          /*
           * Enter if expected type of value has been found.
           */
          *match_indicator = -1 ;
          *param_val = parameter_ptr->default_val ;
          ret = FALSE ;
          goto geva_exit ;
        }
        else
        {
          /*
           * Enter if expected type of value has NOT been found.
           */
          sal_sprintf(err_msg,"Expected value type does not match default type.\r\n") ;
          goto geva_exit ;
        }
      }
      else
      {
        /*
         * Parameter does not have a default value.
         * Nothing to return with!.
         */
        sal_sprintf(err_msg,
            "Value was not entered on current line and there is no default.\r\n") ;
        goto geva_exit ;
      }
    }
    else if (param_val_pair->param_value.val_type & param_type)
    {
      /*
       * Enter if parameter was found on current line and it is of the
       * type(s) required by the user.
       */
      *match_indicator = match_index ;
      *param_val = &(param_val_pair->param_value) ;
      ret = FALSE ;
      goto geva_exit ;
    }
    else
    {
      /*
       * Enter if parameter was found on current line but it is
       * not of the type(s) required by the user.
       */
      sal_sprintf(err_msg,"Value is on current line but is not of required type.\r\n") ;
      goto geva_exit ;
    }
  }
  else
  {
    /*
     * Enter if parameter could not be found on this line.
     */
    unsigned
      int
        uk,
        num_elements,
        found ;
    unsigned
      long
        compare,
        current_mutex[NUM_ELEMENTS_MUTEX_CONTROL] ;
    /*
     * If 'ocr_num' is larger than '1' then return with
     * error since there is default for first occurrence
     * only.
     */
    if (ocr_num > 1)
    {
      sal_sprintf(
        err_msg,
        "Value is not on current line and there is no default for\r\n"
        "second occurrence or higher.\r\n") ;
      goto geva_exit ;
    }
    /*
     * Search for parameter on this subject's list.
     * Search only parameters which are within the current mutex context.
     */
    get_current_mutex_control(current_line,current_mutex) ;
    found = FALSE ;
    parameter_ptr = allowed_params ;
    num_elements = sizeof(current_mutex) / sizeof(current_mutex[0]) ;
    for (ui = 0 ; ui < num_allowed_params ; ui++,parameter_ptr++)
    {
      compare = 0 ;
      for (uk = 0 ; uk < num_elements ; uk++)
      {
        compare |= (parameter_ptr->mutex_control[uk] & current_mutex[uk]) ;
      }
      if ((parameter_ptr->param_id == param_id) && (compare))
      {
        found = TRUE ;
        break ;
      }
    }
    if (!found)
    {
      sal_sprintf(
        err_msg,
        "Parameter ID \'%d\' could not be found on subject\'s list.\r\n",param_id) ;
      goto geva_exit ;
    }
    if ((parameter_ptr->default_care & HAS_DEFAULT_MASK) == HAS_DEFAULT)
    {
      if (param_type & parameter_ptr->default_val->val_type)
      {
        /*
         * Enter if expected type of value has been found.
         */
        *match_indicator = -1 ;
        *param_val = parameter_ptr->default_val ;
        ret = FALSE ;
        goto geva_exit ;
      }
      else
      {
        /*
         * Enter if expected type of value has NOT been found.
         */
        sal_sprintf(err_msg,"Expected value type does not match default type.\r\n") ;
        goto geva_exit ;
      }
    }
    else
    {
      /*
       * Parameter could not be found on the line and it does not
       * have a default value.
       */
      sal_sprintf(err_msg,
        "Parameter ID \'%d\' could not be found on the line and it does not\r\n"
        "have a default value.\r\n",param_id
        ) ;
      goto geva_exit ;
    }
  }
geva_exit:
  if ((ret == FALSE) && ((param_type & VAL_TEXT) == VAL_TEXT))
  {
    /*
     * Enter if parameter value is of 'text' type and
     * it has been found. Strip it of surrounding
     * quotation marks.
     */
    unsigned
      int
        text_len ;
    static
      PARAM_VAL
        *text_param,
        stripped_text_param ;
    text_param = *param_val ;
    if (text_param->value.val_text[0] == QUOTATION_MARK)
    {
      text_len = strlen(text_param->value.val_text) - 2 ;
      /*
       * Copy value to local static variable.
       */
      stripped_text_param = *text_param ;
      memcpy(stripped_text_param.value.val_text,
                      &(text_param->value.val_text[1]),text_len) ;
      stripped_text_param.value.val_text[text_len] = 0 ;
      *param_val = &stripped_text_param ;
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  line_mode_handling
*TYPE: PROC
*DATE: 28/APR/2002
*FUNCTION:
*  Process input line by rules of CLI. Input line
*  is assumed to NOT have been checked at all since
*  line_mode is set!.
*CALLING SEQUENCE:
*  line_mode_handling(current_line,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    char *err_msg -
*      On Output, location to store description
*      of error condition. Caller should
*      prepare place for up to 320 characters
*      plus MAX_CHARS_ON_LINE,
*      including ending null. Error text always
*      ends with CR-LF (only meaningful
*      if return value is non-zero).
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    current_line.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  line_mode_handling(
    CURRENT_LINE *current_line,
    char         *err_msg
  )
{
  int
    ret ;
  unsigned
    long
      enter_case,
      field_len,
      ul,
      index,
      num_left,
      diff,
      prev_num_loaded,
      num_loaded,
      num_analyzed ;
  char
    in_char,
    *field_start,
    *c_ptr,
    line_buf[MAX_CHARS_ON_LINE + 2] ;
  char
    *proc_name ;
  proc_name = "line_mode_handling" ;
  err_msg[0] = 0 ;
  ret = 0 ;
  num_loaded = current_line->num_loaded ;
  /*
   * Take all extra spaces out.
   */
  /*
   * First, take out leading spaces.
   */
  for (ul = 0 ; ul < num_loaded ; ul++)
  {
    if (current_line->line_buf[ul] != BLANK_SPACE)
    {
      break ;
    }
  }
  /*
   * 'ul' is now the index of the first non-zero character.
   */
  if (ul)
  {
    num_loaded -= ul ;
    memcpy(line_buf,&current_line->line_buf[ul],num_loaded) ;
    memcpy(current_line->line_buf,line_buf,num_loaded) ;
    current_line->num_loaded = num_loaded ;
  }
  if (num_loaded == 0)
  {
    sal_sprintf(err_msg,
      "*** Nothing but spaces on this line...") ;
    ret = 1 ;
    goto limo_exit ;
  }
  if (num_loaded > 2)
  {
    /*
     * Now, replace multiple spaces by single spaces.
     */
    for (ul = 1 ; ul < (num_loaded - 1) ; ul++)
    {
      if ((current_line->line_buf[ul] == BLANK_SPACE) &&
              (current_line->line_buf[ul + 1] == BLANK_SPACE))
      {
        memcpy(line_buf,&current_line->line_buf[ul + 2],(num_loaded - ul - 2)) ;
        memcpy(&current_line->line_buf[ul + 1],line_buf,(num_loaded - ul - 2)) ;
        ul-- ;
        num_loaded-- ;
      }
    }
    ul = current_line->num_loaded - num_loaded ;
    current_line->num_loaded = num_loaded ;
  }
  if (current_line->line_buf[num_loaded - 1] == BLANK_SPACE)
  {
    /*
     * Get rid of trailing blank, if any.
     */
    num_loaded-- ;
    current_line->num_loaded = num_loaded ;
  }
  /*
   * Not necessary but just make sure...
   */
  current_line->line_buf[num_loaded] = 0 ;
  /*
   * Copy original line (without spaces) into local storage.
   * Also, keep number of meaningful element in
   * local variable: num_loaded ;
   */
  memcpy(line_buf,current_line->line_buf,sizeof(line_buf)) ;
  /*
   * Make sure this line has no history.
   */
  current_line->num_param_names = 0 ;
  current_line->num_param_values = 0 ;
  current_line->param_string_entered = FALSE ;
  current_line->subject_entered = FALSE ;
  current_line->param_index = 0 ;
  /*
   * Not necessary but good for debugging...
   */
  memset(current_line->parameters,0,sizeof(current_line->parameters)) ;
  current_line->quotation_mark_entered = FALSE ;
  /*
   * At this point, 'num_loaded' contains the actual number of
   * characters on this command line.
   * current_line->num_loaded will now gradually be increased
   * as more items are analyzed.
   */
  /*
   * Search for first space and check subject.
   */
  c_ptr = memchr(current_line->line_buf,' ',num_loaded) ;
  if (c_ptr)
  {
    /*
     * Enter if space has been found.
     */
    current_line->num_loaded = (unsigned long)(c_ptr - current_line->line_buf) ;
    enter_case = FALSE ;
  }
  else
  {
    /*
     * Enter if space has NOT been found. This is the ONLY word on the line.
     */
    enter_case = TRUE ;
  }
  prev_num_loaded =
    current_line->num_loaded_this_field = current_line->num_loaded ;
  current_line->start_index_this_field = 0 ;
  current_line->line_buf_index = current_line->num_loaded_this_field ;
  ret = space_while_receiving_subject(current_line,enter_case) ;
  if (ret)
  {
    ret = 2 ;
    goto limo_exit ;
  }
  if (enter_case)
  {
    goto limo_exit ;
  }
  /*
   * num_analyzed contains the number of characters analyzed so far,
   * including trainling space.
   */
  num_analyzed = current_line->num_loaded ;
  do
  {
    char
      found_names_list[80*10] ;
    unsigned
      int
        count,
        search_space ;
    PARAM
      *parameter_ptr ;

    field_start = &current_line->line_buf[num_analyzed] ;
    in_char = *field_start ;
    search_space = TRUE ;
    num_left = num_loaded - num_analyzed ;
    parameter_ptr =
      &current_line->line_subject->
          allowed_params[current_line->current_param_match_index] ;
    if (((current_line->proc_state == EXPECTING_PARAM_VAL) ||
            (current_line->proc_state == RECEIVING_PARAM_VAL)) &&
        (parameter_ptr->num_text_vals != 0))
    {
      if (in_char == QUOTATION_MARK)
      {
        search_space = FALSE ;
        /*
         * Search for the matching QUOTATION_MARK.
         */
        c_ptr = memchr(field_start + 1,QUOTATION_MARK,num_left - 1) ;
        if (c_ptr)
        {
          /*
           * Enter if matching QUOTATION_MARK has been found.
           * If it is followed by a SPACE then point c_ptr to it.
           * Otherwise, set c_ptr to NULL, indicating this is the last
           * input field on the line.
           */
          if (((unsigned long)(c_ptr - field_start) + 1) == num_left)
          {
            c_ptr = (char *)0 ;
          }
          else if (c_ptr[1] != BLANK_SPACE)
          {
            /*
             * Enter if the second QUOTATION_MARK is not followed
             * by BLANK_SPACE. This is a syntax error.
             */
            sal_sprintf(err_msg,
              "*** Stopped processing command line at:\r\n\n") ;
            index = strlen(err_msg) ;
            count = num_analyzed + (unsigned long)(c_ptr - field_start) ;
            memcpy(&err_msg[index],current_line->line_buf,count) ;
            err_msg[index + count] = 0 ;
            index = strlen(err_msg) ;
            sal_sprintf(&err_msg[index],"\r\n") ;
            index = strlen(err_msg) ;
            sal_sprintf(&err_msg[index],
              "*** Matching quotation mark not followed by space\r\n") ;
            index = strlen(err_msg) ;
            ret = 13 ;
            current_line->line_buf_index = current_line->num_loaded = count ;
            goto limo_exit ;
          }
          else
          {
            /*
             * Enter if the second QUOTATION_MARK is followed
             * by BLANK_SPACE. Point c_ptr to it.
             */
            c_ptr++ ;
          }
        }
        else
        {
          /*
           * Enter if NO matching QUOTATION_MARK has been found!
           */
          sal_sprintf(err_msg,
            "*** Stopped processing command line at:\r\n\n") ;
          index = strlen(err_msg) ;
          count = num_analyzed + num_left ;
          memcpy(&err_msg[index],current_line->line_buf,count) ;
          err_msg[index + count] = 0 ;
          index = strlen(err_msg) ;
          sal_sprintf(&err_msg[index],"\r\n") ;
          index = strlen(err_msg) ;
          sal_sprintf(&err_msg[index],
            "*** Matching quotation mark not found on this line\r\n") ;
          index = strlen(err_msg) ;
          current_line->line_buf_index = current_line->num_loaded = count ;
          ret = 14 ;
          goto limo_exit ;
        }
        current_line->quotation_mark_entered = FALSE ;
      }
    }
    if (search_space)
    {
      /*
       * Search for next space and check subject.
       */
      c_ptr = memchr(field_start,' ',num_left) ;
    }
    if (c_ptr)
    {
      /*
       * Enter if space has been found.
       */
      enter_case = FALSE ;
      field_len = (unsigned long)(c_ptr - field_start) ;
    }
    else
    {
      /*
       * Enter if space has NOT been found. This is the LAST word on the line.
       */
      enter_case = TRUE ;
      field_len = (unsigned long)(num_left) ;
    }
    prev_num_loaded =
      current_line->num_loaded = num_analyzed + field_len ;
    current_line->num_loaded_this_field = field_len ;
    current_line->start_index_this_field = num_analyzed ;
    current_line->line_buf_index = current_line->num_loaded ;
    switch (current_line->proc_state)
    {
      case EXPECTING_ENTER:
      {
        sal_sprintf(err_msg,
          "*** No more parameters expected on this line:\r\n") ;
        index = strlen(err_msg) ;
        memcpy(&err_msg[index],current_line->line_buf,num_analyzed) ;
        err_msg[index + num_analyzed] = 0 ;
        index = strlen(err_msg) ;
        sal_sprintf(&err_msg[index],"\r\n") ;
        index = strlen(err_msg) ;
        ret = 4 ;
        goto limo_exit ;
        break ;
      }
      case EXPECTING_PARAM_STRING:
      case RECEIVING_PARAM_STRING:
      {
        ret = space_receiving_param_string(current_line,enter_case) ;
        if (ret)
        {
          ret = 5 ;
          goto limo_exit ;
        }
        if (enter_case)
        {
          goto limo_exit ;
        }
        break ;
      }
      case EXPECTING_PARAM_VAL:
      case RECEIVING_PARAM_VAL:
      {
        ret = space_receiving_param_val(current_line,enter_case) ;
        if (ret)
        {
          ret = 7 ;
          goto limo_exit ;
        }
        if (enter_case)
        {
          goto limo_exit ;
        }
        break ;
      }
      case (EXPECTING_PARAM_VAL | EXPECTING_PARAM_STRING):
      {
        /*
         * This is a special case. We need to determine first whether
         * this field need to be processed as a parameter value
         * or as a parameter string.
         */
        if (current_line->num_param_values == 0)
        {
          /*
           * Note that since 'num_param_values' is '0',
           * no value has been entered for this paramater.
           */
          int
            num_params ;
          unsigned int
            match_index,
            full_match,
            common_len ;
          char
            *param_name ;
          int
            num_params_x ;
          unsigned int
            match_index_x,
            full_match_x,
            common_len_x ;
          char
            *param_name_x ;
          PARAM
            *parameter_ptr ;
          unsigned int
            num_vals ;

          parameter_ptr =
              &current_line->line_subject->
                  allowed_params[current_line->current_param_match_index] ;
          num_vals = parameter_ptr->num_allowed_vals ;
          /*
           * Since this is not 'insert' mode, 'num_loaded' contains
           * the number of characters in current subject.
           */
          /*
           * If there are IP address values for this field then no
           * other types are expected (i.e. IP address values do not
           * mix with others).
           */
          if (parameter_ptr->num_ip_vals != 0)
          {
            /*
             * Start receiving IP address value.
             * The state must now be RECEIVING_PARAM_VAL.
             */
            current_line->proc_state = RECEIVING_PARAM_VAL ;
          }
          /*
           * If there are free text values for this field then no
           * other types are expected (i.e. free text does not
           * mix with numeric of symbolic).
           */
          else if (parameter_ptr->num_text_vals != 0)
          {
            if (in_char == QUOTATION_MARK)
            {
              /*
               * Start receiving free text value.
               * The state must now be RECEIVING_PARAM_VAL.
               */
              current_line->proc_state = RECEIVING_PARAM_VAL ;
              current_line->quotation_mark_entered = TRUE ;
            }
            else if (is_char_legitimate_text(in_char))
            {
              /*
               * Start receiving free text value.
               * The state must now be RECEIVING_PARAM_VAL.
               */
              current_line->proc_state = RECEIVING_PARAM_VAL ;
            }
          }
          else
          {
            if (is_char_numeric_starter(in_char))
            {
              /*
               * This is the beginning of a numeric value field.
               */
              current_line->proc_state = RECEIVING_PARAM_VAL ;
            }
            else
            {
              /*
               * This is the beginning of a symbol.
               * We need to find out whether this symbol is a
               * value for current parameter or a new parameter.
               */
              /*
               * Check whether there is a symbolic field in the
               * list of values attached to the parameter
               * currently active.
               */
              unsigned
                long
                  current_mutex[NUM_ELEMENTS_MUTEX_CONTROL] ;
              unsigned
                int
                  current_ordinal ;
              num_params =
                search_params(
                    &current_line->line_buf[
                        current_line->start_index_this_field],
                    current_line->num_loaded_this_field,
                    TRUE,
                    0,&match_index,&full_match,
                    &param_name,&common_len,
                    (char *)(parameter_ptr->allowed_vals),
                    num_vals,
                    sizeof(*(parameter_ptr->allowed_vals)),
                    OFFSETOF(PARAM_VAL_RULES,symb_val),
                    found_names_list,
                    FALSE,
                    0,0,
                    (char **)0,0,0,0
                    ) ;
              get_current_mutex_control(current_line,current_mutex) ;
              current_ordinal =
                get_current_ordinal(current_line,current_mutex,0) ;
              num_params_x =
                search_params(
                    &current_line->line_buf[
                          current_line->start_index_this_field],
                    current_line->num_loaded_this_field,
                    TRUE,
                    0,&match_index_x,&full_match_x,
                    &param_name_x,&common_len_x,
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
              if ((num_params == 0) && (num_params_x == 0))
              {
                sal_sprintf(err_msg,
                  "\r\n\n"
                  "Processing ended at:\r\n") ;
                index = strlen(err_msg) ;
                memcpy(&err_msg[index],current_line->line_buf,num_analyzed) ;
                err_msg[index + num_analyzed] = 0 ;
                index = strlen(err_msg) ;
                sal_sprintf(&err_msg[index],"\r\n") ;
                index = strlen(err_msg) ;
                sal_sprintf(&err_msg[index],
                  "*** No symbolic value or parameter, starting"
                  " with \'%c\', was found.\r\n\n",in_char) ;
                index = strlen(err_msg) ;
                ret = 10 ;
                goto limo_exit ;
              }
              else
              {
                /*
                 * Enter if either a symbolic value or a parameter
                 * has been found (or both!).
                 */
                if ((num_params > 0) && (num_params_x == 0))
                {
                  /*
                   * At least one symbolic value has been found
                   * while no parameter has been found.
                   * The state must now be RECEIVING_PARAM_VAL.
                   */
                  current_line->proc_state = RECEIVING_PARAM_VAL ;
                }
                else if ((num_params == 0) && (num_params_x > 0))
                {
                  /*
                   * At least one parameter has been found
                   * while no symbolic value has been found.
                   * The state must now be RECEIVING_PARAM_STRING.
                   * Also, we need to advance to the next
                   * entry in 'param_val_pairs' array and do
                   * all 'blank space' actions which have been
                   * posponed because of the fuzzy state.
                   */
                  PARAM_VAL_PAIR
                    *param_val_pair ;
                  current_line->proc_state = RECEIVING_PARAM_STRING ;
                  param_val_pair =
                    &current_line->param_val_pair[current_line->param_index] ;
                  param_val_pair->in_val_index = 0 ;
                  param_val_pair->param_value.val_type = VAL_NONE ;
                  current_line->param_index++ ;
                  current_line->param_string_entered = FALSE ;
                  /*
                   * Redundant but safe.
                   */
                  param_val_pair =
                    &current_line->param_val_pair[current_line->param_index] ;
                  param_val_pair->par_name = "" ;
                  param_val_pair->in_val_index = 0 ;
                  param_val_pair->param_value.val_type = VAL_NONE ;
                }
                else
                {
                  /*
                   * At least one parameter has been found
                   * and also at least one symbolic value has been found.
                   * For this version, prefer the symbolic value.
                   */
                  current_line->proc_state = RECEIVING_PARAM_VAL ;
                }
              }
            }
          }
        }
        else
        {
          /*
           * Enter if 'num_param_values' is non zero:
           * Some values have been entered for this paramater.
           * This means that either numeric values can be
           * expected or a parameter string. Symbolic values are not
           * acceptable here. Also, since the first numeric
           * value has been entered, this parameter accepts
           * numeric values and there is no need to recheck.
           */
          int
            num_params_x ;
          unsigned int
            match_index_x,
            full_match_x,
            common_len_x ;
          char
            *param_name_x ;
          PARAM
            *parameter_ptr ;
          parameter_ptr =
                &current_line->line_subject->
                    allowed_params[current_line->current_param_match_index] ;
          /*
           * Since this is not 'insert' mode, 'num_loaded' contains
           * the number of characters in current subject.
           */
          if (is_char_numeric_starter(in_char))
          {
            /*
             * This is the beginning of a numeric value field.
             */
            /*
             * No need to check whether there is a numeric
             * field in the list of values attached to the parameter
             * currently active.
             */
            /*
             * Issue a warning if this is the last value allowed for
             * this parameter and more than one value is allowed.
             */
            unsigned
              long
                max_num_repeated ;
            max_num_repeated =
              parameter_ptr->allowed_vals[parameter_ptr->numeric_index].
                            val_descriptor.val_num_descriptor.max_num_repeated ;
            if (max_num_repeated > 1)
            {
              if (current_line->num_param_values >= max_num_repeated)
              {
                sal_sprintf(err_msg,
                  "\r\n\n"
                  "Processing ended at:\r\n") ;
                index = strlen(err_msg) ;
                memcpy(&err_msg[index],current_line->line_buf,num_analyzed) ;
                err_msg[index + num_analyzed] = 0 ;
                index = strlen(err_msg) ;
                sal_sprintf(&err_msg[index],"\r\n") ;
                index = strlen(err_msg) ;
                sal_sprintf(&err_msg[index],
                        "Up to %u values allowed for this parameter (\'%s\').\r\n",
                        (unsigned short)max_num_repeated,
                        parameter_ptr->par_name) ;
                index = strlen(err_msg) ;
                ret = 11 ;
                goto limo_exit ;
              }
            }
            current_line->proc_state = RECEIVING_PARAM_VAL ;
          }
          else
          {
            /*
             * This is the beginning of a symbol.
             */
            /*
             * Check whether there is a matching parameter
             * in the list of values attached to the subject
             * currently active.
             */
            unsigned
              long
                current_mutex[NUM_ELEMENTS_MUTEX_CONTROL] ;
            unsigned
              int
                current_ordinal ;
            get_current_mutex_control(current_line,current_mutex) ;
            current_ordinal =
              get_current_ordinal(current_line,current_mutex,0) ;
            num_params_x =
              search_params(
                  &current_line->line_buf[
                        current_line->start_index_this_field],
                  current_line->num_loaded_this_field,
                  TRUE,
                  0,&match_index_x,&full_match_x,
                  &param_name_x,&common_len_x,
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
            if (num_params_x == 0)
            {
              char
                param_string[MAX_SIZE_OF_SYMBOL + 1] ;
              memcpy(param_string,&current_line->line_buf[
                        current_line->start_index_this_field],
                        current_line->num_loaded_this_field) ;
              param_string[current_line->num_loaded_this_field] = 0 ;
              sal_sprintf(err_msg,
                "\r\n\n"
                "Processing ended at:\r\n") ;
              index = strlen(err_msg) ;
              memcpy(&err_msg[index],current_line->line_buf,num_analyzed) ;
              err_msg[index + num_analyzed] = 0 ;
              index = strlen(err_msg) ;
              sal_sprintf(&err_msg[index],"\r\n") ;
              index = strlen(err_msg) ;
              sal_sprintf(&err_msg[index],
                "*** No parameter, starting"
                " with \'%s\', was found.\r\n\n"
                "List of parameters for this subject:\r\n",param_string) ;
              index = strlen(err_msg) ;
              search_params(
                  "",
                  0,
                  TRUE,
                  0,&match_index_x,&full_match_x,
                  &param_name_x,&common_len_x,
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
              sal_sprintf(&err_msg[index],found_names_list) ;
              ret = 12 ;
              goto limo_exit ;
            }
            else
            {
              /*
               * Enter if At least one parameter has been found.
               */
              /*
               * The state must now be RECEIVING_PARAM_STRING.
               * We do not need to advance to the next
               * entry in 'param_val_pairs' array since this started
               * as the second value for the same parameter.
               */
              PARAM_VAL_PAIR
                *param_val_pair ;
              current_line->proc_state = RECEIVING_PARAM_STRING ;
              param_val_pair =
                &current_line->param_val_pair[current_line->param_index] ;
              current_line->param_string_entered = FALSE ;
              /*
               * Redundant but safe.
               */
              param_val_pair->in_val_index = 0 ;
              param_val_pair->param_value.val_type = VAL_NONE ;
              param_val_pair->par_name = "" ;
            }
          }
        }
        /*
         * No analyzing has been carried out here, just determining
         * processing state. Get pointers back to the beginning
         * of this field.
         */
        num_analyzed -= (field_len + 1) ;
        break ;
      }
      default:
      {
        sal_sprintf(err_msg,
          "\r\n\n"
          "Processing ended at:\r\n") ;
        index = strlen(err_msg) ;
        memcpy(&err_msg[index],current_line->line_buf,num_analyzed) ;
        err_msg[index + num_analyzed] = 0 ;
        index = strlen(err_msg) ;
        sal_sprintf(&err_msg[index],"\r\n") ;
        index = strlen(err_msg) ;
        sal_sprintf(&err_msg[index],
          "*** Unexpected proc_state (%d)",
          current_line->proc_state) ;
        index = strlen(err_msg) ;
        gen_err(FALSE,TRUE,(int)0,0,err_msg,
                  proc_name,SVR_ERR,
                  LINE_MODE_HANDLING_ERR_01,FALSE,0,-1,FALSE) ;
        ret = 9 ;
        goto limo_exit ;
        break ;
      }
    }
  }
  while ((num_analyzed += (field_len + 1)) < num_loaded) ;
limo_exit:
  if (ret)
  {
    /*
     * The following code prints updated line (including completion
     * characters. This can be used in case 'insert' is added.
     * For now, do not use it.
     */
    if (0)
    {
    if (current_line->num_loaded != prev_num_loaded)
    {
      /*
       * Enter if number of characters have been added to the line.
       * Make sure to keep them and add the new characters.
       */
      if (!(current_line->num_loaded == (prev_num_loaded + 1)) ||
            !(current_line->line_buf[prev_num_loaded] == BLANK_SPACE))
      {
        /*
         * Enter if added characters were not just one space.
         */
        /*
         * We can assume here that 'current_line->num_loaded_this_field'
         * is smaller than 'MAX_CHARS_ON_LINE'.
         */
        diff = current_line->num_loaded + num_loaded - prev_num_loaded ;
        if (diff > MAX_CHARS_ON_LINE)
        {
          diff = MAX_CHARS_ON_LINE - current_line->num_loaded ;
        }
        else
        {
          diff = num_loaded - prev_num_loaded ;
        }
        memcpy(
          &current_line->line_buf[current_line->num_loaded],
          &line_buf[prev_num_loaded],
          diff
          ) ;
        current_line->num_loaded += diff ;
      }
    }
    }
  }
  /*
   * Set cursor at the end of the line. Actually, in case
   * of error, we could place it at the location to start
   * correcting. However, this requires adding "insert"
   * mode and sidewise arrow keys.
   */
  current_line->line_buf_index = current_line->num_loaded ;
  return (ret) ;
}
/*****************************************************
*NAME
*  process_line
*TYPE: PROC
*DATE: 03/FEB/2002
*FUNCTION:
*  Process input line by rules of CLI. Input line
*  is assumed to have been checked and found to be
*  of right format unless line_mode is set.
*CALLING SEQUENCE:
*  process_line(current_line,current_line_ptr)
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
*    Processing results.
*REMARKS:
*  This procedure is carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  process_line(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    err,
    ret ;
  unsigned
    int
      process_me ;
  char
    err_msg[MAX_CHARS_ON_LINE + 4*80] ;
  ret = FALSE ;
 
  process_me = FALSE ;
  err = FALSE ;
  err_msg[0] = 0 ;
  if (is_line_mode_set())
  {
    /*
     * If line mode is set then run all validity check on line, starting
     * at the very beginning of the line.
     */
    /*
     * Make sure surrounding quotation marks are taken away before
     * processing and then put back in place, for editing.
     */
    cut_text_params_quotation_marks(current_line) ;
    err = line_mode_handling(current_line,err_msg) ;
    paste_text_params_quotation_marks(current_line) ;
  }
  if ((current_line->subject_entered) && (!err))
  {
    if (current_line->line_subject->subject_handler)
    {
      process_me = TRUE ;
    }else
    {
      soc_sand_os_printf( 
        "Subject Name: %s\n\r"
        "Subject Id: %d\n\r"
        "Subject Detailed Help: %s\n\r"
        "Subject Short Help: %s\n\r", 
        current_line->line_subject->subj_name,
        current_line->line_subject->subject_id,
        current_line->line_subject->subj_detailed_help,
        current_line->line_subject->subj_short_help
        );
    }
  }
  if (process_me)
  {
    int
      err ;
    
    err = params_fully_loaded(current_line,err_msg) ;
    if (err)
    {
      inc_cli_err_counter() ;
      send_array_to_screen("\r\n",2) ;
      send_string_to_screen(err_msg,TRUE) ;
    }
    else
    {
#if LINK_PETRA_LIBRARIES
      uint32 dev_ids[SOC_SAND_MAX_DEVICE];
      uint32 nof_devices;
      uint32 dev_id_i;

      soc_petra_get_default_units(dev_ids, &nof_devices);
      
      for (dev_id_i = 0; dev_id_i < nof_devices; ++dev_id_i)
      {
        soc_petra_set_default_unit(dev_ids[dev_id_i]);
        /*
         * Make sure surrounding quotation marks are taken away before
         * processing and then put back in place, for editing.
         */
        cut_text_params_quotation_marks(current_line) ;
        
        if (nof_devices > 1)
        {
          soc_sand_os_printf( "\n\rDevice id: %d\n\r", dev_ids[dev_id_i]);
        }
#endif /* LINK_PETRA_LIBRARIES */
        err = current_line->
                  line_subject->subject_handler(current_line,current_line_ptr) ;
        paste_text_params_quotation_marks(current_line) ;
        if (err)
        {
          sal_sprintf(err_msg,
            "\r\n"
            "*** Error processing line with subject \'%s\' (error code=%d).\r\n",
            current_line->line_subject->subj_name,err) ;
          inc_cli_err_counter() ;
          send_string_to_screen(err_msg,TRUE) ;
        }
        else
        {
          if (is_echo_mode_on())
          {
            send_array_to_screen("\r\n",2) ;
          }
        }
#if LINK_PETRA_LIBRARIES
      }
#endif /* LINK_PETRA_LIBRARIES */
    }
  }
  else
  {
    
    if (err)
    {
      CURRENT_LINE
        *next_display_line ;
      if (strlen(err_msg))
      {
        send_string_to_screen(err_msg,TRUE) ;
      }
      inc_cli_err_counter() ;
      send_string_to_screen(
        "*** Error indication while trying to process line."
        "\r\n",
        TRUE) ;
      /*
       * Make sure the next line to display is the same as the
       * input line, with some additions (like parameter completion).
       */
      next_display_line = *current_line_ptr ;
      *next_display_line = *current_line ;
    }
    else
    {
      unsigned
        short
          ui ;
      char
        line_str[10] ;
      /*
       * Process input information on line pointed by 'current_line'.
       */
      send_string_to_screen(
        "\r\n\n"
        "   Here handle line (handling procedure not yet installed):"
        "\r\n\n"
        "   ",
        TRUE) ;
      soc_sand_os_printf( 
        "\n\r"
        "Subject Name: %s %s\n\r",
        current_line->line_subject_name,
        (current_line->subject_entered)?"Entered":"Not entered"
      );
      send_array_to_screen("\r\n\n",3) ;
      for (ui = 0 ; ui < 50 ; ui++)
      {
        sal_sprintf(line_str,"%03u",ui) ;
        d_taskDelay(1) ;
        send_string_to_screen(line_str,FALSE) ;
        send_char_to_screen('\r') ;
      }
    }
  }

  clear_abort_print_flag() ;
  clear_num_lines_printed() ;
  clear_ui_suspend_timer() ;
  return (ret) ;
}
