/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __QAX_MULTICAST_IMP_INCLUDED__
#define __QAX_MULTICAST_IMP_INCLUDED__





#include <soc/types.h>
#include <soc/dpp/multicast.h>
#include <soc/dpp/multicast_imp.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>






#define QAX_NOF_GROUPS_PER_IRDB_ENTRY 32 
#define QAX_CGM_TABLE_ENTRY_WORDS 1 
#define QAX_ING_MC_NOF_DEST_BITS 17 
#define QAX_MC_CUD_NOF_BITS 18 
#define QAX_MC_CUD_TOO_BIG (1 << QAX_MC_CUD_NOF_BITS) 





#define QAX_MC_ENTRY_SIZE 3 

#define QAX_MC_ENTRY_MASK_VAL 0xff

#define QAX_MC_ENTRY_MASK_WORD(mcds, index) ((mcds)->mcdb[index].word2) 

#define QAX_MCDS_TYPE_SHIFT 29 
#define QAX_NOF_MCDB_INDEX_BITS 17 
#define QAX_MCDB_INDEX_MASK ((1 << QAX_NOF_MCDB_INDEX_BITS) - 1)
#define QAX_MCDS_PREV_INDEX_SHIFT (QAX_MCDS_TYPE_SHIFT - QAX_NOF_MCDB_INDEX_BITS) 
#define QAX_MCDS_TEST_BIT_SHIFT (QAX_MCDS_PREV_INDEX_SHIFT -1) 


#define QAX_MCDS_LAST_WORD_KEEP_BITS_MASK  \
  (1 << QAX_MCDS_TEST_BIT_SHIFT)



#define QAX_MCDS_ENTRY_GET_TYPE(entry) ((entry)->word2 >> QAX_MCDS_TYPE_SHIFT) 
#define QAX_MCDS_ENTRY_SET_TYPE(entry, type_value)  \
    do {(entry)->word2 = ((entry)->word2 & ~(DPP_MCDS_TYPE_MASK << QAX_MCDS_TYPE_SHIFT)) | \
      ((type_value) << QAX_MCDS_TYPE_SHIFT); } while (0)
#define QAX_MCDS_GET_TYPE(mcds, index) QAX_MCDS_ENTRY_GET_TYPE((mcds)->mcdb + (index))
#define QAX_MCDS_SET_TYPE(mcds, index, type_value) QAX_MCDS_ENTRY_SET_TYPE((mcds)->mcdb + (index), (type_value))



#define QAX_MCDS_GET_PREV_ENTRY(mcds, index) \
  ((QAX_MC_ENTRY_MASK_WORD((mcds), (index)) >> QAX_MCDS_PREV_INDEX_SHIFT) & QAX_MCDB_INDEX_MASK)

#define QAX_MCDS_SET_PREV_ENTRY(mcds, index, prev_entry) \
  do { \
    QAX_MC_ENTRY_MASK_WORD((mcds), (index)) = (QAX_MC_ENTRY_MASK_WORD((mcds), (index)) & \
      ~(QAX_MCDB_INDEX_MASK << QAX_MCDS_PREV_INDEX_SHIFT)) | \
      ((prev_entry) << QAX_MCDS_PREV_INDEX_SHIFT); \
  } while(0);

#define QAX_MCDS_ENTRY_GET_PREV_ENTRY(entry, mcds, index) \
  ((((entry)->word2 >> QAX_MCDS_PREV_INDEX_SHIFT) & QAX_MCDB_INDEX_MASK) | (mcds)->prev_entries[index])
#define QAX_MCDS_ENTRY_SET_PREV_ENTRY(entry, prev_entry) \
  do { \
    (entry)->word2 = ((entry)->word2 & \
      ~(QAX_MCDB_INDEX_MASK << QAX_MCDS_PREV_INDEX_SHIFT)) | \
      ((prev_entry) << QAX_MCDS_PREV_INDEX_SHIFT); \
  } while(0);


#define QAX_MCDS_GET_FREE_NEXT_ENTRY(mcds, index)  \
  ((mcds)->mcdb[index].word1)
