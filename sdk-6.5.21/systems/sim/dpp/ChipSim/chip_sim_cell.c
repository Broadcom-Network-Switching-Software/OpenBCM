/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include "chip_sim.h"
#include "chip_sim_cell.h"
#include "chip_sim_mem.h"
#include "chip_sim_log.h"

#include <string.h>

#include <soc/dpp/SAND/SAND_FM/sand_cell.h>

/*
 * accessing as apears in the spec. doc.
 * tx .. cell was writen from the driver to the registers
 *  waiting the chip to read the cell and clear the triger bit
 */
struct
{
  UINT32 access_trig_address ;
  UINT32 delay_milis ;
  uint8   has_delayed_task[CHIP_SIM_NOF_CHIPS] ;
  INT32  delayed_task_time_milis[CHIP_SIM_NOF_CHIPS] ;
} tx;

#define MAX_CELL_BYTE_SIZE SOC_SAND_DATA_CELL_BYTE_SIZE
#define MAX_NOF_CELL_BLOCKS 2
int nof_cell_blocks = 0;


typedef struct
{
  unsigned char cell[MAX_CELL_BYTE_SIZE];
  int    cell_byte_size;

  UINT32 chip_start_cell_offset;

  uint8   is_random;
  /* the time interval between to cells*/
  INT32  interval_milisec;
  /* time to next cell*/
  INT32  time_left_milisec;
  UINT32 count;

  UINT32 int_address ;
  UINT32 int_bit ;

} SIM_CELL;

SIM_CELL Rx_data[CHIP_SIM_NOF_CHIPS][MAX_NOF_CELL_BLOCKS];
SIM_CELL Rx_ctrl[CHIP_SIM_NOF_CHIPS][MAX_NOF_CELL_BLOCKS];

