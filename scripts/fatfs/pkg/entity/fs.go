package entity

import (
	"fmt"
	"os"
	"unsafe"
	"encoding/binary"
	"errors"

	"qbefat/pkg/constants"
	"qbefat/pkg/mappers"
)

// 36 bytes
type BPB struct {
  BS_jmpBoot [3]byte 
  BS_OEMName [8]byte
  BPB_BytsPerSec [2]byte // How many bytes per sector
  BPB_SecPerClus byte    // How many sectors per cluster
  BPB_RsvdSecCnt [2]byte // How many sectors are reserved (32 including the BPB & FSInfo)
  BPB_NumFATs byte       // Number of FAT tables
  BPB_RootEntCnt [2]byte // Set to 0 on FAT32
  BPB_TotSec16 [2]byte   // The total number of sectors
  BPB_Media byte
  BPB_FATSz16 [2]byte    // The size of the FAT 16 table 
  BPB_SecPerTrk [2]byte  // Old Disks
  BPB_NumHeads [2]byte   // Old Disks
  BPB_HiddSec [4]byte
  BPB_TotSec32 [4]byte   // Total number of sectors on FAT32
}

type BPB_FAT32 struct {
  BPB_FATSz32 [4]byte  // The size of the FAT 32 table
  BPB_ExtFlags [2]byte
  BPB_FSVer [2]byte
  BPB_RootClus [4]byte // The cluster number that contains the root directory
  BPB_FSInfo [2]byte
  BPB_BkBootSec [2]byte
  BPB_Reserved [12]byte
  BS_DrvNum [1]byte 
  BS_Reserved1 [1]byte
  BS_BootSig [1]byte
  BS_VolID [4]byte
  BS_VolLab [11]byte
  BS_FilSysType [8]byte
}

type FAT32 struct{
	BPB BPB            // 36 bytes
  Fat32BPB BPB_FAT32 // 54 bytes
	BootCode [422]byte
	FSInfo   Sector
  ReservedSectors [constants.RES_SEC_COUNT]Sector  
	FAT [][]FATEntry  // FAT tables	
  Data []Cluster 
}

func (fat *FAT32) Serialize(outputPath string) error {
	outputImg, err := os.OpenFile(outputPath, os.O_CREATE | os.O_RDWR | os.O_TRUNC, 0666)	
	if err != nil {
		return fmt.Errorf("failed to open file: %w", err)
	}
	totalBytesWritten := 0

	buf := make([]byte, unsafe.Sizeof(fat.BPB))
	binary.Encode(buf, binary.LittleEndian, fat.BPB)
	n, err := outputImg.Write(buf)
	if err != nil {
		return fmt.Errorf("failed to write Boot sector: %w", err)
	}	
	totalBytesWritten += n
	
	buf = make([]byte, unsafe.Sizeof(fat.Fat32BPB))
	binary.Encode(buf, binary.LittleEndian, fat.Fat32BPB)
	n, err = outputImg.Write(buf)
	if err != nil {
		return fmt.Errorf("failed to write extended boot sector: %w", err)
	}	
	totalBytesWritten += n

	buf = make([]byte, unsafe.Sizeof(fat.BootCode))
	binary.Encode(buf, binary.LittleEndian, fat.BootCode)
	n, err = outputImg.Write(buf)
	if err != nil {
		return err
	}
	totalBytesWritten += n

	buf = make([]byte, unsafe.Sizeof(fat.FSInfo))
	binary.Encode(buf, binary.LittleEndian, fat.FSInfo)
	n, err = outputImg.Write(buf)
	if err != nil {
		return fmt.Errorf("failed to write fsinfo: %w", err)
	}	
	totalBytesWritten += n

	resSecCount := mappers.BytesToInt(fat.BPB.BPB_RsvdSecCnt[:])
	if resSecCount != len(fat.ReservedSectors) + 2 {
		return errors.New("inconsistent reserved sectors and reserved sectors count")	
	}

	reservedSectorsWritten := 0
	if len(fat.ReservedSectors) > 0 {
		for _, rs := range fat.ReservedSectors {
			buf = make([]byte, constants.SECTOR_SIZE)
			binary.Encode(buf, binary.LittleEndian, rs)
			n, err = outputImg.Write(buf)
			if err != nil {
				return err
			}
			totalBytesWritten += n
			reservedSectorsWritten++
		}
	}

	for range 16 - reservedSectorsWritten {
		buf = make([]byte, constants.SECTOR_SIZE)
		n, err = outputImg.Write(buf)
		if err != nil {
			return err
		}
		totalBytesWritten += n
	} 

	if len(fat.FAT) != 0 {
		for _, ffat := range fat.FAT {
			if ffat[0] != [4]byte{0xf8,0xff,0xff,0x0f} { 
				return fmt.Errorf("incorrect fat signature")
			}

			if ffat[1] != FATEntry(constants.EOC_MARKER) {
				return fmt.Errorf("incorrect fat end of cluster marker")
			}

			buf = make([]byte, int(unsafe.Sizeof(ffat[0])) * len(ffat))
			binary.Encode(buf, binary.LittleEndian, ffat)

			n, err = outputImg.Write(buf)
			if err != nil {
				return err
			}
			totalBytesWritten += n
		}
	}
  	
	if len(fat.Data) != 0 {
		fmt.Printf("copying %d clusters\n", len(fat.Data))
		totalClusters := len(fat.Data)
		for idx, clus := range fat.Data {
			if idx == 0 || idx == 1 {
				continue
			}
			fmt.Printf("copying %d / %d\r", idx, totalClusters)
			buf = make([]byte, len(clus) * constants.SECTOR_SIZE)
			binary.Encode(buf, binary.LittleEndian, clus)

			n, err = outputImg.Write(buf)
			if err != nil {
				return err
			}
			totalBytesWritten += n
		}
		fmt.Println()
	}
  
	// The last two clusters are always ignored
  clusterBytes := 2 * uint(fat.BPB.BPB_SecPerClus) * constants.SECTOR_SIZE
	outputImg.Write(make([]byte, clusterBytes))

  
	outputImg.Sync()
	outputImg.Close()

	return nil		
}