#define QAX_MCDS_SET_FREE_NEXT_ENTRY(mcds, index, next_entry)  \
  (mcds)->mcdb[index].word1 = next_entry
#define QAX_MCDS_ENTRY_GET_FREE_PREV_ENTRY(entry)  \
  ((entry)->word0 & QAX_MCDB_INDEX_MASK)
#define QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, index)  \
  ((mcds)->mcdb[index].word0)
#define QAX_MCDS_SET_FREE_PREV_ENTRY(mcds, index, prev_entry)  \
  (mcds)->mcdb[index].word0 = (prev_entry)
#define QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, index)  \
  ((mcds)->mcdb[index].word2 & DPP_MCDS_FREE_BLOCK_SIZE_MASK)
#define QAX_MCDS_SET_FREE_BLOCK_SIZE(mcds, index, size)  \
  (mcds)->mcdb[index].word2 = ((mcds)->mcdb[index].word2 & ~(QAX_MCDB_INDEX_MASK )) | (size)


#define QAX_MCDS_ENTRY_GET_TEST_BIT(     entry)    (((entry)->word2 >> QAX_MCDS_TEST_BIT_SHIFT) & 1)
#define QAX_MCDS_ENTRY_SET_TEST_BIT_ON(  entry) do {(entry)->word2 |=  (1 << QAX_MCDS_TEST_BIT_SHIFT);} while (0)
#define QAX_MCDS_ENTRY_SET_TEST_BIT_OFF( entry) do {(entry)->word2 &= ~(1 << QAX_MCDS_TEST_BIT_SHIFT);} while (0)
#define QAX_MCDS_ENTRY_SET_TEST_BIT(entry, val) do {(entry)->word2 &= ~(1 << QAX_MCDS_TEST_BIT_SHIFT); (entry)->word2 |= (((val) & 1) << QAX_MCDS_TEST_BIT_SHIFT);} while (0)


#define QAX_MULT_MAX_INGRESS_REPLICATIONS 4094 
#define QAX_MULT_MAX_EGRESS_REPLICATIONS 4095  
#define QAX_MULT_MAX_REPLICATIONS QAX_MULT_MAX_EGRESS_REPLICATIONS 

#define QAX_MC_ING_DESTINATION_DISABLED 0x1ffff 






#define QAX_MCDS_REP_DATA_DEST_OFFSET 0
#define QAX_MCDS_REP_DATA_CUD1_OFFSET QAX_ING_MC_NOF_DEST_BITS
#define QAX_MCDS_REP_DATA_CUD2_OFFSET (QAX_MCDS_REP_DATA_CUD1_OFFSET + QAX_MC_CUD_NOF_BITS)
#define QAX_MCDS_REP_DATA_TYPE_OFFSET (QAX_MCDS_REP_DATA_CUD2_OFFSET + QAX_MC_CUD_NOF_BITS)
#define QAX_MCDS_REP_DATA_TYPE_SIZE 1

#define QAX_MCDS_REP_DATA_DEST_MASK ((1 << QAX_ING_MC_NOF_DEST_BITS) - 1)
#define QAX_MCDS_REP_DATA_CUD_MASK ((1 << QAX_MC_CUD_NOF_BITS) - 1)
#define QAX_MCDS_REP_DATA_CUD1_LSB_MASK (QAX_MCDS_REP_DATA_CUD_MASK << QAX_MCDS_REP_DATA_CUD1_OFFSET)
#define QAX_MCDS_REP_DATA_CUD1_MSB_MASK (QAX_MCDS_REP_DATA_CUD_MASK >> (32 - QAX_MCDS_REP_DATA_CUD1_OFFSET))
#define QAX_MCDS_REP_DATA_CUD2_LSB_MASK 0
#define QAX_MCDS_REP_DATA_CUD2_MSB_MASK (QAX_MCDS_REP_DATA_CUD_MASK << (QAX_MCDS_REP_DATA_CUD2_OFFSET - 32))
#define QAX_MCDS_REP_DATA_TYPE_MASK ((1 << QAX_MCDS_REP_DATA_TYPE_SIZE) - 1)
#define QAX_MCDS_REP_DATA_TYPE_LSB_MASK 0
#define QAX_MCDS_REP_DATA_TYPE_MSB_MASK (QAX_MCDS_REP_DATA_TYPE_MASK << (QAX_MCDS_REP_DATA_TYPE_OFFSET - 32))



