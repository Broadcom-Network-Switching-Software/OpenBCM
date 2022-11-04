

#include "kaps_jr1_allocator.h"
#include "kaps_jr1_model_internal.h"
#include "kaps_jr1_model_rpb.h"

kaps_jr1_status kaps_jr1_c_model_rpb_create(struct kaps_jr1_c_model *m, uint8_t rpb_id, struct kaps_jr1_c_model_rpb **rpb_pp)
{
    struct kaps_jr1_c_model_rpb *self = NULL;
    struct kaps_jr1_allocator *alloc = m->alloc;

    self = alloc->xcalloc(alloc->cookie, 1, sizeof(struct kaps_jr1_c_model_rpb));

    if (!self)
        return KAPS_JR1_OUT_OF_MEMORY;

    self->model = m;

    *rpb_pp = self;

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_rpb_destroy(struct kaps_jr1_c_model_rpb *rpb)
{
    struct kaps_jr1_allocator *alloc;

    if (rpb) {
        alloc = rpb->model->alloc;
        alloc->xfree(alloc->cookie, rpb);
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_rpb_write(struct kaps_jr1_c_model_rpb *rpb, uint32_t row_nr, uint8_t * data, uint8_t is_valid)
{
    kaps_jr1_memcpy(rpb->rows[row_nr].entry, data, KAPS_JR1_RPB_WIDTH_8);

    if (rpb->model->config.is_x_y_valid_bit_common) 
    {
      
        if (rpb->model->config.is_valid_bit_updated_only_on_x_write)
        {
            if ( (row_nr & 1) == 0)
            {
                
                rpb->rows[row_nr].valid_bit = is_valid;
                rpb->rows[row_nr + 1].valid_bit = is_valid; 
            }
                
        }
        else
        {
            rpb->rows[row_nr].valid_bit = is_valid;

            if (row_nr & 1) 
            {
                rpb->rows[row_nr - 1].valid_bit = is_valid;
            } 
            else 
            {
                rpb->rows[row_nr + 1].valid_bit = is_valid;
            }
        }


    }
    else
    {
        rpb->rows[row_nr].valid_bit = is_valid;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status kaps_jr1_c_model_rpb_read(struct kaps_jr1_c_model_rpb *rpb, uint32_t row_nr, uint8_t * data, uint8_t *is_valid)
{
    kaps_jr1_memcpy(data, rpb->rows[row_nr].entry,  KAPS_JR1_RPB_WIDTH_8);

    *is_valid = rpb->rows[row_nr].valid_bit ;

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_rpb_compare(
    struct kaps_jr1_c_model_rpb *rpb, 
    uint32_t rpb_nr,
    uint8_t * rpb_key, 
    int32_t * entry_index)
{
    struct kaps_jr1_c_model *model = rpb->model;
    uint32_t i = 0, row_nr = 0;
    int32_t j = 0;
    int32_t entry_matched;
    uint8_t *data_x, *data_y;
    uint8_t x_c, y_c;

    *entry_index = -1;

    
    for (i = 0; i < model->config.num_rows_in_rpb[rpb_nr]; i++ ) {

        row_nr = 2 * i;

        if (!rpb->rows[row_nr].valid_bit || !rpb->rows[row_nr + 1].valid_bit)
            continue;

        data_x = rpb->rows[row_nr].entry;
        data_y = rpb->rows[row_nr + 1].entry;

        entry_matched = 1;
        for (j = 0; j < KAPS_JR1_RPB_WIDTH_8; ++j) {

            if (data_x[j] == 0 && data_y[j] == 0)
                continue;

            
            x_c = ~data_x[j];
            y_c = ~data_y[j];

            if ( ((x_c & y_c) | (x_c & ~rpb_key[j]) | (y_c & rpb_key[j])) == 0xFF) {
                continue;
            } else {
                entry_matched = 0;
                break;
            }

        }

        if (entry_matched == 1) {
            *entry_index = i;
            return KAPS_JR1_OK;
        }

    }

    
    return KAPS_JR1_OK;
}