/*****************************************************
*NAME
*  SIM_CELL_init
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:29:52
*FUNCTION:
*  Initialize the SIM_CELL struct
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_OUT SIM_CELL* cell
*    (2) SOC_SAND_OUT int       byte_size
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
  SIM_CELL_init(
    SOC_SAND_OUT SIM_CELL* cell,
    SOC_SAND_OUT int       byte_size
    )
{
  cell->cell_byte_size = byte_size;
  cell->chip_start_cell_offset = INVALID_ADDRESS;
  cell->is_random = FALSE;
  cell->interval_milisec = 0;
  cell->time_left_milisec = 0;
  cell->count = 0;
  cell->int_address = INVALID_ADDRESS;
  cell->int_bit = 0;
  memset(cell->cell, 0x0, MAX_CELL_BYTE_SIZE);
}


/*****************************************************
*NAME
*  chip_sim_cell_init
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:58:08
*FUNCTION:
*  Initialize the cell module
*INPUT:
*  SOC_SAND_DIRECT:
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
  chip_sim_cell_init(
    void
    )
{
  int
    chip_i,
    block_i;
  tx.access_trig_address = INVALID_ADDRESS;
  tx.delay_milis = 0;
  for (chip_i=0; chip_i<CHIP_SIM_NOF_CHIPS; chip_i++)
  {
    tx.has_delayed_task[chip_i] = FALSE ;
    tx.delayed_task_time_milis[chip_i] = 0 ;
  }

  for (chip_i=0; chip_i<CHIP_SIM_NOF_CHIPS; chip_i++)
  {
    for (block_i=0; block_i<MAX_NOF_CELL_BLOCKS; block_i++)
    {
      SIM_CELL_init(&Rx_data[chip_i][block_i], SOC_SAND_DATA_CELL_BYTE_SIZE);
      SIM_CELL_init(&Rx_ctrl[chip_i][block_i], SOC_SAND_CTRL_CELL_BYTE_SIZE);
    }
  }

  return OK;
}


/*****************************************************
*NAME
*  chip_sim_cell_malloc
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:58:12
*FUNCTION:
*  Malloc the cell module
*INPUT:
*  SOC_SAND_DIRECT:
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
  chip_sim_cell_malloc()
{
  int i;
  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    tx.has_delayed_task[i] = FALSE;
    tx.delayed_task_time_milis[i] = 0;
  }
  return OK;
}

/*****************************************************
*NAME
*  chip_sim_cell_free
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:58:16
*FUNCTION:
*  Free the cell module
*INPUT:
*  SOC_SAND_DIRECT:
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
  chip_sim_cell_free()
{
  return OK;
}


/*****************************************************
*NAME
*  chip_sim_cell_init_chip_specifics
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:58:27
*FUNCTION:
*  Initialize with specifics the cell module
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    i_nof_cell_blocks
*    (2) SOC_SAND_IN UINT32 cell_access_trig_address
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
SOC_SAND_RET
  chip_sim_cell_init_chip_specifics(
    SOC_SAND_IN int    i_nof_cell_blocks,
    SOC_SAND_IN UINT32 cell_access_trig_address
)
{
  tx.access_trig_address = cell_access_trig_address ;

  nof_cell_blocks = i_nof_cell_blocks ;

  return OK;
}

/*****************************************************
*NAME
*  chip_sim_cell_rx_init_chip_specifics
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:59:23
*FUNCTION:
*  Initialize with specifics the cell module
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_start_data_cell_offset_a
*    (2) SOC_SAND_IN UINT32 chip_start_data_cell_offset_b
*    (3) SOC_SAND_IN UINT32 data_int_address_a
*    (4) SOC_SAND_IN UINT32 data_int_address_b
*    (5) SOC_SAND_IN int    data_int_bit_a
*    (6) SOC_SAND_IN int    data_int_bit_b
*    (7) SOC_SAND_IN UINT32 chip_start_ctrl_cell_offset_a
*    (8) SOC_SAND_IN UINT32 chip_start_ctrl_cell_offset_b
*    (9) SOC_SAND_IN UINT32 ctrl_int_address_a
*    (10) SOC_SAND_IN UINT32 ctrl_int_address_b
*    (11) SOC_SAND_IN int    ctrl_int_bit_a
*    (12) SOC_SAND_IN int    ctrl_int_bit_b
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
SOC_SAND_RET
  chip_sim_cell_rx_init_chip_specifics(
    SOC_SAND_IN UINT32 chip_start_data_cell_offset_a,
    SOC_SAND_IN UINT32 chip_start_data_cell_offset_b,
    SOC_SAND_IN UINT32 data_int_address_a,
    SOC_SAND_IN UINT32 data_int_address_b,
    SOC_SAND_IN int    data_int_bit_a,
    SOC_SAND_IN int    data_int_bit_b,
    SOC_SAND_IN UINT32 chip_start_ctrl_cell_offset_a,
    SOC_SAND_IN UINT32 chip_start_ctrl_cell_offset_b,
    SOC_SAND_IN UINT32 ctrl_int_address_a,
    SOC_SAND_IN UINT32 ctrl_int_address_b,
    SOC_SAND_IN int    ctrl_int_bit_a,
    SOC_SAND_IN int    ctrl_int_bit_b
)
{
  int
    chip_i;

  for (chip_i=0; chip_i<CHIP_SIM_NOF_CHIPS; chip_i++)
  {
    Rx_data[chip_i][0].chip_start_cell_offset = chip_start_data_cell_offset_a;
    Rx_data[chip_i][1].chip_start_cell_offset = chip_start_data_cell_offset_b;
    Rx_data[chip_i][0].int_address            = data_int_address_a;
    Rx_data[chip_i][1].int_address            = data_int_address_b;
    Rx_data[chip_i][0].int_bit                = data_int_bit_a;
    Rx_data[chip_i][1].int_bit                = data_int_bit_b;

    Rx_ctrl[chip_i][0].chip_start_cell_offset = chip_start_ctrl_cell_offset_a;
    Rx_ctrl[chip_i][1].chip_start_cell_offset = chip_start_ctrl_cell_offset_b;
    Rx_ctrl[chip_i][0].int_address            = ctrl_int_address_a;
    Rx_ctrl[chip_i][1].int_address            = ctrl_int_address_b;
    Rx_ctrl[chip_i][0].int_bit                = ctrl_int_bit_a;
    Rx_ctrl[chip_i][1].int_bit                = ctrl_int_bit_b;
  }
  return OK;
}


/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*  chip_sim_cell_is_trig_offset
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:59:31
*FUNCTION:
*  cell tx trigger address
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_offset
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
uint8
  chip_sim_cell_is_trig_offset(
    SOC_SAND_IN UINT32 chip_offset
    )
{
  uint8
    answer = FALSE;

  if (chip_offset == tx.access_trig_address)
  {
    answer = TRUE;
  }

  return answer ;
}


/*****************************************************
*NAME
*  chip_sim_cell_tx_get_delay
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:59:37
*FUNCTION:
*  delay of tx trigger
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
UINT32
  chip_sim_cell_tx_get_delay(
    void
    )
{
  return tx.delay_milis ;
}

/*****************************************************
*NAME
*  chip_sim_cell_tx_set_delay
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:59:41
*FUNCTION:
*  Sets the delay
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 delay_mili
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
  chip_sim_cell_tx_set_delay(
    SOC_SAND_IN UINT32 delay_mili
    )
{
  tx.delay_milis = delay_mili;
  return;
}

/* Under Interrupt Halting No OS Calls */
/*set the delay task .*/
/*****************************************************
*NAME
*  chip_sim_cell_set_delay_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:59:46
*FUNCTION:
*  Setting tx cell in the future (tx.delay_milis).
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN chip_ndx
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
  chip_sim_cell_set_delay_op(
    SOC_SAND_IN int chip_ndx
    )
{
  STATUS
    status = OK ;
  char
    msg[160];

  if (tx.has_delayed_task[chip_ndx] == TRUE)
  {
    /* an error by the driver */
    sal_sprintf(msg, "chip_sim_cell_set_delay_op(%d):\r\n"
                 "    trying to set trigger, while trigger is already SET!! \r\n"
                 "    DRIVER BUG ??? \r\n", chip_ndx);
    chip_sim_log_run(msg);
    status = ERROR ;
    GOTO_FUNC_EXIT_POINT ;
  }

  tx.has_delayed_task[chip_ndx] = TRUE ;
  tx.delayed_task_time_milis[chip_ndx] = tx.delay_milis ;

