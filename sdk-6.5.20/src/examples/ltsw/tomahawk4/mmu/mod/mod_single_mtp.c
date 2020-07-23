/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
cint_reset();
bcm_cosq_mod_control_t control;
int unit = 0;

bcm_cosq_mod_control_t_init(&control);
control.valid_flags = BCM_COSQ_MOD_CONTROL_POOL_LIMIT_VALID;
control.pool_limit = 65024; /* 256 Cells. */

print bcm_cosq_mod_control_set(unit, &control);
print bcm_cosq_mod_control_get(unit, &control);
print control;

control.pool_limit = 0;
print bcm_cosq_mod_control_set(unit, &control);
print bcm_cosq_mod_control_get(unit, &control);
print control;

cint_reset();
int unit = 0;
bcm_cosq_mod_profile_t profile;
int profile_id = 0;

bcm_cosq_mod_profile_t_init(&profile);
profile.percent_0_25 = 800;
profile.percent_25_50 = 400;
profile.percent_50_75 = 200;
profile.percent_75_100 = 100;
profile.enable = 1;

/* Expect E_NOT_FOUND. */
print bcm_cosq_mod_profile_set(unit, profile_id, &profile);

print bcm_cosq_mod_profile_create(unit, 0, &profile_id);
print bcm_cosq_mod_profile_set(unit, profile_id, &profile);
print bcm_cosq_mod_profile_get(unit, profile_id, &profile);
print profile;
print bcm_cosq_mod_profile_destroy(unit, profile_id);
/* Expect E_NOT_FOUND. */
print bcm_cosq_mod_profile_get(unit, profile_id, &profile);
print profile;

cint_reset();
int unit = 0;
int dest_port1 = 1;
bcm_gport_t gport;
uint8 dest_mac[6] = {0x00, 0x11, 0x12, 0x13, 0x14, 0x15};
uint8 src_mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
bcm_mirror_source_t source;
int count = 0;

bcm_mirror_destination_t mtp;
bcm_mirror_destination_t_init(&mtp);
BCM_GPORT_MODPORT_SET(gport, 0, dest_port1);
mtp.gport=gport;
mtp.flags2=BCM_MIRROR_DEST_FLAGS2_MOD_TUNNEL_PSAMP;
sal_memcpy(mtp.dst_mac, dest_mac, 6);
sal_memcpy(mtp.src_mac, src_mac, 6);
mtp.tpid=0x8100;
mtp.vlan_id=5;
mtp.version=4;
mtp.src_addr=0x021010a0;
mtp.dst_addr=0x111010a0;
mtp.ttl=2;
mtp.tos=2;
mtp.df=1;
mtp.cosq = 9;
print bcm_mirror_destination_create(unit,&mtp);

bcm_mirror_source_t_init(&source);
source.type = bcmMirrorSourceTypeCosqMod;
print bcm_mirror_source_dest_add(unit, &source, mtp.mirror_dest_id);
print bcm_mirror_source_dest_get_all(unit, &source, 1, &mtp.mirror_dest_id, &count);
print mtp.mirror_dest_id;
print count;


/* Mirror-on-drop counter API. */

bcm_cosq_object_id_t id;
uint64 value1, value2, value3, value4, value5;

id.buffer = 0;

print bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatDroppedPackets, &value1);
print bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatEnqueuedPackets, &value2);
print bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatPoolBytesCurrent, &value3);
print bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatPoolBytesPeak, &value4);

id.port = mtp.gport;
id.cosq = mtp.cosq;
print bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatDestQueueBytesCurrent, &value5);
print value1;
print value2;
print value3;
print value4;
print value5;

print bcm_mirror_source_dest_delete(unit, &source, mtp.mirror_dest_id);
/* Expect E_NOT_FOUND. */
print bcm_mirror_source_dest_get_all(unit, &source, 1, &mtp.mirror_dest_id, &count);

print bcm_mirror_source_dest_add(unit, &source, mtp.mirror_dest_id);
print bcm_mirror_source_dest_delete_all(unit, &source);
/* Expect E_NOT_FOUND. */
print bcm_mirror_source_dest_get_all(unit, &source, 1, &mtp.mirror_dest_id, &count);

print bcm_mirror_destination_destroy(unit, mtp.mirror_dest_id);


