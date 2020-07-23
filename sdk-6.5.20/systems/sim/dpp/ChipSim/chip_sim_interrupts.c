/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include  "chip_sim_interrupts.h"
#include  "chip_sim_log.h"
#include  "chip_sim_mem.h"
#include  "chip_sim_task.h"

#include <string.h>
#include <stdlib.h>

/*
 * Counters object.
 */
CHIP_SIM_INTERRUPT* Sim_int[CHIP_SIM_NOF_CHIPS] ={0} ;
const CHIP_SIM_INTERRUPT* Sim_int_mirror = NULL;

UINT32  General_int_chip_offset = INVALID_ADDRESS;
UINT32  General_int_bit =0;
UINT32  General_mask_chip_offset = INVALID_ADDRESS;
UINT32  General_mask_bit =0;


/*****************************************************
*NAME
*   chip_sim_interrupt_init
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:20
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_init(
    void
    )
{
  int
    i;

  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    Sim_int[i] = NULL ;
  }
  Sim_int_mirror = NULL;

  return OK;
}

/*****************************************************
*NAME
*  chip_sim_interrupt_init_chip_specifics
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:25
*FUNCTION:
*  chip specifics difference between FEC/SOC_SAND_FAP10M....
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN CHIP_SIM_INTERRUPT* const m_sim_int
*         The profile of the interrupt
*    (2) SOC_SAND_IN UINT32  i_general_int_chip_offset
*    (3) SOC_SAND_IN int     i_general_int_bit
*    (4) SOC_SAND_IN UINT32  i_general_mask_chip_offset
*    (5) SOC_SAND_IN int     i_general_mask_bit
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_init_chip_specifics(
    SOC_SAND_IN CHIP_SIM_INTERRUPT* const m_sim_int,
    SOC_SAND_IN UINT32  i_general_int_chip_offset,
    SOC_SAND_IN int     i_general_int_bit,
    SOC_SAND_IN UINT32  i_general_mask_chip_offset,
    SOC_SAND_IN int     i_general_mask_bit
    )
{
  Sim_int_mirror = m_sim_int ;

  General_int_chip_offset  = i_general_int_chip_offset;
  General_int_bit          = i_general_int_bit;
  General_mask_chip_offset = i_general_mask_chip_offset;
  General_mask_bit         = i_general_mask_bit;
  return OK;
}

/*****************************************************
*NAME
*  chip_sim_interrupt_malloc
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:30
*FUNCTION:
*  Allocate memory for interrupt module.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_malloc(
    void
    )
{
  STATUS
    status = OK;
  int chip_i;
  int nof_mirror_int = 0 ;

  /* Count the number of interrupts...*/
  for (;Sim_int_mirror[nof_mirror_int].int_chip_offset!=INVALID_ADDRESS; nof_mirror_int++)
  {
  }
  nof_mirror_int++;


  for (chip_i=0; chip_i<CHIP_SIM_NOF_CHIPS; chip_i++)
  {
    int int_i;
    Sim_int[chip_i] = (CHIP_SIM_INTERRUPT*)CHIP_SIM_MALLOC(nof_mirror_int*
                                                           sizeof(CHIP_SIM_INTERRUPT), "Sim_int[chip_i]");
    if (NULL == Sim_int[chip_i])
    {
      chip_sim_log_run("chip_sim_interrupt_malloc(): malloc failed\r\n");
      status = ERROR ;
      chip_sim_interrupt_free();
      GOTO_FUNC_EXIT_POINT;
    }

    for (int_i=0; int_i<nof_mirror_int; int_i++)
    {
      Sim_int[chip_i][int_i] = Sim_int_mirror[int_i] ;
    }
  }

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_interrupt_free
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:35
*FUNCTION:
*  Free allocated memory for interrupt module.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  chip_sim_interrupt_free(
    void
    )
{
  int i;

  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    if (NULL != Sim_int[i])
    {
      CHIP_SIM_FREE(Sim_int[i]);
    }
    Sim_int[i] = NULL;
  }
}


