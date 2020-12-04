/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>







SOC_SAND_RET
  soc_sand_pack_control_cell(
    SOC_SAND_IN     SOC_SAND_CONTROL_CELL control_cell,
    SOC_SAND_INOUT  unsigned char     *packed_control_cell
  )
{
  
  return SOC_SAND_OK ;
}


SOC_SAND_RET
  soc_sand_unpack_control_cell(
    SOC_SAND_IN     unsigned char     *packed_control_cell,
    SOC_SAND_INOUT  SOC_SAND_CONTROL_CELL *control_cell
  )
{
  
  return SOC_SAND_OK ;
}



STATIC uint32
  soc_sand_pack_dest_routed_data_cell(
    SOC_SAND_IN   SOC_SAND_DATA_CELL  *data_cell,
    SOC_SAND_OUT  uint32        *packed_data_cell
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    iter = 0 ;
  uint32
    tmp_u32_data_cell_field = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PACK_DEST_ROUTED_DATA_CELL);

  
  packed_data_cell = 0 ;
  
  tmp_u32_data_cell_field = data_cell->data_cell.dest_routed.dest_or_mc_id ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          SOC_SAND_DATA_CELL_DEST_ID_START,
          SOC_SAND_DATA_CELL_DEST_ID_LENGTH,
          (packed_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  tmp_u32_data_cell_field = 0 ;
  tmp_u32_data_cell_field = data_cell->data_cell.dest_routed.origin_time ;

  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          SOC_SAND_DATA_CELL_ORIGIN_TIME_START,
          SOC_SAND_DATA_CELL_ORIGIN_TIME_LENGTH,
          (packed_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
 tmp_u32_data_cell_field = data_cell->data_cell.dest_routed.frag_number ;
 res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          SOC_SAND_DATA_CELL_FRAG_NUMBER_START,
          SOC_SAND_DATA_CELL_FRAG_NUMBER_LENGTH,
          (packed_data_cell)
       );
 SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
  for(iter=0; iter < SOC_SAND_DATA_CELL_PAYLOAD_IN_BYTES ; ++iter)
  {
    tmp_u32_data_cell_field = data_cell->data_cell.dest_routed.cell_data[iter] ;
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_data_cell_field),
            SOC_SAND_NOF_BITS_IN_CHAR * iter + SOC_SAND_DATA_CELL_PAYLOAD_START,
            SOC_SAND_NOF_BITS_IN_CHAR,
            (packed_data_cell)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pack_dest_routed_data_cell()",iter,0);
}



STATIC uint32
  soc_sand_unpack_dest_routed_data_cell(
    SOC_SAND_IN  uint32        *packed_data_cell,
    SOC_SAND_OUT SOC_SAND_DATA_CELL  *data_cell
  )
{
  uint32
    iter = 0 ;
  uint32
    res = SOC_SAND_OK ,
    tmp_u32_data_cell_field = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_UNPACK_DEST_ROUTED_DATA_CELL);

  
  tmp_u32_data_cell_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_data_cell,
          SOC_SAND_DATA_CELL_DEST_ID_START,
          SOC_SAND_DATA_CELL_DEST_ID_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
  data_cell->data_cell.dest_routed.dest_or_mc_id = (uint16) tmp_u32_data_cell_field;
  

  
  tmp_u32_data_cell_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_data_cell,
          SOC_SAND_DATA_CELL_ORIGIN_TIME_START,
          SOC_SAND_DATA_CELL_ORIGIN_TIME_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
  data_cell->data_cell.dest_routed.origin_time = (uint16) tmp_u32_data_cell_field;
  

  
  tmp_u32_data_cell_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_data_cell,
          SOC_SAND_DATA_CELL_FRAG_NUMBER_START,
          SOC_SAND_DATA_CELL_FRAG_NUMBER_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
  data_cell->data_cell.dest_routed.frag_number = (uint16) tmp_u32_data_cell_field;
  

  
  for(iter=0; iter < SOC_SAND_DATA_CELL_PAYLOAD_IN_BYTES ; ++iter)
  {
    tmp_u32_data_cell_field = 0 ;
    res = soc_sand_bitstream_get_any_field(
            (packed_data_cell) ,
            SOC_SAND_NOF_BITS_IN_CHAR * iter + SOC_SAND_DATA_CELL_PAYLOAD_START,
            SOC_SAND_NOF_BITS_IN_CHAR,
            &(tmp_u32_data_cell_field)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 271, exit);
    data_cell->data_cell.dest_routed.cell_data[iter] = (uint8) tmp_u32_data_cell_field ;
    
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_unpack_dest_routed_data_cell()",iter,0);
}