#define QAX_MCDS_REP_DATA_SET_DEST(data, dest) do {\
    uint64 _repdata_temp;\
    COMPILER_64_SET(_repdata_temp, 0, (dest));\
    COMPILER_64_OR(*(data), _repdata_temp);\
} while (0)
#define QAX_MCDS_REP_DATA_RESET_DEST(data, dest) do {\
    uint64 _repdata_temp, _repdata_temp2;\
    COMPILER_64_SET(_repdata_temp, ~0, ~QAX_MCDS_REP_DATA_DEST_MASK);\
    COMPILER_64_SET(_repdata_temp2, 0, (dest));\
    COMPILER_64_AND(*(data), _repdata_temp);\
    COMPILER_64_OR(*(data), _repdata_temp2);\
} while (0)
#define QAX_MCDS_REP_DATA_GET_DEST(data, dest) do {\
    (dest) = COMPILER_64_LO((*data)) & QAX_MCDS_REP_DATA_DEST_MASK;\
} while (0)

#define QAX_MCDS_REP_DATA_SET_CUD1(data, cud) do {\
    uint64 _repdata_temp;\
    COMPILER_64_SET(_repdata_temp, 0, (cud));\
    COMPILER_64_SHL(_repdata_temp, QAX_MCDS_REP_DATA_CUD1_OFFSET);\
    COMPILER_64_OR(*(data), _repdata_temp);\
} while (0)
#define QAX_MCDS_REP_DATA_RESET_CUD1(data, cud) do {\
    uint64 _repdata_temp, _repdata_temp2;\
    COMPILER_64_SET(_repdata_temp, ~QAX_MCDS_REP_DATA_CUD1_MSB_MASK, ~QAX_MCDS_REP_DATA_CUD1_LSB_MASK);\
    COMPILER_64_SET(_repdata_temp2, 0, (cud));\
    COMPILER_64_SHL(_repdata_temp2, QAX_MCDS_REP_DATA_CUD1_OFFSET);\
    COMPILER_64_AND(*(data), _repdata_temp);\
    COMPILER_64_OR(*(data), _repdata_temp2);\
} while (0)
#define QAX_MCDS_REP_DATA_GET_CUD1(data, cud) do {\
    uint64 _repdata_temp;\
    COMPILER_64_COPY(_repdata_temp, (*data));\
    COMPILER_64_SHR(_repdata_temp2, QAX_MCDS_REP_DATA_CUD1_OFFSET);\
    (cud) = COMPILER_64_LO(_repdata_temp2) & QAX_MCDS_REP_DATA_CUD_MASK;\
} while (0)

#define QAX_MCDS_REP_DATA_SET_CUD2(data, cud) do {\
    uint64 _repdata_temp;\
    COMPILER_64_SET(_repdata_temp, (cud) << (QAX_MCDS_REP_DATA_CUD2_OFFSET - 32), 0);\
    COMPILER_64_OR(*(data), _repdata_temp);\
} while (0)
#define QAX_MCDS_REP_DATA_RESET_CUD2(data, cud) do {\
    uint64 _repdata_temp, _repdata_temp2;\
    COMPILER_64_SET(_repdata_temp, ~QAX_MCDS_REP_DATA_CUD2_MSB_MASK, ~QAX_MCDS_REP_DATA_CUD2_LSB_MASK);\
    COMPILER_64_SET(_repdata_temp2, (cud) << (QAX_MCDS_REP_DATA_CUD2_OFFSET - 32), 0);\
    COMPILER_64_AND(*(data), _repdata_temp);\
    COMPILER_64_OR(*(data), _repdata_temp2);\
} while (0)
#define QAX_MCDS_REP_DATA_GET_CUD2(data, cud) do {\
    uint64 _repdata_temp;\
    COMPILER_64_COPY(_repdata_temp, (*data));\
    COMPILER_64_SHR(_repdata_temp2, QAX_MCDS_REP_DATA_CUD2_OFFSET);\
    (cud) = COMPILER_64_LO(_repdata_temp2) & QAX_MCDS_REP_DATA_CUD_MASK;\
} while (0)