FUNC_EXIT_POINT:
  return status ;
}


/*****************************************************
*NAME
*     chip_sim_cell_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 23-Sep-02 11:38:49
*FUNCTION:
*  (1) check the chip if have been triggered to tx cell.
*  (2) If NO simulator error.
*  (3) check if need to do loop with the cell - serdes loop.
*  (4) set off the trigger bit.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int chip_ndx
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_cell_op(
    SOC_SAND_IN int chip_ndx
    )
{
  STATUS
    status = OK;
  UINT32
    access_triger = 0;

  /* 1 */
  if (chip_sim_mem_read(chip_ndx, tx.access_trig_address, &access_triger))
  {
    char msg[160];
    sal_sprintf(msg,"chip_sim_cell_op(): ERROR \r\n"
                "    chip_sim_mem_read(chip_ndx(%d), tx.access_trig_address(0x%X)) failed.\r\n",
                 chip_ndx, tx.access_trig_address);
    chip_sim_log_run(msg);
    status = ERROR ;
    GOTO_FUNC_EXIT_POINT ;
  }

  /* 2 */
  if (access_triger != 0x1)
  {
    /* simulator error*/
    status = ERROR ;
    chip_sim_log_run("chip_sim_cell_op(): ERROR \r\n"
                     "    'access_triger != 0x1' simulator error\r\n");
    GOTO_FUNC_EXIT_POINT ;
  }


  /* 3 */
  /*
 */



