
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS






#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap_access.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_eg_encap_access.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/alloc_mngr.h>






#define QAX_PP_OUT_RIF_TBL_NUM_OF_ENTRIES_PER_ROW(unit)   (SOC_IS_QUX(unit) ? 4 : 8)

#define QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS              (8)

#define QAX_PP_OUT_RIF_TBL_ENTRY_SIZE                     (3)


#define QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, _offset) \
  (_offset / QAX_PP_OUT_RIF_TBL_NUM_OF_ENTRIES_PER_ROW(unit))


#define QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_ROW_OFFSET(unit, _offset) \
  (_offset - (_offset / QAX_PP_OUT_RIF_TBL_NUM_OF_ENTRIES_PER_ROW(unit)) * QAX_PP_OUT_RIF_TBL_NUM_OF_ENTRIES_PER_ROW(unit))

#define QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_ARRAY_INDEX(_offset) \
  ((_offset * QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS) / SOC_SAND_NOF_BITS_IN_UINT32)

#define QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_BIT_OFFSET(_offset) \
  ((_offset * QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS) % SOC_SAND_NOF_BITS_IN_UINT32);
















soc_error_t
  soc_qax_pp_eg_rif_profile_set(
    int                 unit,
    uint32              rif_profile_index,
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT  *tbl_data
  )
{
    uint32 rif_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_mem_field32_set(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, DROPf, tbl_data->drop);
    soc_mem_field32_set(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, OAM_LIF_SETf, tbl_data->oam_lif_set);
    soc_mem_field32_set(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, OUTLIF_PROFILEf, tbl_data->outlif_profile);

    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EDB_RIF_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, rif_profile_index, &rif_profile));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_qax_pp_eg_rif_profile_get(
    int                 unit,
    uint32              rif_profile_index,
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT  *tbl_data
  )
{
    uint32 rif_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_SAND_IF_ERR_EXIT(READ_EDB_RIF_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, rif_profile_index, &rif_profile));

    tbl_data->drop = soc_mem_field32_get(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, DROPf);
    tbl_data->oam_lif_set = soc_mem_field32_get(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, OAM_LIF_SETf);
    tbl_data->outlif_profile = soc_mem_field32_get(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, OUTLIF_PROFILEf);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_qax_pp_eg_rif_profile_delete(
    int                 unit,
    uint32              rif_profile_index
  )
{
    uint32 rif_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EDB_RIF_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, rif_profile_index, &rif_profile));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
qax_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
    int                 unit,
    uint32              outlif,
    ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data)
{
    uint32
        entry_offset,
        internal_row_offset,
        data[QAX_PP_OUT_RIF_TBL_ENTRY_SIZE],
        entry = 0,
        rif_profile_index,
        data_index,
        data_inner_offset;
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT out_rif_profile;

    SOCDNX_INIT_FUNC_DEFS;

    if (outlif >= SOC_DPP_DEFS_GET(unit, max_nof_rifs)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT,
            (_BSL_SOCDNX_MSG("OutRif: %d, is out of range: %d.\n"),
                             outlif, SOC_DPP_DEFS_GET(unit, max_nof_rifs) - 1));
    }

    sal_memset(data, 0, sizeof(uint32) * QAX_PP_OUT_RIF_TBL_ENTRY_SIZE);

    entry_offset = QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, outlif);

    SOCDNX_SAND_IF_ERR_EXIT(READ_EDB_OUTRIF_TABLEm(unit, MEM_BLOCK_ANY, entry_offset, data));

    internal_row_offset = QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_ROW_OFFSET(unit, outlif);

    
    data_index = QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_ARRAY_INDEX(internal_row_offset);
    data_inner_offset = QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_BIT_OFFSET(internal_row_offset);

    
    entry = (data[data_index] >> data_inner_offset) & ((1 << QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS) - 1);

    
    tbl_data->remark_profile = entry & ((1 << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) - 1);

    
    rif_profile_index = (entry >> (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) & ((1 << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) - 1);

    SOCDNX_SAND_IF_ERR_EXIT(soc_qax_pp_eg_rif_profile_get(unit, rif_profile_index, &out_rif_profile));

    tbl_data->outrif_profile_index = rif_profile_index;
    tbl_data->drop = out_rif_profile.drop;
    tbl_data->oam_lif_set = out_rif_profile.oam_lif_set;
    tbl_data->outlif_profile = out_rif_profile.outlif_profile;
    tbl_data->next_vsi_lsb = outlif; 

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
qax_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
    int                 unit,
    uint32              outlif,
    ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data)
{
    uint32
        bank_id=0,
        entry_offset,
        internal_row_offset,
        data[QAX_PP_OUT_RIF_TBL_ENTRY_SIZE],
        entry = 0,
        data_index,
        data_inner_offset;
        int new_profile_index,
        is_last = 0,
        is_first = 0;
        QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT new_profile;

    SOCDNX_INIT_FUNC_DEFS;

    if (outlif >= SOC_DPP_DEFS_GET(unit, max_nof_rifs)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT,
            (_BSL_SOCDNX_MSG("OutRif: %d, is out of range: %d.\n"),
                             outlif, SOC_DPP_DEFS_GET(unit, max_nof_rifs) - 1));
    }

    sal_memset(data, 0, sizeof(uint32)*QAX_PP_OUT_RIF_TBL_ENTRY_SIZE);

    entry_offset = QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, outlif);

    SOCDNX_SAND_IF_ERR_EXIT(READ_EDB_OUTRIF_TABLEm(unit, MEM_BLOCK_ANY, entry_offset, data));

    internal_row_offset = QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_ROW_OFFSET(unit, outlif);

    
    data_index = QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_ARRAY_INDEX(internal_row_offset);
    data_inner_offset = QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_BIT_OFFSET(internal_row_offset);
    
    entry = SOC_SAND_U32_MAX - (((1 << QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS) - 1) << data_inner_offset);
    data[data_index] &= entry;

    
    entry = tbl_data->remark_profile & ((1 << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) - 1);

    new_profile.drop = tbl_data->drop;
    new_profile.oam_lif_set = tbl_data->oam_lif_set;
    new_profile.outlif_profile = tbl_data->outlif_profile;
    new_profile.remark_profile = tbl_data->remark_profile;

    SOCDNX_SAND_IF_ERR_EXIT(_bcm_dpp_am_template_out_rif_profile_exchange(unit, 0, tbl_data->outrif_profile_index,&new_profile,&is_last,&new_profile_index,&is_first));
    if(is_last){
        MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_rif_profile_delete, (unit,tbl_data->outrif_profile_index));
    }
    if (is_first) {
        MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_rif_profile_set, (unit, new_profile_index, &new_profile));
    }

    
    entry |= (new_profile_index & ((1 << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) - 1)) << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2);

    
    entry <<= data_inner_offset;
    data[data_index] |= entry;

    for (bank_id=0; bank_id < EDB_ARRAY_AS_CORE_BLOCK; bank_id++) {
        SOCDNX_SAND_IF_ERR_EXIT(soc_mem_array_write(unit, EDB_OUTRIF_TABLEm, bank_id, MEM_BLOCK_ANY, entry_offset, data));
    }
exit:
    SOCDNX_FUNC_RETURN;
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


