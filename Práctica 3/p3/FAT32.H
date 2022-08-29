//********************************************************
// Autor: SOA
//********************************************************


#ifndef _FAT32_H_
#define _FAT32_H_

#include <stdint.h>
 
//Structure to access Master Boot Record for getting info about partioions
struct MBRinfo_Structure{
uint8_t     nothing[446];      //ignore, placed here to fill the gap in the structure
uint8_t     partitionData[64]; //partition records (16x4)
uint16_t    signature;         //0xaa55
};


//Structure to access info of the first partioion of the disk
struct partitionInfo_Structure{                      
uint8_t       status;                 //0x80 - active partition
uint8_t       headStart;              //starting head
uint16_t      cylSectStart;           //starting cylinder and sector
uint8_t     type;                   //partition type
uint8_t     headEnd;                //ending head of the partition
uint16_t      cylSectEnd;    //ending cylinder and sector
uint32_t     firstSector;    //total sectors between MBR & the first sector of the partition
uint32_t     sectorsTotal;   //size of this partition in sectors
};


//Structure to access boot sector data
struct  __attribute__((__packed__)) BS_Structure {
	uint8_t  jumpBoot[3];      //default: 0x009000EB
	uint8_t  OEMName[8];
	uint16_t bytesPerSector;   //deafault: 512
	uint8_t  sectorPerCluster;
	uint16_t reservedSectorCount;
	uint8_t  numberofFATs;
	uint16_t rootEntryCount;
	uint16_t totalSectors_F16; //must be 0 for FAT32
	uint8_t  mediaType;
	uint16_t FATsize_F16;      //must be 0 for FAT32
	uint16_t sectorsPerTrack;
	uint16_t numberofHeads;
	uint32_t hiddenSectors;
	uint32_t totalSectors_F32;
	uint32_t FATsize_F32;      //count of sectors occupied by one FAT
	uint16_t extFlags;
	uint16_t FSversion;        //0x0000 (defines version 0.0)
	uint32_t rootCluster;      //first cluster of root directory (=2)
	uint16_t FSinfo;           //sector number of FSinfo structure (=1)
	uint16_t BackupBootSector;
	uint8_t  reserved[12];
	uint8_t  driveNumber;
	uint8_t  reserved1;
	uint8_t  bootSignature;
	uint32_t volumeID;
	uint8_t  volumeLabel[11];   //"NO NAME "
	uint8_t  fileSystemType[8]; //"FAT32"
	uint8_t  bootData[420];
	uint16_t bootEndSignature;  //0xaa55
};


//Structure to access FSinfo sector data
struct FSInfo_Structure
{
	uint32_t leadSignature;      //0x41615252
	uint8_t  reserved1[480];
	uint32_t structureSignature; //0x61417272
	uint32_t freeClusterCount;   //initial: 0xffffffff
	uint32_t nextFreeCluster;    //initial: 0xffffffff
	uint8_t  reserved2[12];
	uint32_t trailSignature;     //0xaa550000
};

//Structure to access Directory Entry in the FAT
struct DIR_Structure{
	uint8_t  DIR_name[11];
	uint8_t  DIR_attrib;     //file attributes
	uint8_t  NTreserved;     //always 0
	uint8_t  timeTenth;      //tenths of seconds, set to 0 here
	uint16_t createTime;     //time file was created
	uint16_t createDate;     //date file was created
	uint16_t lastAccessDate;
	uint16_t firstClusterHI; //higher word of the first cluster number
	uint16_t writeTime;      //time of last write
	uint16_t writeDate;      //date of last write
	uint16_t firstClusterLO; //lower word of the first cluster number
	uint32_t fileSize;       //size of file in bytes
};

//Attribute definitions for file/directory
#define ATTR_READ_ONLY     0x01
#define ATTR_HIDDEN        0x02
#define ATTR_SYSTEM        0x04
#define ATTR_VOLUME_ID     0x08
#define ATTR_DIRECTORY     0x10
#define ATTR_ARCHIVE       0x20
#define ATTR_LONG_NAME     0x0F

#define DIR_ENTRY_SIZE     0x32
#define EMPTY              0x00
#define DELETED            0xe5
#define GET         0
#define SET         1
#define READ        0
#define VERIFY      1
#define ADD         0
#define REMOVE      1
#define TOTAL_FREE  1
#define NEXT_FREE   2
#define GET_LIST    0
#define GET_FILE    1
#define DELETE          2

#endif

/*
A FAT32 FAT entry is actually only a 28-bit entry. The high 4 bits of a FAT32 FAT entry are reserved.
The only time that the high 4 bits of FAT32 FAT entries should ever be changed is when the volume is
formatted, at which time the whole 32-bit FAT entry should be zeroed, including the high 4 bits.
A bit more explanation is in order here, because this point about FAT32 FAT entries seems to cause a
great deal of confusion. Basically 32-bit FAT entries are not really 32-bit values; they are only 28-bit
values. For example, all of these 32-bit cluster entry values: 0x10000000, 0xF0000000, and
0x00000000 all indicate that the cluster is FREE, because you ignore the high 4 bits when you read
the cluster entry value. If the 32-bit free cluster value is currently 0x30000000 and you want to mark
this cluster as bad by storing the value 0x0FFFFFF7 in it. Then the 32-bit entry will contain the value
0x3FFFFFF7 when you are done, because you must preserve the high 4 bits when you write in the
0x0FFFFFF7 bad cluster mark.


La FAT tiene forma de tabla organizada en entradas sucesivas de 32 bits. Una entrada para cada clúster del soporte.
Estas entradas permiten conocer el estado de cada uno de estos clústers, es decir saber si son defectuosos o no, si
contienen o no información y si así es, si se corresponden con el último clúster del fichero o si en cambio el fichero
continúa en algún otro clúster. De los 32 bits de cada entrada solo los 28 primeros ( los más bajos ) se utilizan para
almacenar la dirección de clústers, los cuatro superiores están reservados. Algunos valores para las entradas de la FAT son :

0x?0000000 : clúster libre
0x?0000001 : clúster reservado que no se debe utilizar
0x?0000002 hasta 0x?FFFFFEF : clúster usado ( en realidad no es 0x?0000002 sino BPB_RootCluster que casi siempre es 2 ).
                              El valor aquí contenido representa el clúster donde continuaría el directorio o fichero contenido en este clúster.
0x?FFFFFF0 hasta 0x?FFFFFF6 : Valores reservados que no se deben utilizar
0x?FFFFFF7: marca de clúster defectuoso
0x?FFFFFF8: marca de primer clúster (reservado) de la data region el ultimo byte se corresponde con el BPB_Media que suele ser 0xF8
0x?FFFFFFF : Indica que es el ultimo clúster del fichero o directorio es decir que el fichero finaliza en este clúster.

(NOTA: el interrogante ?' se corresponde con los 4 bits superiores que tampoco se utilizan para dirección)
*/