#define QAX_MCDS_REP_DATA_SET_TYPE(data, rep_type) do {\
    uint64 _repdata_temp;\
    COMPILER_64_SET(_repdata_temp, (rep_type) << (QAX_MCDS_REP_DATA_TYPE_OFFSET - 32), 0);\
    COMPILER_64_OR(*(data), _repdata_temp);\
} while (0)
#define QAX_MCDS_REP_DATA_RESET_TYPE(data, rep_type) do {\
    uint64 _repdata_temp, _repdata_temp2;\
    COMPILER_64_SET(_repdata_temp, ~QAX_MCDS_REP_DATA_TYPE_MSB_MASK, ~QAX_MCDS_REP_DATA_TYPE_LSB_MASK);\
    COMPILER_64_SET(_repdata_temp2, (rep_type) << (QAX_MCDS_REP_DATA_TYPE_OFFSET - 32), 0);\
    COMPILER_64_AND(*(data), _repdata_temp);\
    COMPILER_64_OR(*(data), _repdata_temp2);\
} while (0)
#define QAX_MCDS_REP_DATA_GET_TYPE(data, rep_type) do {\
    uint64 _repdata_temp;\
    COMPILER_64_COPY(_repdata_temp, (*data));\
    COMPILER_64_SHR(_repdata_temp, QAX_MCDS_REP_DATA_TYPE_OFFSET);\
    (rep_type) = COMPILER_64_LO(_repdata_temp) & QAX_MCDS_REP_DATA_TYPE_MASK;\
} while (0)

#define QAX_MCDS_REP_DATA_GET_FIELDS(data, dest, cud1, cud2, rep_type) do {\
    uint64 _repdata_temp;\
    COMPILER_64_COPY(_repdata_temp, (*data));\
    (dest) = COMPILER_64_LO((*data)) & QAX_MCDS_REP_DATA_DEST_MASK;\
    COMPILER_64_SHR(_repdata_temp, QAX_MCDS_REP_DATA_CUD1_OFFSET);\
    (cud1) = COMPILER_64_LO(_repdata_temp) & QAX_MCDS_REP_DATA_CUD_MASK;\
    COMPILER_64_SHR(_repdata_temp, QAX_MCDS_REP_DATA_CUD2_OFFSET - QAX_MCDS_REP_DATA_CUD1_OFFSET);\
    (cud2) = COMPILER_64_LO(_repdata_temp) & QAX_MCDS_REP_DATA_CUD_MASK;\
    COMPILER_64_SHR(_repdata_temp, QAX_MCDS_REP_DATA_TYPE_OFFSET - QAX_MCDS_REP_DATA_CUD2_OFFSET);\
    (rep_type) = COMPILER_64_LO(_repdata_temp) & QAX_MCDS_REP_DATA_TYPE_MASK;\
} while (0)

#define QAX_MCDS_REP_DATA_CLEAR(data) do {COMPILER_64_ZERO((*data));} while (0)

#define QAX_MCDS_REP_TYPE_DEST 0
#define QAX_MCDS_REP_TYPE_BM 1



#define QAX_MCDS_GET_EGRESS_GROUP_START(mcds, mcid) ((mcds)->egress_mcdb_offset + (mcid))

#define QAX_GET_MCDB_ENTRY(mcds, index) ((mcds)->mcdb + (index))

#define QAX_LAST_MCDB_ENTRY(_unit) (SOC_IS_QUX(_unit) ? 0xbfff : 0x17fff)

#define QAX_NO_MCDB_INDEX ((uint32)(-1))

#define QAX_MC_INGRESS_LINK_PTR_END (SOC_IS_QUX(unit) ? 0xffff : 0x1ffff) 


#define QAX_MCDS_STATE_INITED 1 
#define QAX_MCDS_STATE_STARTED 2 
#define QAX_MCDS_STATE_FINISHED 3 






typedef uint64 qax_rep_data_t; 