FUNC_EXIT_POINT:
/* 4 */
  if (chip_sim_mem_write(chip_ndx, tx.access_trig_address, 0x0))
  {
      chip_sim_log_run("chip_sim_cell_op()- error: \r\n"
 	              "chip_sim_mem_write() failed.\r\n");
      status = ERROR;
  }

  return status ;
}


/*****************************************************
*NAME
*   chip_sim_cell_tx
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:59:52
*FUNCTION:
* 1. decrement time
* 2. check if the time to work arrived.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 time_diff
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
  chip_sim_cell_tx(
    SOC_SAND_IN UINT32 time_diff
    )
{
  STATUS
    status = OK;
  int
    i;

  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    if (tx.has_delayed_task[i])
    {
      /* 1 */
      tx.delayed_task_time_milis[i] -= time_diff;
      /* 2 */
      if (tx.delayed_task_time_milis[i]<0)
      {
        tx.delayed_task_time_milis[i] = 0;
        tx.has_delayed_task[i]=FALSE;
        if( chip_sim_cell_op(i) )
        {
          status = ERROR ;
          chip_sim_log_run("chip_sim_cell_tx(): ERROR \r\n"
                           "    chip_sim_cell_op() - FAILED \r\n");
          GOTO_FUNC_EXIT_POINT ;
        }
      }
    }
  }

FUNC_EXIT_POINT:
  return status ;
}


/*****************************************************
*NAME
*   chip_sim_cell_rx_cell
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:36:28
*FUNCTION:
* 1. for every chip
* 2. for every block
* 3. check if there is a cell to receive
* 4. get if the int there is clear (case not clear --
*     its indicate that the driver did not
*     read the previous cell yet
*     (CHECK TIME INTERVAL that did not read?? ).
* 5. check time
* 6. copy cell
* 7. assert interrupt
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN    UINT32 time_diff
*    (2) SOC_SAND_INOUT SIM_CELL cells[][MAX_NOF_CELL_BLOCKS]
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
  chip_sim_cell_rx_cell(
    SOC_SAND_IN    UINT32 time_diff,
    SOC_SAND_INOUT SIM_CELL cells[][MAX_NOF_CELL_BLOCKS]
    )
{
  STATUS
    status = OK;
  int
    block_i,
    chip_i,
    cell_byte_i,
    long_i;
  UINT32
    int_reg;
  UINT32
    *long_p,
     long_val;

  /* 1 */
  for (chip_i=0; chip_i<CHIP_SIM_NOF_CHIPS; chip_i++)
  {
    /* 2 */
    for (block_i=0; block_i<nof_cell_blocks;  block_i++)
    {
      /* 3 */
      if (cells[chip_i][block_i].count > 0)
      {
        /* 4 */
        if (chip_sim_mem_read(chip_i, cells[chip_i][block_i].int_address, &int_reg))
        {
          char msg[160];
          sal_sprintf(msg,"chip_sim_cell_rx_cell- error: \r\n"
                       "chip_sim_mem_read(chip_i(%d), block_i(%d), cells[chip_i][block_i].int_address(0x%X)) failed.\r\n",
                      chip_i, block_i, cells[chip_i][block_i].int_address);
          chip_sim_log_run(msg);
          status = ERROR;
          GOTO_FUNC_EXIT_POINT;
        }
        if ( (int_reg>>cells[chip_i][block_i].int_bit) & 0x1)
        {
          GOTO_FUNC_EXIT_POINT;
        }
        /* 5 */
        cells[chip_i][block_i].time_left_milisec -= time_diff;
        if (cells[chip_i][block_i].time_left_milisec<0)
        {
          cells[chip_i][block_i].time_left_milisec = cells[chip_i][block_i].interval_milisec;
          cells[chip_i][block_i].count -- ;
          /* 6 */
          long_p = (UINT32*)cells[chip_i][block_i].cell;
          for (cell_byte_i=0, long_i=0;
                cell_byte_i<cells[chip_i][block_i].cell_byte_size;
                cell_byte_i+=sizeof(UINT32), long_i++)
          {
            long_val = *long_p;
            if (chip_sim_mem_write(chip_i,
                                   cells[chip_i][block_i].chip_start_cell_offset + sizeof(UINT32)*long_i
                                   ,long_val)
                )
            {
              char msg[160];
              sal_sprintf(msg,"chip_sim_cell_rx_cell- error: \r\n");
              chip_sim_log_run(msg);
              status = ERROR;
              GOTO_FUNC_EXIT_POINT;
            }
          }
          /* 7 */
          int_reg |= (0x1 << cells[chip_i][block_i].int_bit);
          if (chip_sim_mem_write(chip_i, cells[chip_i][block_i].int_address, int_reg))
          {
            char msg[160];
            sal_sprintf(msg,"chip_sim_cell_rx_cell- error: \r\n"
                         "chip_sim_mem_write(chip_i(%d), block_i(%d), cells[chip_i][block_i].int_address(0x%X)) failed.\r\n",
                        chip_i, block_i, cells[chip_i][block_i].int_address);
            chip_sim_log_run(msg);
            status = ERROR;
            GOTO_FUNC_EXIT_POINT;
          }
        }
      }
    }
  }

