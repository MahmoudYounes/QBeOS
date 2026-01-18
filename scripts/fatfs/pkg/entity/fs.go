package entity

import (
	"fmt"
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
  ReservedSectors [RES_SEC_COUNT]Sector  
	FAT [][]FATEntry  // FAT tables	
  Data []Cluster 
}

// NewFAT32 Creates a new FAT32 object
func NewFAT32(
	diskSizeSectors int,
	bootloaderCode []byte,
  secondStageCode []byte) (*FAT32, error) {

	fat32 := FAT32{} 
   
  secPerClus, ok := MapDiskSzToSecPerClus(diskSizeSectors) 
  if !ok {
    return nil, errors.New("unsupported FAT32 size")
  }
  
	rootEntCnt := 0
  rootDirSectors := (rootEntCnt * 32 + (BYTES_PER_SEC - 1)) / BYTES_PER_SEC
  // Left over sectors after removing reserved sectors and root dir sectors
	tmpVal1 := diskSizeSectors - (RES_SEC_COUNT + rootDirSectors)
	// (256 clusters + 1 FAT) / 2 <- don't quite understand this
  tmpVal2 := (256 * secPerClus + NUM_FATS) / 2;
  FATSizeSectors := (tmpVal1 + (tmpVal2 - 1)) / tmpVal2;
    
	// BPB
	fat32.BPB.BS_jmpBoot[0] = bootloaderCode[0]
	fat32.BPB.BS_jmpBoot[1] = bootloaderCode[1]
	fat32.BPB.BS_jmpBoot[2] = 0x00
 
	copy(fat32.BPB.BS_OEMName[:], []byte("qbeosfat"))

	copy(fat32.BPB.BPB_BytsPerSec[:], shortToBytes(SECTOR_SIZE))
	fat32.BPB.BPB_SecPerClus = byte(secPerClus)
	copy(fat32.BPB.BPB_RsvdSecCnt[:], shortToBytes(int16(RES_SEC_COUNT + 2)))
	fat32.BPB.BPB_NumFATs = byte(NUM_FATS)
	copy(fat32.BPB.BPB_RootEntCnt[:], shortToBytes(0))
	fat32.BPB.BPB_TotSec16 = [2]byte{0, 0}
	fat32.BPB.BPB_Media = 0xf0
	fat32.BPB.BPB_FATSz16 = [2]byte{0, 0}
	fat32.BPB.BPB_SecPerTrk = [2]byte{0, 0}
	fat32.BPB.BPB_NumHeads = [2]byte{0, 0}
  fat32.BPB.BPB_HiddSec = [4]byte{0, 0, 0, 0}
	copy(fat32.BPB.BPB_TotSec32[:], intToBytes(diskSizeSectors))

	// Extended BPB
	copy(fat32.Fat32BPB.BPB_FATSz32[:], intToBytes(FATSizeSectors))
	fat32.Fat32BPB.BPB_ExtFlags[0] = 0x40 // 01000000
	fat32.Fat32BPB.BPB_ExtFlags[1] = 0
	fat32.Fat32BPB.BPB_FSVer[0] = 0
	fat32.Fat32BPB.BPB_FSVer[1] = 0
	copy(fat32.Fat32BPB.BPB_RootClus[:], intToBytes(2))
	copy(fat32.Fat32BPB.BPB_FSInfo[:], shortToBytes(1))
	fat32.Fat32BPB.BPB_BkBootSec = [2]byte{0, 0}
	fat32.Fat32BPB.BPB_Reserved = [12]byte{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	fat32.Fat32BPB.BS_DrvNum[0] = 0x80
	fat32.Fat32BPB.BS_Reserved1[0] = 0x0
	fat32.Fat32BPB.BS_BootSig[0] = 0x29
	fat32.Fat32BPB.BS_VolID = [4]byte{0x0, 0x0, 0x0, 0x0}
	fat32.Fat32BPB.BS_VolLab = [11]byte{'N', 'O', ' ', 'N', 'A', 'M', 'E', ' ', ' ', ' ', ' '}
	copy(fat32.Fat32BPB.BS_FilSysType[:], "FAT32   ")
	
	// BootCode
	if len(bootloaderCode[90:]) >= len(fat32.BootCode){
		return nil, fmt.Errorf("bootload code is bigger than a boot sector")
	}
	copy(fat32.BootCode[:], bootloaderCode[90:])	
	fat32.BootCode[420] = 0x55
	fat32.BootCode[421] = 0xaa
 
	// FSInfo struct
	copy(fat32.FSInfo[0:4], []byte{0x52, 0x52, 0x61, 0x41})
	copy(fat32.FSInfo[484:488], []byte{0x72, 0x72, 0x41, 0x61})
	copy(fat32.FSInfo[488:492], []byte{0xff, 0xff, 0xff, 0xff})
	copy(fat32.FSInfo[492:496], []byte{0xff, 0xff, 0xff, 0xff})
	copy(fat32.FSInfo[508:512], []byte{0x00, 0x00, 0x55, 0xaa})
  
	// Second Stage
	for i := range RES_SEC_COUNT {
		if i * SECTOR_SIZE > len(secondStageCode) { break }
		end := min(i * SECTOR_SIZE + SECTOR_SIZE - 1, len(secondStageCode))
		fat32.ReservedSectors[i] = [512]byte{}
		copy(fat32.ReservedSectors[i][:], secondStageCode[i*SECTOR_SIZE:end])
	}
  
	// FAT
	FATEntriesCount := (FATSizeSectors * SECTOR_SIZE) / 4 
	fat32.FAT = make([][]FATEntry, 2) 
	for idx := range fat32.FAT{
		fat32.FAT[idx] = make([]FATEntry, FATEntriesCount)
		copy(fat32.FAT[idx][0][:], []byte{0xf8,0xff,0xff,0x0f})
		copy(fat32.FAT[idx][1][:], EOC_MARKER)
	} 
  
	// Data
	dataSectors := diskSizeSectors - (RES_SEC_COUNT + 2 + FATSizeSectors * 2)
	dataClusterCount := (dataSectors + secPerClus) / secPerClus
	fmt.Printf("FATSize in Sectors %d", FATSizeSectors)
	fmt.Printf("FAT Entries Count %d\n", FATEntriesCount)
	fmt.Printf("Disk size sectors %d\n", diskSizeSectors)
	fmt.Printf("Data cluster count %d\n", dataClusterCount)
  fat32.Data = make([]Cluster, dataClusterCount)
	for idx := range dataClusterCount{
		fat32.Data[idx] = make([]Sector, secPerClus) 
	}
  fat32.prepareRootDirEntry()	
  return &fat32, nil
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

	resSecCount := bytesToInt(fat.BPB.BPB_RsvdSecCnt[:])
	if resSecCount != len(fat.ReservedSectors) + 2 {
		return errors.New("inconsistent reserved sectors and reserved sectors count")	
	}

	reservedSectorsWritten := 0
	if len(fat.ReservedSectors) > 0 {
		for _, rs := range fat.ReservedSectors {
			buf = make([]byte, SECTOR_SIZE)
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
		buf = make([]byte, SECTOR_SIZE)
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

			if ffat[1] != FATEntry(EOC_MARKER) {
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
			buf = make([]byte, len(clus) * SECTOR_SIZE)
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
  clusterBytes := 2 * uint(fat.BPB.BPB_SecPerClus) * SECTOR_SIZE
	outputImg.Write(make([]byte, clusterBytes))

  
	outputImg.Sync()
	outputImg.Close()

	return nil		
}