typedef struct
{
  uint32 word0;
  uint32 word1;
  uint32 word2;
} qax_mcdb_entry_t;

typedef struct {
    dpp_mcds_common_t common; 
    uint32 nof_unoccupied; 

    
    qax_mcdb_entry_t *mcdb;    
    qax_mcdb_entry_t free_value; 
    qax_mcdb_entry_t empty_ingr_value; 
    uint32 egress_mcdb_offset;   
    uint32 ingress_bitmap_start; 
    uint32 egress_bitmap_start;  
    int unit; 
    dpp_free_entries_blocks_region_t no_starts; 
    dpp_free_entries_blocks_region_t ingress_starts; 
    dpp_free_entries_blocks_region_t egress_starts; 

#ifdef DONOT_USE_SW_DB_FOR_MULTICAST
    uint32 *egress_groups_open_data; 
#endif

    

    
    qax_rep_data_t reps[QAX_MULT_MAX_REPLICATIONS]; 

    qax_mcdb_entry_t *cur_entry;  
    qax_mcdb_entry_t start_entry; 
    uint32 cur_entry_index;       
    
    uint32 list_start;        
    uint32 list_end;          
    uint32 linked_list;       
    uint32 linked_list_end;   

    uint32 alloc_flags;       
    uint32 hw_end;            
    uint32 block_start;       
    SOC_TMC_ERROR out_err;    

    uint16 nof_reps;          
    uint16 nof_bitmap_reps;   
    uint16 nof_dest_cud_reps; 
    uint16 nof_possible_reps; 
    uint8 group_type;         
    uint8 group_type_start;   
    uint8 writing_full_list;  
    uint8 is_group_start_to_be_filled; 
    uint8 state;              
    dpp_free_entries_block_size_t block_size; 

} qax_mcds_t; 












uint32 qax_mcds_get_replications_from_entry(
    SOC_SAND_IN    int     unit,
    SOC_SAND_IN    uint32  entry_index, 
    SOC_SAND_IN    uint32  entry_type,  
    SOC_SAND_INOUT uint16  *max_size,   
    SOC_SAND_INOUT uint16  *group_size, 
    SOC_SAND_OUT   uint32  *next_entry  
);

#ifdef QAX_TO_BE_IMPLEMENTED 

uint32 qax_mcdb_get_next_pointer(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry,      
    SOC_SAND_IN  uint32  entry_type, 
    SOC_SAND_OUT uint32  *next_entry 
);


uint32 qax_mcdb_set_next_pointer(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry_to_set, 
    SOC_SAND_IN  uint32  entry_type,   
    SOC_SAND_IN  uint32  next_entry    
);


uint32 qax_convert_ingress_replication_hw2api(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       cud,            
    SOC_SAND_IN  uint32       dest,           
    SOC_SAND_OUT soc_gport_t  *port_array,    
    SOC_SAND_OUT soc_if_t     *encap_id_array 
);


uint32 qax_mult_ing_encode_entry(
    SOC_SAND_IN    int                    unit,
    SOC_SAND_IN    SOC_TMC_MULT_ING_ENTRY *ing_entry,       
    SOC_SAND_OUT   uint32                 *out_destination, 
    SOC_SAND_OUT   uint32                 *out_cud          
  );
#endif  




uint32
  qax_mcds_write_entry(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mcdb_index 
);



uint32 qax_mcds_unoccupied_get(
    SOC_SAND_IN qax_mcds_t *mcds
);



uint32 qax_mcds_copy_replications_from_arrays(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint8     is_egress,       
    SOC_SAND_IN  uint8     do_clear,        
    SOC_SAND_IN  uint32    arrays_size,     
    SOC_SAND_IN  dpp_mc_replication_t *reps 
);


uint32 qax_mcds_copy_replications_to_arrays(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint8        is_egress,           
    SOC_SAND_IN  uint32       arrays_size,         
    SOC_SAND_OUT soc_gport_t  *port_array,         
    SOC_SAND_OUT soc_if_t     *encap_id_array,     
    SOC_SAND_OUT soc_multicast_replication_t *reps 
);





