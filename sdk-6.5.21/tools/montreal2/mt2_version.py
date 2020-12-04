#!/usr/bin/env python

#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

import os, sys
import array

def parse_fw_file(fn):
    f = open(fn)
    data = f.read()
    f.close()

    length = len(data)
    hp = 0

    while (hp < length) :
       if (data[hp:hp+5] != "MAGIC") :
           print "Invalid firmware file !"
           return


       section_length =  (ord(data[hp+5]) << 16) + (ord(data[hp+6]) << 8) + ord(data[hp+7])

       if ((ord(data[hp+8]) == 0x1) and (ord(data[hp+10]) == 0x1)) :
           print "SRAM_CODE found"
           extract(data[hp+16:hp+16+section_length])
#           return
#       else:
#           print "NON SRAM_SECTION"

       hp += section_length + 16


def extract(data):
    a=array.array("L",data)
    a.byteswap()
    data=a.tostring()
    p = data[8:].split('\x00')
    print p[0]

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage: version.py file'
    else:
        parse_fw_file(sys.argv[1])


