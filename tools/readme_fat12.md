# FAT12 Tool

fat12.c is a C program that show every FAT12 info about the disk and list all the files/dir that are present in the root directory.
you can compile it using your C compiler.

To test it I provide a FAT12 disk I made (OEM = JINKOS10), it should return something like this : 

```
Disk disk.img opened successfully!

OEM :                   JINKOS10
JumpBoot :              EB 3C 90
BytesPerSector :        512
SectorsPerClusters :    1
ReservedSectorCount :   1
NumFats :               2
RootEntryCount :        224
TotalSectorsShort :     2880
Media :                 0xF0
FatSizeSector :         9
SectorsPerTrack :       18
NumHeads :              2
HiddenSectors :         0
TotalSectorsLong :      0

Root directory entries:
Volume label: BOOTDISK
HELLO.TXT       [ Attr=0x20 Size=13B Cluster=2 ]
Content of HELLO:
hello i'm teo

KERNEL.BIN      [ Attr=0x20 Size=56B Cluster=3 ]
Content of KERNEL:
zoieutozueiotuozitoiouetiouiozeutiozeuiotuezioutiozeutio

TEST <DIR>      [ Attr=0x10 Size=0B Cluster=4 ]
```