dpp_free_entries_blocks_region_t* qax_mcds_get_region(qax_mcds_t *mcds, uint32 mcdb_index);


uint32 qax_mcds_build_free_blocks(
    SOC_SAND_IN    int                              unit,   
    SOC_SAND_INOUT qax_mcds_t                       *mcds,
    SOC_SAND_IN    uint32                           start_index, 
    SOC_SAND_IN    uint32                           end_index,   
    SOC_SAND_INOUT dpp_free_entries_blocks_region_t *region,     
    SOC_SAND_IN    mcds_free_build_option_t         entry_option 
);


uint32 qax_mcds_get_free_entries_block(
    SOC_SAND_INOUT qax_mcds_t                    *mcds,
    SOC_SAND_IN    uint32                        flags,        
    SOC_SAND_IN    dpp_free_entries_block_size_t wanted_size,  
    SOC_SAND_IN    dpp_free_entries_block_size_t max_size,     
    SOC_SAND_OUT   uint32                        *block_start, 
    SOC_SAND_OUT   dpp_free_entries_block_size_t *block_size   
);


uint32 qax_mcds_reserve_group_start(
    SOC_SAND_INOUT qax_mcds_t *mcds,
    SOC_SAND_IN    uint32           mcdb_index 
);



uint32
    qax_mcds_multicast_init(
      SOC_SAND_IN int unit
);


uint32
    qax_mcds_multicast_init2(
      SOC_SAND_IN int unit
);


uint32
    qax_mcds_multicast_terminate(
        SOC_SAND_IN int unit
    );


uint32 qax_mult_does_group_exist(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_MULT_ID mcid,       
    SOC_SAND_IN  int             is_egress,  
    SOC_SAND_OUT uint8           *does_exist 
);



int qax_rep_data_t_compare(void *a, void *b);


#define QAX_EQ_REP_DATA(rep1, rep2) COMPILER_64_EQ(*(rep1), *(rep2))



uint32
  qax_mcdb_relocate_entries(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        mcdb_index,             
    SOC_SAND_IN  uint32                        relocation_block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t relocation_block_size   
);




uint32
  qax_mcdb_free_linked_list_till_my_end(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 first_index,  
    SOC_SAND_IN uint32 entries_type, 
    SOC_SAND_IN uint32 end_index     
);



uint32
  qax_mcdb_free_linked_list(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  first_index, 
    SOC_SAND_IN uint32  entries_type 
);



uint32 qax_mcds_get_group(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                do_clear,     
    SOC_SAND_IN  uint32               group_id,     
    SOC_SAND_IN  uint32               group_type,   
    SOC_SAND_IN  uint16               max_size,     
    SOC_SAND_OUT uint16               *group_size   
);


uint32 qax_mcds_remove_replications_from_group(
    SOC_SAND_IN int                  unit,       
    SOC_SAND_IN dpp_mc_id_t          group_mcid, 
    SOC_SAND_IN uint32               type,       
    SOC_SAND_IN uint32               nof_reps,   
    SOC_SAND_IN dpp_mc_replication_t *reps       
);


uint32 qax_mult_remove_replications(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32               group_type,   
    SOC_SAND_IN  uint32               nof_reps,   
    SOC_SAND_IN  dpp_mc_replication_t *reps       
);


uint32 qax_mult_rplct_tbl_entry_unoccupied_set_all(
    SOC_SAND_IN  int unit
);





uint32 qax_mult_ing_group_open(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *mc_group,        
    SOC_SAND_IN  uint32                 mc_group_size,    
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          
);


uint32 qax_mult_ing_group_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t      multicast_id_ndx 
);


uint32 qax_mult_ing_group_update(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  dpp_mc_id_t          multicast_id_ndx,    
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY   *mc_group,           
    SOC_SAND_IN  uint32                mc_group_size,       
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err             
);


uint32 qax_mult_ing_add_replications(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  dpp_mc_id_t                 mcid,     
    SOC_SAND_IN  uint32                      nof_reps, 
    SOC_SAND_IN  soc_multicast_replication_t *reps,    
    SOC_SAND_OUT SOC_TMC_ERROR               *out_err  
);


