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
*  print_history_fifo
*TYPE: PROC
*DATE: 31/JAN/2002
*FUNCTION:
*  Print the full contents of the history fifo
*  such that the last printed is the latest.
*CALLING SEQUENCE:
*  print_history_fifo(num_to_print)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int num_to_print -
*      Number of entries to print. If this number
*      is larger than whatever the fifo currently contains,
*      the maximal number possible is displayed.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Printed history.
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  print_history_fifo(
    unsigned int num_to_print
  )
{
  CURRENT_LINE
    **line_ptrs ;
  CURRENT_LINE
    *current_line ;
  unsigned
    int
      index,
      ui ;
  line_ptrs = History_fifo.history_array ;
  if (num_to_print > History_fifo.num_loaded)
  {
    num_to_print = History_fifo.num_loaded ;
  }
  index = History_fifo.index - num_to_print ;
  if ((int)index < 0)
  {
    index += History_fifo.elements_in_array ;
  }
  if (num_to_print)
  {
    send_array_to_screen("\r\n\n",3) ;
  }
  else
  {
    send_string_to_screen(
      "\r\n\n"
      "History buffer is empty!!"
      "\r\n",
      TRUE) ;
  }
  for (ui = 0 ; ui < num_to_print ; ui++)
  {
    char
      numeric[20] ;
    current_line = line_ptrs[index] ;
    sal_sprintf(numeric,"%02u) ",(num_to_print - ui)) ;
    send_string_to_screen(numeric,FALSE) ;
    send_array_to_screen(current_line->line_buf,current_line->num_loaded) ;
    send_array_to_screen("\r\n",2) ;
    index++ ;
    if (index >= History_fifo.elements_in_array)
    {
      index = 0 ;
    }
  }
  History_fifo.display_index = History_fifo.index ;
  History_fifo.display_direction = DIRECTION_NONE ;
  return ;
}
/*****************************************************
*NAME
*  put_history_fifo
*TYPE: PROC
*DATE: 31/JAN/2002
*FUNCTION:
*  Add an entry to the 'history' fifo within CLI.
*CALLING SEQUENCE:
*  put_history_fifo(current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE **current_line_ptr -
*      Pointer to pointer to current prompt line.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred
*      and output is not meaningful. This is a
*      fatal error.
*  SOC_SAND_INDIRECT:
*    Memory pointed by current_line_ptr is loaded
*    by a new pointer, pointing to an initialized
*    CURRENT_LINE block (clear line). The original
*    pointer to CURRENT_LINE is added to the fifo.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  put_history_fifo(
    CURRENT_LINE **current_line_ptr
  )
{
  int
    ret ;
  CURRENT_LINE
    **line_ptrs ;
  CURRENT_LINE
    *current_line ;
  unsigned
    int
      index,
      dump ;
  current_line = *current_line_ptr ;
  dump = FALSE ;
  ret = 0;

  /*
   * Do not store 'history' subjects in history fifo!
   * Do not store empty lines in history fifo!
   */
  if (current_line->num_loaded)
  {
    if (current_line->subject_entered)
    {
      if (current_line->line_subject->subject_id == SUBJECT_HISTORY_ID)
      {
        dump = TRUE ;
      }
    }
    else
    {
      /*
       * Enter if something has been typed but not a full subject.
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
        found_names_list[80*10] ;
      ret = TRUE ;
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
      if (num_subjs == 1)
      {
        if (Subjects_list[match_index].subject_id == SUBJECT_HISTORY_ID)
        {
          dump = TRUE ;
        }
      }
    }
  }
  else
  {
    /*
     * If the line is empty then dump.
     */
    dump = TRUE ;
  }
  if (dump)
  {
    init_current_line(History_fifo.trash_line_ptr) ;
    *current_line_ptr = History_fifo.trash_line_ptr ;
    History_fifo.trash_line_ptr = current_line ;
    goto puhi_exit ;
  }
  line_ptrs = History_fifo.history_array ;
  index = History_fifo.index ;
  /*
   * Oldest entry must be cleared.
   */
  init_current_line(line_ptrs[index]) ;
  *current_line_ptr = line_ptrs[index] ;
  line_ptrs[index] = current_line ;
  index++ ;
  if (index >= History_fifo.elements_in_array)
  {
    index = 0 ;
  }
  History_fifo.num_loaded++ ;
  if (History_fifo.num_loaded >
            History_fifo.elements_in_array)
  {
    History_fifo.num_loaded = History_fifo.elements_in_array ;
  }
  History_fifo.index =
      History_fifo.display_index = index ;
  History_fifo.display_direction = DIRECTION_NONE ;
puhi_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  swap_history_fifo
*TYPE: PROC
*DATE: 19/FEB/2002
*FUNCTION:
*  Swap an input prompt line buffer with a selected
*  buffer in history fifo. Load input buffer by all
*  data from original history fifo buffer.
*CALLING SEQUENCE:
*  swap_history_fifo(current_line_ptr,selected)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE **current_line_ptr -
*      Pointer to pointer to current prompt line.
*      On input, it points to the pointer to
*      the buffer which will replace the original
*      in history fifo.
*      On output, it points to the pointer to
*      the buffer which has been in the history
*      fifo.
*    unsigned int selected -
*      Index of the line to replace in history
*      fifo. '1' stands for last line displayed
*      on the screen (not including 'history'
*      commands), '2' stands for the line
*      before the last, etc.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred
*      and output is not meaningful. This is a
*      fatal error.
*  SOC_SAND_INDIRECT:
*    Memory pointed by current_line_ptr is loaded
*    by a new pointer, pointing to a CURRENT_LINE
*    block. The original pointer to CURRENT_LINE
*    is added to the fifo.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  swap_history_fifo(
    CURRENT_LINE **current_line_ptr,
    unsigned int selected
  )
{
  int
    ret ;
  CURRENT_LINE
    **line_ptrs ;
  CURRENT_LINE
    *current_line ;
  unsigned
    int
      index ;
  ret = FALSE ;
  if ((History_fifo.num_loaded > 0) && (selected > 0))
  {
    current_line = *current_line_ptr ;
    line_ptrs = History_fifo.history_array ;
    index = History_fifo.display_index ;
    if (selected > History_fifo.num_loaded)
    {
      selected = History_fifo.num_loaded ;
    }
    index -= selected ;
    if ((int)index < 0)
    {
      index += History_fifo.num_loaded ;
    }
    *current_line_ptr = line_ptrs[index] ;
    memcpy(current_line,line_ptrs[index],sizeof(*current_line)) ;
    line_ptrs[index] = current_line ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  show_prev_history_line
*TYPE: PROC
*DATE: 10/MAR/2002
*FUNCTION:
*  Bring to command line and print the line, on history
*  fifo, which is older than the last displayed (from
*  history fifo).
*CALLING SEQUENCE:
*  show_prev_history_line(current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE **current_line_ptr -
*      Pointer to pointer to current prompt line.
*      On input, it points to the pointer to
*      the buffer which will replace the original
*      in history fifo.
*      On output, it points to the pointer to
*      the buffer which has been in the history
*      fifo.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Memory pointed by current_line_ptr is loaded
*    by a new pointer, pointing to a CURRENT_LINE
*    block. The original pointer to CURRENT_LINE
*    is added to the fifo. (History_fifo is updated).
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  show_prev_history_line(
    CURRENT_LINE **current_line_ptr
  )
{
  CURRENT_LINE
    *current_line ;
  History_fifo.display_direction = DIRECTION_UP ;
  swap_history_fifo(current_line_ptr,1) ;
  dec_display_index() ;
  current_line = *current_line_ptr ;
  redisplay_line(current_line) ;
  return ;
}
/*****************************************************
*NAME
*  show_next_history_line
*TYPE: PROC
*DATE: 10/MAR/2002
*FUNCTION:
*  Bring to command line and print the line, on history
*  fifo, which is newer than the last displayed (from
*  history fifo).
*CALLING SEQUENCE:
*  show_next_history_line(current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE **current_line_ptr -
*      Pointer to pointer to current prompt line.
*      On input, it points to the pointer to
*      the buffer which will replace the original
*      in history fifo.
*      On output, it points to the pointer to
*      the buffer which has been in the history
*      fifo.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Memory pointed by current_line_ptr is loaded
*    by a new pointer, pointing to a CURRENT_LINE
*    block. The original pointer to CURRENT_LINE
*    is added to the fifo. (History_fifo is updated).
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  show_next_history_line(
    CURRENT_LINE **current_line_ptr
  )
{
  CURRENT_LINE
    *current_line ;
  History_fifo.display_direction = DIRECTION_DOWN ;
  inc_display_index() ;
  inc_display_index() ;
  swap_history_fifo(current_line_ptr,1) ;
  dec_display_index() ;
  current_line = *current_line_ptr ;
  redisplay_line(current_line) ;
  return ;
}
/*****************************************************
*NAME
*  dec_display_index
*TYPE: PROC
*DATE: 10/MAR/2002
*FUNCTION:
*  Decrement display index (within history fifo) so
*  the next display-request will refer to an older line.
*CALLING SEQUENCE:
*  dec_display_index()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  dec_display_index(
    void
  )
{
  History_fifo.display_index-- ;
  if ((int)History_fifo.display_index < 0)
  {
    History_fifo.display_index += History_fifo.num_loaded ;
  }
  return ;
}
/*****************************************************
*NAME
*  inc_display_index
*TYPE: PROC
*DATE: 10/MAR/2002
*FUNCTION:
*  Increment display index (within history fifo) so
*  the next display-request will refer to an older line.
*CALLING SEQUENCE:
*  inc_display_index()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  inc_display_index(
    void
  )
{
  History_fifo.display_index++ ;
  if ((int)History_fifo.display_index >= History_fifo.num_loaded)
  {
    History_fifo.display_index -= History_fifo.num_loaded ;
  }
  return ;
}
/*****************************************************
*NAME
*  init_history_fifo
*TYPE: PROC
*DATE: 31/JAN/2002
*FUNCTION:
*  Allocate memory for and itialize all variables
*  related to the 'history' option within CLI.
*CALLING SEQUENCE:
*  init_history_fifo()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    History_fifo.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Allocated initialized memory.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  init_history_fifo(
    void
  )
{
  int
    ret ;
  static
    CURRENT_LINE
      *current_line_ptr[NUM_HISTORY_ELEMENTS] ;
  static
    CURRENT_LINE
      line_array[NUM_HISTORY_ELEMENTS] ;
  static
    CURRENT_LINE
      trash_line,
      current_line ;
  unsigned
    int
      ui ;
  memset(&History_fifo,0,sizeof(History_fifo)) ;
  History_fifo.elements_in_array = NUM_HISTORY_ELEMENTS ;
  History_fifo.display_direction = DIRECTION_NONE ;
  History_fifo.
    history_array = current_line_ptr ;
  memset(current_line_ptr,0,sizeof(current_line_ptr)) ;
  for (ui = 0 ; ui < NUM_HISTORY_ELEMENTS ; ui++)
  {
    current_line_ptr[ui] = &line_array[ui] ;
  }
  History_fifo.trash_line_ptr = &trash_line ;
  /*
   * Allocate memory for current line.
   */
  Current_line_ptr = (CURRENT_LINE *)&current_line ;
  memset(Current_line_ptr,0,sizeof(*Current_line_ptr)) ;
  init_current_line(Current_line_ptr) ;
  ret = FALSE ;
  return (ret);
}
/*****************************************************
*NAME
*  subject_history
*TYPE: PROC
*DATE: 18/FEB/2002
*FUNCTION:
*  Process input line which has a 'History' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_history(current_line,current_line_ptr)
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
  subject_history(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    ret ;
  PARAM_VAL_PAIR
     *param_val_pair ;
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] ;
  ret = FALSE ;
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'history').
     */
    print_history_fifo(History_fifo.elements_in_array) ;
  }
  else
  {
    unsigned
      int
        match_index ;
    if (search_param_val_pairs(current_line,&match_index,PARAM_HIST_LEN_ID,1))
    {
      /*
       * The 'len' parameter could not be found.
       * Since only 'get' and 'len' are allowed, parameter 'get'
       * must be on the line.
       */
      unsigned
        int
          get_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_HIST_GET_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * The 'get' parameter could not be found.
         * Since only 'get' and 'len' are allowed, this is
         * an error!
         */
        send_string_to_screen(
            "\r\n"
            "*** Neither \'get\' nor \'len\' has been found on the command line.\r\n"
            "    Syntax error...\r\n",
            TRUE) ;
        ret = TRUE ;
      }
      get_value = param_val->value.ulong_value ;
      swap_history_fifo(current_line_ptr,get_value) ;
    }
    else
    {
      /*
       * 'Len' has been found. Note that, by syntax rules, 'get' can not
       * be on the line. Get 'len' value or use default if there is none.
       */
      unsigned
        int
          display_len ;
      display_len = History_fifo.elements_in_array ;
      param_val_pair = &current_line->param_val_pair[match_index] ;
      if (param_val_pair->param_value.val_type == VAL_NONE)
      {
        /*
         * No value found. Use default.
         */
        PARAM
          *parameter_ptr,
          *allowed_params ;
        allowed_params =
          current_line->line_subject->allowed_params ;
        parameter_ptr = &allowed_params[param_val_pair->param_match_index] ;
        if ((parameter_ptr->default_care & HAS_DEFAULT_MASK) == HAS_DEFAULT)
        {
          /*
           * There is a default value. Use it.
           * It can either be a number or the symbol "all".
           * The number is larger than zero.
           */
          PARAM_VAL
            *default_val ;
          default_val = parameter_ptr->default_val ;
          if ((default_val->val_type & VAL_SYMBOL) == VAL_SYMBOL)
          {
            /*
             * It can only be "all". If it is not, it is a syntax error which
             * should never occur.
             */
            if (default_val->numeric_equivalent != ALL_EQUIVALENT)
            {
              send_string_to_screen(
                  "\r\n"
                  "*** Default value for \'len\' is symbolic but not \'all\'.\r\n"
                  "    Syntax error...\r\n",
                  TRUE) ;
              ret = TRUE ;
            }
          }
          else if ((default_val->val_type & VAL_NUMERIC) == VAL_NUMERIC)
          {
            /*
             * It must be numeric.
             */
            display_len = (unsigned int)default_val->value.ulong_value ;
            if (display_len > History_fifo.elements_in_array)
            {
              display_len = History_fifo.elements_in_array ;
            }
          }
          else
          {
            /*
             * This is not an allowed syntax error. However,
             * display the full history buffer.
             */
            send_string_to_screen(
                  "\r\n"
                  "*** Unknown type of default value for \'len\'.\r\n"
                  "    Syntax error...\r\n",
                  TRUE) ;
            ret = TRUE ;
          }
        }
        else
        {
          /*
           * There is no default value. Syntax error which should never occur!
           */
          send_string_to_screen(
                  "\r\n"
                  "*** No default value for \'len\'.\r\n"
                  "    Syntax error...\r\n",
                  TRUE) ;
          ret = TRUE ;
        }
      }
      else if ((param_val_pair->param_value.val_type & VAL_SYMBOL) == VAL_SYMBOL)
      {
        /*
         * Value found. Load 'display_len'.
         */
        /*
         * It can only be "all". If it is not, it is a syntax error which
         * should never occur.
         */
        if (param_val_pair->param_value.numeric_equivalent != ALL_EQUIVALENT)
        {
          send_string_to_screen(
                  "\r\n"
                  "*** Unknown symbolic value for \'len\'.\r\n"
                  "    Syntax error...\r\n",
                  TRUE) ;
          ret = TRUE ;
        }
      }
      else if ((param_val_pair->param_value.val_type & VAL_NUMERIC) == VAL_NUMERIC)
      {
        /*
         * Value found. Load 'display_len'.
         */
        display_len = (unsigned int)param_val_pair->param_value.value.ulong_value ;
        if (display_len > History_fifo.elements_in_array)
        {
          display_len = History_fifo.elements_in_array ;
        }
      }
      else
      {
        /*
         * Unknown type of value. Syntax error which should never occur!
         */
        send_string_to_screen(
                  "\r\n"
                  "*** Unknown type of value for \'len\'.\r\n"
                  "    Syntax error...\r\n",
                  TRUE) ;
        ret = TRUE ;
      }
      /*
       * At this point. 'display_len' contains the number of entries to display.
       */
      print_history_fifo(display_len) ;
    }
  }
  return (ret) ;
}