FUNC_EXIT_POINT:
  return status ;
}


/*****************************************************
*NAME
*   chip_sim_cell_rx
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 23-Sep-02 14:50:32
*FUNCTION:
* calls for chip_sim_cell_rx_cell() with data and control cells
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 time_diff
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
  chip_sim_cell_rx(
    SOC_SAND_IN UINT32 time_diff
    )
{
  STATUS
    status = OK;
  char
    msg[160];

  if (chip_sim_cell_rx_cell(time_diff, Rx_data))
  {
    sal_sprintf(msg,"chip_sim_cell_rx():\r\n"
                "    chip_sim_cell_rx_cell(time_diff, Rx_data) failed\r\n");
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  if (chip_sim_cell_rx_cell(time_diff, Rx_ctrl))
  {
    sal_sprintf(msg,"chip_sim_cell_rx():\r\n"
                "    chip_sim_cell_rx_cell(time_diff, Rx_ctrl) failed\r\n");
    chip_sim_log_run(msg);
    chip_sim_log_run(msg);
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

FUNC_EXIT_POINT:

  return status ;
}

/*****************************************************
*NAME
*  chip_sim_cell_add_rx_data
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:00:01
*FUNCTION:
*  New Cell Receive
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int           chip_ndx
*    (2) SOC_SAND_IN int           block_ndx
*    (3) SOC_SAND_IN unsigned char *data_buff
*    (4) SOC_SAND_IN int           data_buff_size
*    (5) SOC_SAND_IN uint8          is_random
*    (6) SOC_SAND_IN INT32         interval_mili
*    (7) SOC_SAND_IN UINT32        cells_count

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* assuming the indices are right.
*SEE ALSO:
 */
STATUS
  chip_sim_cell_add_rx_data(
    SOC_SAND_IN int           chip_ndx,
    SOC_SAND_IN int           block_ndx,
    SOC_SAND_IN unsigned char *data_buff,
    SOC_SAND_IN int           data_buff_size,
    SOC_SAND_IN uint8          is_random,
    SOC_SAND_IN INT32         interval_mili,
    SOC_SAND_IN UINT32        cells_count
    )
{
  memcpy(Rx_data[chip_ndx][block_ndx].cell, data_buff, (UINT32)data_buff_size);
  Rx_data[chip_ndx][block_ndx].is_random = is_random;
  Rx_data[chip_ndx][block_ndx].interval_milisec = interval_mili;
  Rx_data[chip_ndx][block_ndx].count = cells_count;
  Rx_data[chip_ndx][block_ndx].time_left_milisec = interval_mili;

  return OK;
}