/*****************************************************
*NAME
*  chip_sim_interrupt_set
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:40
*FUNCTION:
*  For a specific interrupt.
*   Write to its bit with the value of 1.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_ndx
*    (2) SOC_SAND_IN CHIP_SIM_INTERRUPT *int_p
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_set(
    SOC_SAND_IN int chip_ndx,
    SOC_SAND_IN CHIP_SIM_INTERRUPT *int_p
    )
{
  STATUS
    status = OK;
  char
    msg[160];

  if(chip_sim_mem_write_field(chip_ndx, int_p->int_chip_offset,
                              0x1, int_p->int_bit, (UINT32)(0x1<<(int_p->int_bit))))
  {
    sal_sprintf(msg,"chip_sim_interrupt_set():\r\n"
                "    chip_ndx(%d) int_p->int_chip_offset(0x%08X) ERROR\n\r",
            chip_ndx, int_p->int_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_interrupt_father_set
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:44
*FUNCTION:
*  For a specific interrupt.
*   Write to its father bit with the value of 1.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_ndx
*    (2) SOC_SAND_IN CHIP_SIM_INTERRUPT *int_p
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_father_set(
    SOC_SAND_IN int chip_ndx,
    SOC_SAND_IN CHIP_SIM_INTERRUPT *int_p
    )
{
  STATUS
    status = OK;
  char
    msg[160];

  if(chip_sim_mem_write_field(chip_ndx, int_p->father_chip_offset,
                              0x1, int_p->father_bit, (UINT32)0x1<<(int_p->father_bit)))
  {
    sal_sprintf(msg,"chip_sim_interrupt_father_set():\r\n"
                "    chip_ndx(%d) int_p->father_chip_offset(0x%08X) ERROR\n\r",
            chip_ndx, int_p->father_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*  chip_sim_interrupt_get_general_mask
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:49
*FUNCTION:
*  Get the general interrupt mask bit, is 0 or 1.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 chip_ndx
*    (2) SOC_SAND_OUT uint8 *is_set
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_get_general_mask(
    SOC_SAND_IN  int chip_ndx,
    SOC_SAND_OUT uint8 *is_set)
{
  STATUS
    status = OK;
  UINT32
    field=0;
  char
    msg[160];

  if (General_mask_chip_offset != INVALID_ADDRESS)
  {
    if(chip_sim_mem_read_field(chip_ndx, General_mask_chip_offset,
                               &field, General_mask_bit, (UINT32)0x1<<(General_mask_bit)))
    {
      sal_sprintf(msg,"chip_sim_interrupt_get_general_mask():\r\n"
                  "    chip_ndx(%d) General_mask_chip_offset(0x%08X) ERROR\n\r",
              chip_ndx, General_mask_chip_offset);
      chip_sim_log_run(msg);
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
  }
  else
  {
    /*
     * In case where the General_mask_chip_offset is INVALID_ADDRESS
     * it indicates it do not exist (like in the SOC_SAND_FAP10M).
     */
    field = 1;
  }

  *is_set = (field==0x1);

FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*  chip_sim_interrupt_is_general_set
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:52
*FUNCTION:
*  Get the general interrupt bit, is 0 or 1.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 chip_ndx
*    (2) SOC_SAND_OUT uint8 *is_set
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_is_general_int_set(
    SOC_SAND_IN  int chip_ndx,
    SOC_SAND_OUT uint8 *is_set
    )
{
  STATUS
    status = OK;
  UINT32
    field=0;
  char
    msg[160];

  if(chip_sim_mem_read_field(chip_ndx, General_int_chip_offset,
                             &field, General_int_bit, (UINT32)0x1<<(General_int_bit)))
  {
    sal_sprintf(msg,"chip_sim_interrupt_is_general_int_set():\r\n"
                "    chip_ndx(%d) General_int_chip_offset(0x%08X) ERROR\n\r",
            chip_ndx, General_int_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  *is_set = (field==0x1);

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_interrupt_general_int_set
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:52
*FUNCTION:
*  Set the general interrupt bit to 0 or 1.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 chip_ndx
*       The chip index we currently work on.
*    (2) SOC_SAND_OUT uint8   bit
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_general_int_set(
    SOC_SAND_IN  int chip_ndx,
    SOC_SAND_OUT uint8 bit)
{
  STATUS
    status = OK;
  char
    msg[160];

  /*
   * INVALID_ADDRESS this address do not exist (as in SOC_SAND_FAP10M).
   */
  if (General_int_chip_offset != INVALID_ADDRESS)
  {
    if(chip_sim_mem_write_field(chip_ndx, General_int_chip_offset,
                               (UINT32)(bit&0x1), General_int_bit, (UINT32)0x1<<(General_int_bit)))
    {
      sal_sprintf(msg,"chip_sim_interrupt_general_int_set():\r\n"
                  "    chip_ndx(%d) General_int_chip_offset(0x%08X) ERROR\n\r",
              chip_ndx, General_int_chip_offset);
      chip_sim_log_run(msg);
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
  }

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_interrupt_father_is_set
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:57
*FUNCTION:
*  Check if the father bit of a specific interrupt is set.
*   in case TRUE. check if the mask it is maksed.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 chip_ndx
*    (2) SOC_SAND_IN  CHIP_SIM_INTERRUPT *int_p
*    (3) SOC_SAND_OUT uint8 *is_set
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_father_is_set(
    SOC_SAND_IN  int chip_ndx,
    SOC_SAND_IN  CHIP_SIM_INTERRUPT *int_p,
    SOC_SAND_OUT uint8 *is_set)
{
  STATUS
    status = OK;
  UINT32
    int_bit,
    mask_bit;
  char
    msg[160];

  *is_set = FALSE;
  /* check if the father bit is on*/
  if(chip_sim_mem_read_field(chip_ndx, int_p->father_chip_offset,
                             &int_bit, int_p->father_bit, (UINT32)0x1<<(int_p->father_bit)))
  {
    sal_sprintf(msg,"chip_sim_interrupt_father_is_set():\r\n"
                "    chip_ndx(%d) int_p->father_chip_offset(0x%08X) ERROR\n\r",
                chip_ndx, int_p->father_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }


  /* if the father bit is on*/
  /* check the mask*/
  if(chip_sim_mem_read_field(chip_ndx, int_p->father_mask_chip_offset,
                             &mask_bit, int_p->father_mask_bit, (UINT32)0x1<<(int_p->father_mask_bit)))
  {
    sal_sprintf(msg,"chip_sim_interrupt_father_is_set():\r\n"
                "    chip_ndx(%d) int_p->father_mask_chip_offset(0x%08X) ERROR\n\r",
                chip_ndx, int_p->father_mask_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }
  *is_set = mask_bit&int_bit;

FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*  chip_sim_interrupt_is_set
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:01
*FUNCTION:
*  Check if the specific interrupt is set.
*   in case TRUE. check if is masked.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 chip_ndx
*    (2) SOC_SAND_IN  CHIP_SIM_INTERRUPT *int_p
*    (3) SOC_SAND_OUT uint8 *is_set
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_is_set(
    SOC_SAND_IN  int chip_ndx,
    SOC_SAND_IN  CHIP_SIM_INTERRUPT *int_p,
    SOC_SAND_OUT uint8 *is_set)
{
  STATUS
    status = OK;
  UINT32
    int_bit,
    mask_bit;
  char
    msg[160];

  *is_set = FALSE ;

  /* check the inner block interrupt*/
  if(chip_sim_mem_read_field(chip_ndx, int_p->int_chip_offset,
                             &int_bit, int_p->int_bit, (UINT32)0x1<<(int_p->int_bit)))
  {
    sal_sprintf(msg,"chip_sim_interrupt_is_set():\r\n"
                "    chip_ndx(%d) int_p->int_chip_offset(0x%08X) ERROR\n\r",
                chip_ndx, int_p->int_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  /*
   * INVALID_ADDRESS indicates that this interrupt do not have mask in the block,
   * only in the father level (like SOC_SAND_FAP10M).
   */
  if (int_p->mask_chip_offset != INVALID_ADDRESS)
  {
    /*
     * SOC_SAND_FE200 -- enter here.
     */
    /* if the inner block interrupt is on*/
    /* check the mask*/
    if(chip_sim_mem_read_field(chip_ndx, int_p->mask_chip_offset,
                               &mask_bit, int_p->mask_bit, (UINT32)0x1<<(int_p->mask_bit)))
    {
      sal_sprintf(msg,"chip_sim_interrupt_is_set():\r\n"
                  "    chip_ndx(%d) int_p->mask_chip_offset(0x%08X) ERROR\n\r",
                  chip_ndx, int_p->mask_chip_offset);
      chip_sim_log_run(msg);
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
  }
  else
  {
    /*
     * SOC_SAND_FAP10M -- enter here.
     */
    mask_bit = TRUE;
  }
  *is_set = (int_bit&mask_bit);

FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*   chip_sim_interrupt_in_blocks
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:47:47
*FUNCTION:
*  1. Run all interrupts of the specific chip.
*  2. if it need to generate more interrupts..
*  3. Check if its time has come...
*  4. Case time ahs come, prepare next interrupt time...
*     5. Set the interrupt
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 time_diff
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*     STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_in_blocks(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 time_diff
    )
{
  CHIP_SIM_INTERRUPT
    *int_p;
  STATUS
    status = OK;
  char
    msg[160];

  /* 1. */
  for (int_p=Sim_int[chip_ndx]; int_p->int_chip_offset!=INVALID_ADDRESS; int_p++ )
  {
    /* 2. */
    if (int_p->count>0)
    {
      /* 3. */
      int_p->time_left_milisec -= time_diff;
      if (int_p->time_left_milisec<0)
      {
        /* 4 */
        int_p->count-- ;
        int_p->time_left_milisec = int_p->interval_milisec;
        if (int_p->is_random)
        {
          int mod=10000;
          if (int_p->time_left_milisec>0)
          {
            mod = int_p->time_left_milisec;
          }
          int_p->time_left_milisec = sal_rand()%mod;
        }
        /* 5 */
        if (chip_sim_interrupt_set(chip_ndx, int_p))
        {
          sal_sprintf(msg,"chip_sim_interrupt_in_blocks():\r\n"
                      "    call to chip_sim_interrupt_set(%d, %p) failed\n\r", chip_ndx, (void *)int_p);
          chip_sim_log_run(msg);
          status = ERROR;
          GOTO_FUNC_EXIT_POINT;
        }
      }
    }
  }

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_interrupt_all_father_reset
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:47:54
*FUNCTION:
*  Reset for the chip -- all the father interrupts.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_all_father_reset(
    SOC_SAND_IN int chip_ndx
    )
{
  STATUS
    status = OK;
  CHIP_SIM_INTERRUPT
    *int_p;
  char
    msg[160];
  UINT32
    bit = 0;

  for (int_p=Sim_int[chip_ndx]; int_p->int_chip_offset!=INVALID_ADDRESS; int_p++ )
  {
    if(chip_sim_mem_write_field(chip_ndx, int_p->father_chip_offset,
                                bit, int_p->father_bit, (UINT32)0x1<<(int_p->father_bit)))
    {
      sal_sprintf(msg,"chip_sim_interrupt_all_father_reset(): \r\n"
                  "    chip_ndx(%d) int_p->father_chip_offset(0x%08X) ERROR\n\r",
                  chip_ndx, int_p->father_chip_offset);
      chip_sim_log_run(msg);
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
  }

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_interrupt_in_eci
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:47:54
*FUNCTION:
*
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_in_eci(
    SOC_SAND_IN int    chip_ndx
    )
{
  const
    CHIP_SIM_INTERRUPT
      *int_p;
  STATUS
    status = OK;
  uint8
    is_set;
  char
    msg[160];

  if (chip_sim_interrupt_all_father_reset(chip_ndx))
  {
    sal_sprintf(msg,"chip_sim_interrupt_in_eci():\r\n"
                "    call to chip_sim_interrupt_all_father_reset(%d) failed\n\r", chip_ndx);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  /* eci interrupt view*/
  for (int_p=Sim_int[chip_ndx]; int_p->int_chip_offset!=INVALID_ADDRESS; int_p++ )
  {
    /* check is this interrupt is set and not masked */
    if (chip_sim_interrupt_is_set(chip_ndx, int_p, &is_set))
    {
      sal_sprintf(msg,"chip_sim_interrupt_in_eci():\r\n"
                  "    call to chip_sim_interrupt_is_set(%d, %p, &is_set) failed\n\r", chip_ndx, (void *)int_p);
      chip_sim_log_run(msg);
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
    if (is_set)
    {
      if (chip_sim_interrupt_father_set(chip_ndx, int_p))
      {
        sal_sprintf(msg,"chip_sim_interrupt_in_eci():\r\n"
                    "    call to chip_sim_interrupt_father_set(%d, %p) failed\n\r", chip_ndx, (void *)int_p);
        chip_sim_log_run(msg);
        status = ERROR;
        GOTO_FUNC_EXIT_POINT;
      }
    }
  }

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_interrupt_any_father_is_set
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:48:00
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_OUT uint8   *any_father_chip_i
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_any_father_is_set(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_OUT uint8   *any_father_chip_i
    )
{
  STATUS
    status = OK;
  const
    CHIP_SIM_INTERRUPT
      *int_p;
  uint8
    is_set;
  char
    msg[160];

  *any_father_chip_i = FALSE;
  /* search if one of the father is on and enabled. if yes general interrupt.*/
  for (int_p=Sim_int[chip_ndx]; int_p->int_chip_offset!=INVALID_ADDRESS; int_p++ )
  {
    /* check is this interrupt is set and not masked */
    if (chip_sim_interrupt_father_is_set(chip_ndx, int_p, &is_set))
    {
      sal_sprintf(msg,"chip_sim_interrupt_any_father_is_set():\r\n"
                  "    call to chip_sim_interrupt_father_is_set(%d, %p, &is_set) failed\n\r",
                  chip_ndx, (void *)int_p);
      chip_sim_log_run(msg);
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
    if (is_set)
    {
      *any_father_chip_i=TRUE;
      GOTO_FUNC_EXIT_POINT;
    }
  }


FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_interrupt_run
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:17
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 time_diff
*    (2) SOC_SAND_OUT uint8   general_int
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_run(
    SOC_SAND_IN  UINT32 time_diff,
    SOC_SAND_OUT uint8   *general_int
    )
{
  STATUS
    status = OK;
  int
    chip_i;
  uint8
    is_set;
  uint8
    any_father_chip_i = FALSE;

  *general_int=FALSE;

  for (chip_i=0; chip_i<CHIP_SIM_NOF_CHIPS; chip_i++)
  {
    if (chip_sim_interrupt_in_blocks(chip_i, time_diff))
    {
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
    if (chip_sim_interrupt_in_eci(chip_i))
    {
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }

    /* search if one of the father is on and enabled. if yes general interrupt.*/
    if (chip_sim_interrupt_any_father_is_set(chip_i, &any_father_chip_i))
    {
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }

    if (chip_sim_interrupt_get_general_mask(chip_i, &is_set))
    {
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }

    if (!is_set)
    {
      /* the general interrupt is masked -- nothing to do*/
      any_father_chip_i = FALSE;
    }

    /*Sets or Reset the general interupt in chip_i*/
    if (chip_sim_interrupt_general_int_set(chip_i, any_father_chip_i))
    {
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
    *general_int |= any_father_chip_i;
  }


FUNC_EXIT_POINT:
  if (status)
  {
    chip_sim_log_run("chip_sim_interrupt_run() - failed\n\r");
  }

  return status;
}

/*****************************************************
*NAME
*  chip_sim_interrupt_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:02:24
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_ndx
*    (2) CHIP_SIM_INTERRUPT *int_p

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_op(
    SOC_SAND_IN int chip_ndx,
    CHIP_SIM_INTERRUPT *int_p
    )
{
  STATUS
    status = OK;
  UINT32
    reg_val;
  char
    msg[160];

  if( chip_sim_mem_read(chip_ndx, int_p->mask_chip_offset, &reg_val) )
  {
    sal_sprintf(msg,"chip_sim_interrupt_op(): \r\n"
                "    chip_ndx(%d) int_p->mask_chip_offset(0x%08X) ERROR\n\r", chip_ndx, int_p->mask_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  if ( (reg_val>>(int_p->mask_bit))&0x1)
  {
    /*the mask is on*/
    GOTO_FUNC_EXIT_POINT;
  }

  if( chip_sim_mem_read(chip_ndx, int_p->int_chip_offset, &reg_val) )
  {
    sal_sprintf(msg,"chip_sim_interrupt_op(): \r\n"
                "    chip_ndx(%d) int_p->int_chip_offset(0x%08X) ERROR\n\r", chip_ndx, int_p->int_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  if ( (reg_val>>(int_p->int_bit))&0x1)
  {
    /*the interrupt is alreay on*/
    GOTO_FUNC_EXIT_POINT;
  }

  reg_val |= 0x1<<(int_p->int_bit) ;

  if( chip_sim_mem_write(chip_ndx, int_p->int_chip_offset, reg_val) )
  {
    sal_sprintf(msg,"chip_sim_interrupt_op(): \r\n"
                "    chip_ndx(%d) int_p->int_chip_offset(0x%08X) ERROR\n\r", chip_ndx, int_p->int_chip_offset);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*  chip_sim_interrupt_mask_all
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:48:11
*FUNCTION:
* Mask all interrupts with the bit ....
*  1. The block masks and there father mask
*  2. The general mask
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN uint8 bit
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_mask_all(
    SOC_SAND_IN UINT32 bit
    )
{
  STATUS
    status = OK;
  int
    chip_i;
  CHIP_SIM_INTERRUPT
    *int_p;
  char
    msg[160];

  for (chip_i=0; chip_i<CHIP_SIM_NOF_CHIPS; chip_i++)
  {
    /* 1 */
    for (int_p=Sim_int[chip_i]; int_p->int_chip_offset!=INVALID_ADDRESS; int_p++ )
    {
      /*
       * INVALID_ADDRESS in mask address indicates that we do not have this mask (SOC_SAND_FAP10M).
       */
      if (int_p->mask_chip_offset != INVALID_ADDRESS)
      {
        if(chip_sim_mem_write_field(chip_i, int_p->mask_chip_offset,
                                    bit, int_p->mask_bit, (UINT32)0x1<<(int_p->mask_bit)))
        {
          sal_sprintf(msg,"chip_sim_interrupt_mask_all(): \r\n"
                      "    chip_ndx(%d) int_p->mask_chip_offset(0x%08X) ERROR\n\r",
                      chip_i, int_p->mask_chip_offset);
          chip_sim_log_run(msg);
          status = ERROR;
          GOTO_FUNC_EXIT_POINT;
        }
      }
      if(chip_sim_mem_write_field(chip_i, int_p->father_mask_chip_offset,
                                  bit, int_p->father_mask_bit, (UINT32)0x1<<(int_p->father_mask_bit)))
      {
        sal_sprintf(msg,"chip_sim_interrupt_mask_all(): \r\n"
                    "    chip_ndx(%d) int_p->father_mask_chip_offset(0x%08X) ERROR\n\r",
                    chip_i, int_p->father_mask_chip_offset);
        chip_sim_log_run(msg);
        status = ERROR;
        GOTO_FUNC_EXIT_POINT;
      }
    }

    /* 2 */
    /*
     * INVALID_ADDRESS in mask address indicates that we do not have this mask (SOC_SAND_FAP10M).
     */
    if (General_mask_chip_offset != INVALID_ADDRESS)
    {
      if(chip_sim_mem_write_field(chip_i, General_mask_chip_offset,
                                  bit, General_mask_bit, (UINT32)0x1<<General_mask_bit))
      {
        sal_sprintf(msg,"chip_sim_interrupt_mask_all(): \r\n"
                    "    chip_ndx(%d) General_mask_chip_offset(0x%08X) ERROR\n\r",
                    chip_i, General_mask_chip_offset);
        chip_sim_log_run(msg);
        status = ERROR;
        GOTO_FUNC_EXIT_POINT;
      }
    }
  }

#ifdef __VXWORKS__
  /*
   * No interrupts on MS-windowz
   */
  if (intContext() &&
      (bit==0)
     )
  {
    chip_sim_interrupt_cpu_deassert();
  }
#endif

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_interrupt_set_interrupt_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:48:19
*FUNCTION:
*  Set a specific interrupt - it profile.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 int_chip_offset
*    (3) SOC_SAND_IN int    int_bit
*    (4) SOC_SAND_IN uint8   is_random
*    (5) SOC_SAND_IN INT32  interval_milisec
*    (6) SOC_SAND_IN UINT32 count
*    (7) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_interrupt_set_interrupt_op(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 int_chip_offset,
    SOC_SAND_IN UINT32 int_bit,
    SOC_SAND_IN uint8   is_random,
    SOC_SAND_IN INT32  interval_milisec,
    SOC_SAND_IN UINT32 count,
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = ERROR;
  CHIP_SIM_INTERRUPT
    *int_p;
  if (chip_ndx<0 || chip_ndx>=CHIP_SIM_NOF_CHIPS)
  {
    sal_strncpy(msg, "invalid chip_index in chip_sim_interrupt_set_interrupt_op()\n\r", 70);
    GOTO_FUNC_EXIT_POINT;
  }
  for (int_p = Sim_int[chip_ndx]; int_p->int_chip_offset!=INVALID_ADDRESS; int_p++)
  {
    if (((int_p->int_chip_offset) == int_chip_offset) &&
        ((int_p->int_bit) == int_bit)
       )
    {
      int_p->is_random = is_random;
      int_p->interval_milisec = interval_milisec;
      int_p->time_left_milisec = interval_milisec;
      int_p->count = count;
      status = OK;
      GOTO_FUNC_EXIT_POINT;
    }
  }
  sal_strncpy(msg, "chip_sim_interrupt_set_interrupt_op -- did not find the asked interrupt", 75);

FUNC_EXIT_POINT:
  return status;
}

/*
 * This functions are defined in usrAppLoader.c
 */
extern void assert_irq_6(void);
extern void deassert_irq_6(void);
extern int attach_irq(SOC_SAND_FUNC_PTR      irq_procedur,
                      unsigned int irq_num,
                      int          private_param,
                      char         *err_msg
                      );

extern int detach_irq(unsigned int irq_num,
                      char         *err_msg
                      );

int int_count=0;
void
  chip_sim_interrupt_cpu_handler(
    int param
    )
{
  int_count ++;
#ifdef __VXWORKS__
  logMsg("in chip_sim_interrupt_cpu_handler()...  int_count(%d) \r\n"
         "    Write your dam' handler already ... :-)", int_count, 0,0,0,0,0);
#else
  soc_sand_os_printf("in chip_sim_interrupt_cpu_handler()...  int_count(%d) \r\n"
         "    Write your dam' handler already ... :-)", int_count);
#endif
  chip_sim_interrupt_mask_all(0);
}

void
  chip_sim_interrupt_cpu_assert(
    void
    )
{
  assert_irq_6() ;
}

void
  chip_sim_interrupt_cpu_deassert(
    void
    )
{
  deassert_irq_6() ;
}

STATUS
  chip_sim_interrupt_cpu_hock(
    SOC_SAND_OUT char msg[]
    )
{
  STATUS
    status = OK;


  if (attach_irq((SOC_SAND_FUNC_PTR)chip_sim_interrupt_cpu_handler, 6, 1, msg))
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

FUNC_EXIT_POINT:
  return status ;
}

STATUS
  chip_sim_interrupt_cpu_unhock(
    SOC_SAND_OUT char msg[]
    )
{
  STATUS
    status = OK;


  if (detach_irq(6,msg))
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

FUNC_EXIT_POINT:
  return status ;
}



