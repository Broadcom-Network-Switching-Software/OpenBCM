/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BFCMAP_VLANMAP_H
#define BFCMAP_VLANMAP_H

struct bfcmap_dev_ctrl_s;

struct bfcmap_port_ctrl_s;

struct bfcmap_int_hw_mapper_s;

/*
 * The following structure is SW abstraction of FCMAP vlanmap. It 
 * contains information about the vlanmap and its corresponding 
 * action. Also it keeps tract of the channel ID to which this
 * vlanmap is bound (If controlled vlanmap).
 */
typedef struct bfcmap_int_vlanmap_entry_s {
    /*
    int                     vlanmapId;
    */

    bfcmap_vlan_vsan_map_t    vlanmap;   /* vlanmap information */

#define BFCMAP_VLANMAP_F_INSTALLED         0x0001
    buint32_t               flags;

    struct bfcmap_port_ctrl_s    *pc;

    /* Next vlanmap entry sharing same bucket index. */
    struct bfcmap_int_vlanmap_entry_s  *next;

    /* Index in the mapper table */
    int                     hw_index;

    /* MP table to which the vlanmap belongs. */
    struct bfcmap_int_hw_mapper_s *mapper;

    /* Pointer to next vlanmap if sharing the same mapper table 
     * entry */
    struct bfcmap_int_vlanmap_entry_s *hw_next;

} bfcmap_int_vlanmap_entry_t;

/*
 * The following structure contains the callbacks for comparing
 * vlanmap, programming HW vlanmap tables, moving vlanmaps in HW etc.
 */
typedef struct bfcmap_int_hw_mapper_vec_s {
    int (*mp_cmp)(struct bfcmap_dev_ctrl_s*, struct bfcmap_port_ctrl_s *, 
                  bfcmap_int_vlanmap_entry_t*, bfcmap_int_vlanmap_entry_t*, buint32_t);
    int (*mp_clear)(struct bfcmap_dev_ctrl_s*, struct bfcmap_port_ctrl_s*,
                    int, int, buint32_t);
    int (*mp_move)(struct bfcmap_dev_ctrl_s*, struct bfcmap_port_ctrl_s*,
                    int, int, int, buint32_t);
    int (*mp_write)(struct bfcmap_dev_ctrl_s*, struct bfcmap_port_ctrl_s*,
                    bfcmap_int_vlanmap_entry_t*, int, buint32_t);
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
    int (*mp_read)(struct bfcmap_dev_ctrl_s*, struct bfcmap_port_ctrl_s*,
                    bfcmap_int_vlanmap_entry_t*, int,
                    bfcmap_int_vlanmap_entry_t**,
                    buint32_t);
#endif /* BCM_WARM_BOOT_SUPPORT */
} bfcmap_hw_mapper_vec_t;


typedef struct bfcmap_int_hw_mapper_s {
    bfcmap_int_vlanmap_entry_t    **hw_vlanmap_tbl;
    int                     max_entry;
    buint32_t               cb_data;
    bfcmap_hw_mapper_vec_t vec;
    bfcmap_sal_lock_t      lock;
} bfcmap_int_hw_mapper_t;

#define BFCMAP_MP_ENTRY_PTR(fil, i)   ((fil)->hw_vlanmap_tbl + (i))

#define BFCMAP_MP_ENTRY(fil, i)      (*((fil)->hw_vlanmap_tbl + (i)))

#define BFCMAP_MP_MOVE_ENTRY(mpc,fil,fi,ti,n)    \
            (fil)->vec.mp_move((mpc)->parent, (mpc), (fi),(ti),(n),(fil)->cb_data)

#define BFCMAP_MP_WRITE_ENTRY(mpc,fil,idx)     \
            (fil)->vec.mp_write((mpc)->parent, (mpc), BFCMAP_MP_ENTRY((fil),(idx)),(idx),(fil)->cb_data)

#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
#define BFCMAP_MP_READ_ENTRY(mpc,fil,idx, new_map)     \
            (fil)->vec.mp_read((mpc)->parent, (mpc), BFCMAP_MP_ENTRY((fil),(idx)),(idx), (new_map),(fil)->cb_data)
#endif /* BCM_WARM_BOOT_SUPPORT */


#define BFCMAP_MP_CMP_ENTRY(mpc,fil,e1,e2)          \
            (fil)->vec.mp_cmp((mpc)->parent, (mpc), (e1),(e2),(fil)->cb_data)

#define BFCMAP_MP_CLR_ENTRY(mpc,fil,idx,n)          \
            (fil)->vec.mp_clear((mpc)->parent,(mpc),(idx),(n),(fil)->cb_data)


#define BFCMAP_MP_VLAN_VSAN_VALID(vid,vfid)   \
    ( (vid) < (1 << 12) && (vfid) < (1 << 12) )


extern int bfcmap_int_vlanmap_init(void);

extern bfcmap_int_hw_mapper_t *
bfcmap_int_hw_mapper_create(struct bfcmap_port_ctrl_s *mpc, 
                     int max_en, 
                     buint32_t cb_arg, 
                     bfcmap_hw_mapper_vec_t *vec);

extern bfcmap_int_vlanmap_entry_t*
bfcmap_int_vlanmap_find_by_id(struct bfcmap_port_ctrl_s *mpc,
                              int vlanid, int vfid);

extern bfcmap_int_vlanmap_entry_t*
bfcmap_int_vlanmap_get_next_or_first(struct bfcmap_port_ctrl_s *mpc,
                           bfcmap_int_hw_mapper_t  *mapper, 
                           int vlanid, int vfid);


extern bfcmap_int_vlanmap_entry_t*
bfcmap_int_vlanmap_add_or_update(struct bfcmap_port_ctrl_s *mpc, 
                           bfcmap_vlan_vsan_map_t *vlanmap);

extern int
bfcmap_int_vlanmap_delete(struct bfcmap_port_ctrl_s *mpc,
                          int vlanid, int vfid);


extern int
bfcmap_int_vlanmap_add_to_hw(struct bfcmap_port_ctrl_s *mpc, 
                           bfcmap_int_hw_mapper_t  *mapper,
                           bfcmap_int_vlanmap_entry_t *fle);

#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
extern int
bfcmap_int_vlanmap_get_from_hw(struct bfcmap_port_ctrl_s *mpc, 
                           bfcmap_int_hw_mapper_t  *mapper);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int
bfcmap_int_vlanmap_delete_from_hw(struct bfcmap_port_ctrl_s *mpc, 
                                bfcmap_int_hw_mapper_t  *mapper,
                                bfcmap_int_vlanmap_entry_t *fle);

#endif /* BFCMAP_VLANMAP_H */