uint32 qax_mult_ing_remove_replications(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  dpp_mc_id_t                 mcid,     
    SOC_SAND_IN  uint32                      nof_reps, 
    SOC_SAND_IN  soc_multicast_replication_t *reps,    
    SOC_SAND_OUT SOC_TMC_ERROR               *out_err  
);


uint32 qax_mult_ing_group_size_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx,
    SOC_SAND_OUT uint32      *mc_group_size
);


uint32 qax_mult_ing_get_group(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t group_mcid,           
    SOC_SAND_IN  uint32      mc_group_size,        
    SOC_SAND_OUT soc_gport_t *ports,               
    SOC_SAND_OUT soc_if_t    *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,
    SOC_SAND_OUT uint32      *exact_mc_group_size, 
    SOC_SAND_OUT uint8       *is_open              
);


uint32 qax_mult_ing_encode_entry(
    SOC_SAND_IN    int                    unit,
    SOC_SAND_IN    SOC_TMC_MULT_ING_ENTRY *ing_entry,       
    SOC_SAND_OUT   uint32                 *out_destination, 
    SOC_SAND_OUT   uint32                 *out_cud          
);






uint32 qax_mult_eg_group_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  dpp_mc_id_t          mcid,         
    SOC_SAND_IN  uint8                allow_create, 
    SOC_SAND_IN  uint32               group_size,   
    SOC_SAND_IN  dpp_mc_replication_t *reps,        
    SOC_SAND_OUT SOC_TMC_ERROR        *out_err      
);


uint32 qax_mult_eg_group_close(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t mcid
);


uint32 qax_mult_eg_reps_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid, 
    SOC_SAND_IN  uint32                nof_reps,   
    SOC_SAND_IN  dpp_mc_replication_t  *reps,      
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err    
);


uint32 qax_mult_eg_reps_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid,   
    SOC_SAND_IN  uint32                nof_reps,     
    SOC_SAND_IN  dpp_mc_replication_t  *reps,        
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err      
);


uint32 qax_mult_eg_group_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  dpp_mc_id_t  multicast_id_ndx,
    SOC_SAND_OUT uint32       *mc_group_size
);


uint32 qax_mult_eg_get_group(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t    group_mcid,           
    SOC_SAND_IN  uint32         mc_group_size,        
    SOC_SAND_OUT soc_gport_t    *ports,               
    SOC_SAND_OUT soc_if_t       *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,   
    SOC_SAND_OUT uint32         *exact_mc_group_size, 
    SOC_SAND_OUT uint8          *is_open              
);



uint32 qax_mcds_set_linked_list(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint8                         list_type,           
    SOC_SAND_IN uint32                        group_id,            
    SOC_SAND_IN uint32                        list_end,            
    SOC_SAND_IN uint32                        alloced_block_start, 
    SOC_SAND_IN dpp_free_entries_block_size_t alloced_block_size,   
    SOC_SAND_IN uint32                        to_delete            
);




soc_error_t qax_mult_get_entry(
                  int    unit,
                  uint32 mcdb_index,
                  uint32 *entry 
);

uint32 qax_mult_eg_bitmap_group_create(
                 int         unit,
                 dpp_mc_id_t multicast_id_ndx 
);


uint32 qax_mult_eg_bitmap_group_close(
                  int          unit,
                  dpp_mc_id_t  multicast_id_ndx
);


uint32 qax_mult_eg_bitmap_group_update(
                  int                                   unit,
                  dpp_mc_id_t                           multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group 
);


uint32 qax_mult_eg_bitmap_group_get(
                  int                                   unit,
                  dpp_mc_id_t                           bitmap_id, 
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group     
);


uint32 qax_mult_eg_bitmap_group_add(
                  int                 unit,
                  dpp_mc_id_t         multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *add_bm, 
                  SOC_TMC_ERROR                         *out_err 
);


uint32 qax_mult_eg_bitmap_group_remove(
                  int                 unit,
                  dpp_mc_id_t         multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *bitmap, 
                  SOC_TMC_ERROR                         *out_err 
);


int qax_mcds_test_free_entries( 
    SOC_SAND_IN int unit
);



#endif 
