package main

import (
	"bytes"
	"encoding/binary"
	"errors"
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"unsafe"
)

const BYTES_PER_SEC = 512

// 36 bytes
type BPB struct {
  BS_jmpBoot [3]byte 
  BS_OEMName [8]byte
  BPB_BytsPerSec [2]byte
  BPB_SecPerClus [1]byte
  BPB_RsvdSecCnt [2]byte
  BPB_NumFATs [1]byte
  BPB_RootEntCnt [2]byte
  BPB_TotSec16 [2]byte
  BPB_Media [1]byte
  BPB_FATSz16 [2]byte
  BPB_SecPerTrk [2]byte
  BPB_NumHeads [2]byte
  BPB_HiddSec [4]byte
  BPB_TotSec32 [4]byte
}


type BPB_FAT32 struct {
  BPB_FATSz32 [4]byte
  BPB_ExtFlags [2]byte
  BPB_FSVer [2]byte
  BPB_RootClus [4]byte
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

type Sector [512]byte
type FATEntry [4]byte
type RootDirEntry []byte

type FAT32 struct{
  // 36 bytes
	BPB BPB
	// 90 bytes
  Fat32BPB BPB_FAT32

	BootCode [422]byte
  ReservedSectors []Sector
  FAT []FATEntry
  RootDir []RootDirEntry
  Data []Sector
}

// NewFAT32 Creates a new FAT32 object
func NewFAT32(
	diskSize int,
	rootEntCnt int,
	resSecCnt int,
	numFats int,
	bootloaderCode []byte,
  secondStageCode []byte) (*FAT32, error) {
  var fat32 = FAT32{}
  
  secPerClus, ok := DiskSzToSecPerClus[int(diskSize)] 
  if !ok {
    return nil, errors.New("unsupported FAT32 size")
  }
  
  rootDirSectors := (rootEntCnt * 32 + (BYTES_PER_SEC - 1)) / BYTES_PER_SEC
  tmpVal1 := diskSize - (resSecCnt + rootDirSectors)
  tmpVal2 := (256 * secPerClus + numFats) / 2;
  FATSize := (tmpVal1 + (tmpVal2 - 1)) / tmpVal2;
    
	// BPB
	fat32.BPB.BS_jmpBoot[0] = 0xeb
	fat32.BPB.BS_jmpBoot[1] = 0x90
	fat32.BPB.BS_jmpBoot[2] = 0x00
 
	copy(fat32.BPB.BS_OEMName[:], []byte("qbeosfat"))

	fat32.BPB.BPB_BytsPerSec = shortToBytes(SECTOR_SIZE)
	fat32.BPB.BPB_SecPerClus[0] = shortToBytes(int16(secPerClus))[0]
	fat32.BPB.BPB_RsvdSecCnt = shortToBytes(16)
	fat32.BPB.BPB_NumFATs[0] = 2
	fat32.BPB.BPB_RootEntCnt = shortToBytes(0)
	fat32.BPB.BPB_TotSec16 = [2]byte{0, 0}
	fat32.BPB.BPB_Media[0] = 0xf0
	fat32.BPB.BPB_FATSz16 = [2]byte{0, 0}
	fat32.BPB.BPB_SecPerTrk = [2]byte{0, 0}
	fat32.BPB.BPB_NumHeads = [2]byte{0, 0}
  fat32.BPB.BPB_HiddSec = [4]byte{0, 0, 0, 0}
  fat32.BPB.BPB_TotSec32 = intToBytes((diskSize / SECTOR_SIZE) + 1) 

	// Extended BPB
	fat32.Fat32BPB.BPB_FATSz32 = intToBytes(FATSize)
	fat32.Fat32BPB.BPB_ExtFlags[0] = 0x41 // 01000001
	fat32.Fat32BPB.BPB_ExtFlags[1] = 0
	fat32.Fat32BPB.BPB_FSVer[0] = 0
	fat32.Fat32BPB.BPB_FSVer[1] = 0
	fat32.Fat32BPB.BPB_RootClus = intToBytes(2)
	fat32.Fat32BPB.BPB_FSInfo = shortToBytes(1)
	fat32.Fat32BPB.BPB_BkBootSec = [2]byte{0, 0}
	fat32.Fat32BPB.BPB_Reserved = [12]byte{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	fat32.Fat32BPB.BS_DrvNum[0] = 0x80
	fat32.Fat32BPB.BS_Reserved1[0] = 0x0
	fat32.Fat32BPB.BS_BootSig[0] = 0x0
	fat32.Fat32BPB.BS_VolID = [4]byte{0x0, 0x0, 0x0, 0x0}
	fat32.Fat32BPB.BS_VolLab = [11]byte{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	copy(fat32.Fat32BPB.BS_FilSysType[:], "FAT32   ")
	
	// BootCode
	copy(fat32.BootCode[:], bootloaderCode[90:])	
	fat32.BootCode[420] = 0x55
	fat32.BootCode[421] = 0xaa

	fat32.ReservedSectors = make([]Sector, 16)
	copy(fat32.Fat32BPB.BPB_Reserved[:], secondStageCode)

  return &fat32, nil
}

func (fs *FAT32) RootDirSectors() (int, error) {
  rootEntCnt, err := bytesToInt(fs.BPB.BPB_RootEntCnt[:]) 
  if err != nil {
    return 0, err
  }

  bytesPerSec, err := bytesToInt(fs.BPB.BPB_BytsPerSec[:]) 
  if err != nil {
    return 0, err
  }

  return ((rootEntCnt * 32) + (bytesPerSec - 1)) / bytesPerSec, nil 
}

func (fs *FAT32) FirstDataSector() (int, error) {
  reservedSecCnt, err := bytesToInt(fs.BPB.BPB_RsvdSecCnt[:])
  if err != nil {
    return 0, err
  }

  numFats, err := bytesToInt(fs.BPB.BPB_NumFATs[:])
  if err != nil {
    return 0, err
  }

  fatSize, err := bytesToInt(fs.Fat32BPB.BPB_FATSz32[:])
  if err != nil {
    return 0, err
  }

  rootDirSectors, err := fs.RootDirSectors()
  if err != nil {
    return 0, err
  }

  return reservedSecCnt + (numFats * fatSize) + rootDirSectors, nil
}

func (fs *FAT32) FirstSectorOfCluster(N int)(int, error){
  secPerCluster, err := bytesToInt(fs.BPB.BPB_SecPerClus[:])
  if err != nil {
    return 0, err
  }

  firstDataSec, err := fs.FirstDataSector()
  if err != nil {
    return 0, err
  }

  return ((N - 2) * secPerCluster) + firstDataSec, nil
}

func (fat *FAT32) Serialize(outputPath string) error {
	outputImg, err := os.OpenFile(outputPath, os.O_CREATE | os.O_RDWR, 0666)	
	if err != nil {
		return err
	}

	totalBytesWritten := 0


	buf := make([]byte, unsafe.Sizeof(fat.BPB))
	binary.Encode(buf, binary.LittleEndian, fat.BPB)
	
	n, err := outputImg.Write(buf)
	if err != nil {
		return err
	}

	if n != int(unsafe.Sizeof(fat.BPB)) {
		return errors.New(fmt.Sprintf("error writing BPB. wrote %d but should have written %d",
		n, unsafe.Sizeof(fat.BPB)))
	}
	totalBytesWritten += n
	
	buf = make([]byte, unsafe.Sizeof(fat.Fat32BPB))
	binary.Encode(buf, binary.LittleEndian, fat.Fat32BPB)

	n, err = outputImg	.Write(buf)
	if err != nil {
		return err
	}

	if n != int(unsafe.Sizeof(fat.Fat32BPB)){
		return errors.New(fmt.Sprintf("error writing Fat32BPB. wrote %d but should have written %d",
		n, unsafe.Sizeof(fat.Fat32BPB)))	
	}
	totalBytesWritten += n


	buf = make([]byte, unsafe.Sizeof(fat.BootCode))
	binary.Encode(buf, binary.LittleEndian, fat.BootCode)

	n, err = outputImg	.Write(buf)
	if err != nil {
		return err
	}

	if n != int(unsafe.Sizeof(fat.BootCode)){
		return errors.New(fmt.Sprintf("error writing BootCode. wrote %d but should have written %d",
		n, unsafe.Sizeof(fat.BootCode)))	
	}
	totalBytesWritten += n


	if resSecCount, err := bytesToShort(fat.BPB.BPB_RsvdSecCnt[:]); err != nil {
		return err
	} else if resSecCount != len(fat.ReservedSectors){
		return errors.New("inconsistent reserved sectors and reserved sectors count")	
	}

	if len(fat.ReservedSectors) > 0 {
		buf = make([]byte, SECTOR_SIZE * len(fat.ReservedSectors))
		binary.Encode(buf, binary.LittleEndian, fat.ReservedSectors)

		n, err = outputImg	.Write(buf)
		if err != nil {
			return err
		}

		if n != len(fat.ReservedSectors) * SECTOR_SIZE {
			return errors.New(fmt.Sprintf("error writing ReservedSectors. wrote %d but should have written %d",
			n, unsafe.Sizeof(fat.ReservedSectors)))	
		}
		totalBytesWritten += n
	}

	if len(fat.FAT) != 0 {
		buf = make([]byte, int(unsafe.Sizeof(fat.FAT[0])) * len(fat.FAT))
		binary.Encode(buf, binary.LittleEndian, fat.FAT)

		n, err = outputImg	.Write(buf)
		if err != nil {
			return err
		}

		if n != int(unsafe.Sizeof(fat.FAT[0])) * len(fat.FAT){
			return errors.New(fmt.Sprintf("error writing FATEntries. wrote %d but should have written %d",
			n, int(unsafe.Sizeof(fat.FAT[0])) * len(fat.FAT)))	
		}
		totalBytesWritten += n
	}


	if len(fat.RootDir) != 0 {
		buf = make([]byte, int(unsafe.Sizeof(fat.RootDir[0])) * len(fat.RootDir))
		binary.Encode(buf, binary.LittleEndian, fat.RootDir)

		n, err = outputImg	.Write(buf)
		if err != nil {
			return err
		}

		if n != int(unsafe.Sizeof(fat.RootDir[0])) * len(fat.RootDir){
			return errors.New(fmt.Sprintf("error writing RootDirEntries. wrote %d but should have written %d",
			n, int(unsafe.Sizeof(fat.RootDir[0])) * len(fat.RootDir)))	
		}
		totalBytesWritten += n
	}


	if len(fat.Data) != 0 {
		buf = make([]byte, int(unsafe.Sizeof(fat.Data[0])) * len(fat.Data))
		binary.Encode(buf, binary.LittleEndian, fat.Data)

		n, err = outputImg	.Write(buf)
		if err != nil {
			return err
		}

		if n != int(unsafe.Sizeof(fat.Data[0])) * len(fat.Data){
			return errors.New(fmt.Sprintf("error writing Data. wrote %d but should have written %d",
			n, int(unsafe.Sizeof(fat.Data[0])) * len(fat.Data)))	
		}
		totalBytesWritten += n
	}

	totalSecSizeBytes, err := bytesToInt(fat.BPB.BPB_TotSec32[:])	
	if err != nil {
		fmt.Println("failed to get totaly sector size in bytes that should be written")
		return nil
	}

	shouldWrite := totalSecSizeBytes * SECTOR_SIZE
	if shouldWrite != totalBytesWritten{
		fmt.Println("detected drift between shouldWrite and totalWritten.")
		fmt.Printf("should write is %d, totalWritten is %d\n", shouldWrite, totalBytesWritten)
		fmt.Printf("filling in the rest with zeros\n")
	}

	outputImg.Write(make([]byte, shouldWrite - totalBytesWritten))
	outputImg.Sync()
	outputImg.Close()

	return nil		
}

func bytesToShort(numBytes []byte) (int, error){
  var num int16
  err := binary.Read(bytes.NewReader(numBytes), binary.LittleEndian, &num) 
  return int(num), err
}


func bytesToInt(numBytes []byte) (int, error){
  var num int32
  err := binary.Read(bytes.NewReader(numBytes), binary.LittleEndian, &num) 
  return int(num), err
}

func intToBytes(num int) ([4]byte) {
  bs := [4]byte{}
  bst := make([]byte, 4)
  binary.LittleEndian.PutUint32(bst, uint32(num))
  copy(bs[:], bst[:4])
  return bs
}

func shortToBytes(num int16) ([2]byte) {
  bs := [2]byte{}
  bst := make([]byte, 2)
  binary.LittleEndian.PutUint16(bst, uint16(num))
  copy(bs[:], bst[:2])
  return bs
}

// CalculateImageSizeBytes takes a rootDir path and calculates what is the size of
// this directory to set the image size generated.
func CalculateImageSizeBytes(rootDir string) (int64, error){
  size := int64(0)
  err := filepath.WalkDir(rootDir, func(path string, d fs.DirEntry, err error) error {
    fmt.Printf("Adding %s\n", path)
    if err !=nil {
      return err
    }

    if d == nil {
      return errors.New("dir entry is nil")
    }

    entryInfo, err := d.Info() 
    if err != nil {
      return err
    }
    
    size += entryInfo.Size()
    return nil
  })   
  if err != nil{
    return 0, err
  }
	
	for k := range DiskSzToSecPerClus {
		if (int64(k) > size){
			return int64(k), nil 
		}	
	}

  return size, nil 
}

func GetRootEntCnt(dirPath string) (int, error) {
	dirEntries, err := os.ReadDir(dirPath)	
	if err != nil {
		return 0, err
	}

	return len(dirEntries), nil	
}
