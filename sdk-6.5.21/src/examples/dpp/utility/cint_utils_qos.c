/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * This file provides switch basic functionality 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */







/* ************************************************************************************************** */
int qos__create_egress_qos_profile_with_id_color_and_priority
                                (int unit,int id,int color,int prio)
{
    int rc = BCM_E_NONE;
    bcm_qos_map_t map;
    int flags = 0;
    int qos_map_id = id;
     /*Configure qos*/
    bcm_qos_map_t_init(&map);
    map.int_pri = prio;
    map.color = color;
    rc = qos__create_egress_qos_profile(unit,flags,&map,&qos_map_id,TRUE);
    if (BCM_E_NONE != rc)
    {
        printf("Error in qos__create_egress_qos_profile \n");
        return rc;
    }
    return rc;
}
/* creates an entry in the cos profile table which will update the TC and DP in ETPP*/
int qos__create_egress_qos_profile(/*in*/ int unit, int flags, bcm_qos_map_t *map,/*out*/ int *qos_map_id,int with_id)
{
    int rc = BCM_E_NONE;
    int create_flags = BCM_QOS_MAP_EGRESS| BCM_QOS_MAP_ENCAP_INTPRI_COLOR;

    if(with_id)
    {
        create_flags |= BCM_QOS_MAP_WITH_ID;
    }

    rc = bcm_qos_map_create(unit, create_flags, qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_create\n");
        return rc;
    }

     printf("Created an entry with id: %d,  0x%x \n", *qos_map_id,*qos_map_id);


    rc = bcm_qos_map_add(unit, flags, map, (*qos_map_id));
    if (BCM_E_NONE != rc) 
    {
        printf("Error in bcm_qos_map_add %d \n",rc);
        return rc;
    }
    
   
    return rc;


}

/* Deletes and destroys the cos profile entry*/
int qos__delete_cos_profile(/*in*/ int unit, int flags, int cos_id)
{
    /* dummy map struct for the function */
    bcm_qos_map_t map;
    int rc = BCM_E_NONE;
    bcm_qos_map_t_init(&map);

    bcm_qos_map_delete( unit, flags, &map, cos_id); 
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_delete\n");
        return rc;
    }
    bcm_qos_map_destroy(unit, cos_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_destroy\n");
        return rc;
    }

    return rc;

}