STATIC uint32
  soc_sand_pack_source_routed_data_cell(
    SOC_SAND_IN  SOC_SAND_DATA_CELL                   *data_cell,
    SOC_SAND_OUT uint32                         *packed_cpu_data_cell
  )
{
  uint32
    iter = 0;
  uint32
    res = SOC_SAND_OK ,
    tmp_u32_sr_extra_field ,
    tmp_u32_sr_extra_field2 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PACK_SOURCE_ROUTED_DATA_CELL);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.src_level ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_SRC_LEVEL_START ,
          SOC_SAND_DATA_CELL_SRC_LEVEL_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.dest_level ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_DEST_LEVEL_START ,
          SOC_SAND_DATA_CELL_DEST_LEVEL_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fip_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_FIP_SWITCH_START ,
          SOC_SAND_DATA_CELL_FIP_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe1_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_FE1_SWITCH_START ,
          SOC_SAND_DATA_CELL_FE1_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe2_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_FE2_SWITCH_START ,
          SOC_SAND_DATA_CELL_FE2_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 77, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe3_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_FE3_SWITCH_START ,
          SOC_SAND_DATA_CELL_FE3_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  
  
  tmp_u32_sr_extra_field = 0;
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fip_switch ;
  res = soc_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 215, exit);
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_POSITION ,
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

  
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe1_switch ;
  res = soc_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_POSITION ,
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

  
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe2_switch ;
  res = soc_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 235, exit);
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_POSITION ,
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

  
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe3_switch ;
  res = soc_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 245, exit);
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_POSITION ,
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.inband_cell ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_INBAND_CELL_POSITION ,
          SOC_SAND_SR_DATA_CELL_INBAND_CELL_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.ack ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_ACK_POSITION ,
          SOC_SAND_SR_DATA_CELL_ACK_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.indirect ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_INDIRECT_POSITION ,
          SOC_SAND_SR_DATA_CELL_INDIRECT_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

  
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.read_or_write ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_RW_POSITION ,
          SOC_SAND_SR_DATA_CELL_RW_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);

  
  if (data_cell->data_cell.source_routed.inband_cell)
  {
    for(iter = 0; iter < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
    {
      if (data_cell->data_cell.source_routed.add_wr_cell[iter] != 0)
      {
        
        res = soc_sand_bitstream_set_bit(
                packed_cpu_data_cell,
                SOC_SAND_SR_DATA_CELL_VALID_START +
                (SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * SOC_SAND_SR_DATA_CELL_WORD_LENGTH
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);

        
        tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.add_wr_cell[iter];
        res = soc_sand_bitstream_set_any_field(
                &(tmp_u32_sr_extra_field),
                SOC_SAND_SR_DATA_CELL_ADDRESS_START +
                (SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * SOC_SAND_SR_DATA_CELL_WORD_LENGTH ,
                SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
                packed_cpu_data_cell
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
        
        tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.data_wr_cell[iter];
        res = soc_sand_bitstream_set_any_field(
                &(tmp_u32_sr_extra_field),
                SOC_SAND_SR_DATA_CELL_RW_START +
                (SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * SOC_SAND_SR_DATA_CELL_WORD_LENGTH,
                SOC_SAND_SR_DATA_CELL_WRITE_LENGTH,
                packed_cpu_data_cell
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
      }
    }

    
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.cell_ident;
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_SAND_SR_DATA_CELL_CELL_IDENT_START ,
            SOC_SAND_SR_DATA_CELL_CELL_IDENT_LENGTH,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

    
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.cell_format;
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_SAND_SR_DATA_CELL_CELL_FORMAT_START ,
            SOC_SAND_SR_DATA_CELL_CELL_FORMAT_LENGTH,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
  }
  else 
  {
    
    for(iter = 0; iter < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
    {
      
      tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.add_wr_cell[iter];
      res = soc_sand_bitstream_set_any_field(
              &(tmp_u32_sr_extra_field),
              SOC_SAND_SR_DATA_CELL_NOT_INBAND_ADDRESS_START +
              (SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH ,
              SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              packed_cpu_data_cell
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);
      
      tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.data_wr_cell[iter];
      res = soc_sand_bitstream_set_any_field(
              &(tmp_u32_sr_extra_field),
              SOC_SAND_SR_DATA_CELL_NOT_INBAND_RW_START +
              (SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * SOC_SAND_SR_DATA_CELL_WRITE_LENGTH,
              SOC_SAND_SR_DATA_CELL_WRITE_LENGTH,
              packed_cpu_data_cell
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);
    }

    
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.cell_ident;
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_START ,
            SOC_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_LENGTH,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 420, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pack_source_routed_data_cell()",iter,0);

}



STATIC uint32
  soc_sand_unpack_source_routed_data_cell(
    SOC_SAND_IN  uint32         *packed_cpu_data_cell,
    SOC_SAND_IN  uint8        is_fe600,
    SOC_SAND_OUT SOC_SAND_DATA_CELL   *data_cell
  )
{
  uint32
    iter = 0 ;
  uint32
    res = SOC_SAND_OK ,
    tmp_field_for_data_cell_add_fields ,
    tmp_u32_sr_extra_field ,
    tmp_u32_sr_extra_field2 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_UNPACK_SOURCE_ROUTED_DATA_CELL);

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_DATA_CELL_SRC_LEVEL_START - is_fe600,
          SOC_SAND_DATA_CELL_SRC_LEVEL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  data_cell->data_cell.source_routed.src_level = (uint8) tmp_u32_sr_extra_field;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_DEST_LEVEL_START - is_fe600,
          SOC_SAND_DATA_CELL_DEST_LEVEL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  data_cell->data_cell.source_routed.dest_level = (uint8) tmp_u32_sr_extra_field;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_FIP_SWITCH_START - is_fe600,
          SOC_SAND_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  data_cell->data_cell.source_routed.fip_switch = (uint8) tmp_u32_sr_extra_field;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_FE1_SWITCH_START - is_fe600,
          SOC_SAND_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  data_cell->data_cell.source_routed.fe1_switch = (uint8) tmp_u32_sr_extra_field;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_FE2_SWITCH_START - is_fe600,
          SOC_SAND_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  data_cell->data_cell.source_routed.fe2_switch = (uint8) tmp_u32_sr_extra_field;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_FE3_SWITCH_START - is_fe600,
          SOC_SAND_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  data_cell->data_cell.source_routed.fe3_switch = (uint8) tmp_u32_sr_extra_field;
  

  

  
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_POSITION - is_fe600,
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fip_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 223, exit);
  data_cell->data_cell.source_routed.fip_switch = (uint8) tmp_u32_sr_extra_field2 ;
  

  
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_POSITION - is_fe600,
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe1_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 233, exit);
  data_cell->data_cell.source_routed.fe1_switch = (uint8) tmp_u32_sr_extra_field2 ;
  

  
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_POSITION - is_fe600,
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe2_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 243, exit);
  data_cell->data_cell.source_routed.fe2_switch = (uint8) tmp_u32_sr_extra_field2 ;
  

  
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_POSITION - is_fe600,
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe3_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 253, exit);
  data_cell->data_cell.source_routed.fe3_switch = (uint8) tmp_u32_sr_extra_field2 ;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_INBAND_CELL_POSITION - is_fe600,
          SOC_SAND_SR_DATA_CELL_INBAND_CELL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
  data_cell->data_cell.source_routed.inband_cell = (uint8) tmp_u32_sr_extra_field ;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_ACK_POSITION - is_fe600,
          SOC_SAND_SR_DATA_CELL_ACK_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
  data_cell->data_cell.source_routed.ack = (uint8) tmp_u32_sr_extra_field ;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_INDIRECT_POSITION - is_fe600,
          SOC_SAND_SR_DATA_CELL_INDIRECT_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
  data_cell->data_cell.source_routed.indirect = (uint8) tmp_u32_sr_extra_field ;
  

  
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_RW_POSITION - is_fe600,
          SOC_SAND_SR_DATA_CELL_RW_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
  data_cell->data_cell.source_routed.read_or_write = (uint8) tmp_u32_sr_extra_field ;
  

  
  if (data_cell->data_cell.source_routed.inband_cell)
  {
    for(iter = 0; iter < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
    {
      
      tmp_field_for_data_cell_add_fields = 0 ;
      res = soc_sand_bitstream_get_any_field(
              packed_cpu_data_cell,
              SOC_SAND_SR_DATA_CELL_ADDRESS_START +
              iter * SOC_SAND_SR_DATA_CELL_WORD_LENGTH - is_fe600,
              SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 420, exit);
      data_cell->data_cell.source_routed.add_wr_cell[SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter] =
        (uint16) tmp_field_for_data_cell_add_fields;
      

      
      tmp_field_for_data_cell_add_fields = 0 ;
      res = soc_sand_bitstream_get_any_field(
              packed_cpu_data_cell,
              SOC_SAND_SR_DATA_CELL_RW_START +
              iter * SOC_SAND_SR_DATA_CELL_WORD_LENGTH - is_fe600,
              SOC_SAND_SR_DATA_CELL_WRITE_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);
      data_cell->data_cell.source_routed.data_wr_cell[SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter] =
        tmp_field_for_data_cell_add_fields ;
    }

    
    tmp_field_for_data_cell_add_fields = 0 ;
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_SAND_SR_DATA_CELL_CELL_IDENT_START - is_fe600,
            SOC_SAND_SR_DATA_CELL_CELL_IDENT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 560, exit);
    data_cell->data_cell.source_routed.cell_ident = (uint16) tmp_field_for_data_cell_add_fields;
    

    
    tmp_field_for_data_cell_add_fields = 0 ;
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_SAND_SR_DATA_CELL_CELL_FORMAT_START - is_fe600,
            SOC_SAND_SR_DATA_CELL_CELL_FORMAT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 570, exit);
    data_cell->data_cell.source_routed.cell_format = (uint8) tmp_field_for_data_cell_add_fields;
    
  }
  else 
  {
    
    for(iter = 0; iter < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
    {
      
      tmp_field_for_data_cell_add_fields = 0 ;
      res = soc_sand_bitstream_get_any_field(
              packed_cpu_data_cell,
              SOC_SAND_SR_DATA_CELL_NOT_INBAND_ADDRESS_START +
              iter * SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH  - is_fe600,
              SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 600, exit);
      data_cell->data_cell.source_routed.add_wr_cell[SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter] = 
        (uint16) tmp_field_for_data_cell_add_fields;
      
      
      tmp_field_for_data_cell_add_fields = 0 ;
      res = soc_sand_bitstream_get_any_field(
              packed_cpu_data_cell,
              SOC_SAND_SR_DATA_CELL_NOT_INBAND_RW_START +
              iter * SOC_SAND_SR_DATA_CELL_WRITE_LENGTH  - is_fe600,
              SOC_SAND_SR_DATA_CELL_WRITE_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 610, exit);
      data_cell->data_cell.source_routed.data_wr_cell[SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter] = 
        tmp_field_for_data_cell_add_fields;
    }

    
    tmp_field_for_data_cell_add_fields = 0 ;
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_START - is_fe600,
            SOC_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 620, exit);
    data_cell->data_cell.source_routed.cell_ident = (uint16) tmp_field_for_data_cell_add_fields;
    
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_unpack_source_routed_data_cell()",iter,0);

}



uint32
  soc_sand_data_cell_to_buffer(
    SOC_SAND_IN  SOC_SAND_DATA_CELL   *data_cell,
    SOC_SAND_OUT uint32         *packed_data_cell
  )
{
  uint32
    i_reg = 0 ,
    length ; 
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp_u32_inversion = 0,
    tmp_u32_data_cell_field = 0,
    tmp_buffer_with_offset[SOC_SAND_DATA_CELL_UINT32_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_DATA_CELL_TO_BUFFER);


  res = soc_sand_os_memset(
          tmp_buffer_with_offset,
          0x0,
          SOC_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  
  tmp_u32_data_cell_field = data_cell->cell_type ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          SOC_SAND_DATA_CELL_CELL_TYPE_START ,
          SOC_SAND_DATA_CELL_CELL_TYPE_LENGTH,
          packed_data_cell
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  tmp_u32_data_cell_field = data_cell->source_id ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          SOC_SAND_DATA_CELL_SOURCE_ID_START,
          SOC_SAND_DATA_CELL_SOURCE_ID_LENGTH,
          packed_data_cell
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  switch(data_cell->cell_type)
  {
  case DATA_CELL_TYPE_SOURCE_ROUTED:
    length = SOC_SAND_SR_DATA_CELL_NOT_COMMON_LENGTH ;
    res = soc_sand_pack_source_routed_data_cell(
            data_cell,
            tmp_buffer_with_offset
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_sand_bitstream_set_any_field(
            tmp_buffer_with_offset,
            0,
            length,
            (packed_data_cell)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    for (i_reg = 0 ; i_reg < SOC_SAND_DATA_CELL_UINT32_SIZE / 2; ++i_reg)
    {
      tmp_u32_inversion = (packed_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ]);
      (packed_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ]) = (packed_data_cell[i_reg ]) ;
      (packed_data_cell[i_reg ]) = tmp_u32_inversion ;
    }
    break;

  case DATA_CELL_TYPE_DESTINATION_ROUTED:
    length = SOC_SAND_SR_DATA_CELL_NOT_COMMON_LENGTH ;
    res = soc_sand_pack_dest_routed_data_cell(
            data_cell,
            tmp_buffer_with_offset
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    res = soc_sand_bitstream_set_any_field(
            tmp_buffer_with_offset,
            0,
            length,
            (packed_data_cell)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    break;

  default:
    res = SOC_SAND_ERR;
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_data_cell_to_buffer()",0,0);

}


uint32
  soc_sand_buffer_to_data_cell(
    SOC_SAND_IN  uint32        *packed_data_cell,
    SOC_SAND_IN  uint8       is_fe600,
    SOC_SAND_OUT SOC_SAND_DATA_CELL  *data_cell
  )
{
  uint32
    i_reg;
  uint32
    res = SOC_SAND_OK ,
    tmp_u32_data_cell_field = 0 ,
    tmp_inverted_packed_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE] ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_BUFFER_TO_DATA_CELL);

  res = soc_sand_os_memset(
          tmp_inverted_packed_data_cell,
          0x0,
          SOC_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  for (i_reg = 0 ; i_reg < SOC_SAND_DATA_CELL_UINT32_SIZE ; ++i_reg)
  {
    tmp_inverted_packed_data_cell[i_reg] = packed_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ] ;
  }


  
  
  res = soc_sand_bitstream_get_any_field(
          (tmp_inverted_packed_data_cell),
          SOC_SAND_DATA_CELL_CELL_TYPE_START,
          SOC_SAND_DATA_CELL_CELL_TYPE_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  data_cell->cell_type = (uint8) tmp_u32_data_cell_field;
  

  
  tmp_u32_data_cell_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (tmp_inverted_packed_data_cell),
          SOC_SAND_DATA_CELL_SOURCE_ID_START - is_fe600,
          SOC_SAND_DATA_CELL_SOURCE_ID_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  data_cell->source_id = (uint16) tmp_u32_data_cell_field;
  

  
  switch(data_cell->cell_type)
  {
  case DATA_CELL_TYPE_SOURCE_ROUTED:
    res = soc_sand_unpack_source_routed_data_cell(
            tmp_inverted_packed_data_cell,
            is_fe600,
            data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;

  case DATA_CELL_TYPE_DESTINATION_ROUTED:
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    res = soc_sand_unpack_dest_routed_data_cell(
            tmp_inverted_packed_data_cell,
            data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    break;

  default:
    res = SOC_SAND_ERR;
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_buffer_to_data_cell()",0,0);
}





uint32
  soc_sand_actual_entity_value(
    SOC_SAND_IN SOC_SAND_DEVICE_ENTITY device_entity
  )
{
  uint32
    res ;
  
  res = 0 ;
  switch(device_entity)
  {
    case SOC_SAND_FE1_ENTITY:
    {
      res = SOC_SAND_ACTUAL_FE1_VALUE;
      break;
    }
    case SOC_SAND_FE2_ENTITY:
    {
      res = SOC_SAND_ACTUAL_FE2_VALUE;
      break;
    }
    case SOC_SAND_FE3_ENTITY:
    {
      res = SOC_SAND_ACTUAL_FE3_VALUE;
      break;
    }
    case SOC_SAND_FAP_ENTITY:
    {
      res = SOC_SAND_ACTUAL_FAP_VALUE;
      break;
    }
    case SOC_SAND_FOP_ENTITY:
    {
      res = SOC_SAND_ACTUAL_FOP_VALUE;
      break;
    }
    case SOC_SAND_FIP_ENTITY:
    {
      res = SOC_SAND_ACTUAL_FIP_VALUE;
      break;
    }
    case SOC_SAND_DONT_CARE_ENTITY:
    case SOC_SAND_FE13_ENTITY:
    {
      
      res = SOC_SAND_ACTUAL_BAD_VALUE;
      break;
    }
    default:
    {
      
      res = SOC_SAND_ACTUAL_OUT_OF_RANGE;
      break;
    }
  }
  return res ;
}




SOC_SAND_DEVICE_ENTITY
  soc_sand_real_entity_value(
    SOC_SAND_IN SOC_SAND_ENTITY_LEVEL_TYPE device_entity_3b
  )
{
  uint32
    res ;
  
  res = 0 ;
  switch(device_entity_3b)
  {
    case SOC_SAND_ACTUAL_FE1_VALUE:
    {
      res = SOC_SAND_FE1_ENTITY;
      break;
    }
    case SOC_SAND_ACTUAL_FE2_VALUE:
    {
      res = SOC_SAND_FE2_ENTITY;
      break;
    }
    case SOC_SAND_ACTUAL_FE3_VALUE:
    {
      res = SOC_SAND_FE3_ENTITY;
      break;
    }
    case SOC_SAND_ACTUAL_FAP_VALUE:
    {
      res = SOC_SAND_FAP_ENTITY;
      break;
    }
    case SOC_SAND_ACTUAL_FOP_VALUE:
    {
      res = SOC_SAND_FOP_ENTITY;
      break;
    }
    case SOC_SAND_ACTUAL_FIP_VALUE:
    {
      res = SOC_SAND_FIP_ENTITY;
      break;
    }
    default:
    {
      
      res = SOC_SAND_REAL_ENTITY_VALUE_OUT_OF_RANGE;
      break;
    }
  }
  return res ;
}
