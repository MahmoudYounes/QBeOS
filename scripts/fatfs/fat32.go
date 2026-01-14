package main

import (
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"unsafe"
)

const (
	BYTES_PER_SEC = 512
	RES_SEC_COUNT = 32
	NUM_FATS = 2
)

var (
	EOC_MARKER = []byte{0xFF, 0xFF, 0xFF, 0x0F}
	EMT_MARKER = []byte{0x00, 0x00, 0x00, 0x00}
)

type FatDirEntry struct {
  Name [11]byte
	Attr byte
	NTRes byte
	CreateTimeTenth byte
	CreateTime [2]byte
	CreateDate [2]byte 
	LastAccessDate [2]byte
	FirstClusHigh [2]byte
	WriteTime [2]byte
	WriteDate [2]byte
	FirstClustLow [2]byte
	FileSize [4]byte
}

func (fde *FatDirEntry) IsEmpty() bool {
	for _, b := range fde.Name {
		if b != 0 {
			return false
		}
	}
	return true
}

func (fde *FatDirEntry) IsDir() bool {
	return false
	
}

func GetFatDirEntry(parts []byte) (*FatDirEntry, error) {
	if len(parts) < 32 {
		return nil, fmt.Errorf("passed buffer can't be deserialized to fat dir entry") 
	}
	fde := FatDirEntry{}
	copy(fde.Name[:], parts[0:11])
	fde.Attr = parts[11]
	fde.NTRes = parts[12]
	fde.CreateTimeTenth = parts[13]
	copy(fde.CreateTime[:], parts[14:16])
	copy(fde.CreateDate[:], parts[16:18])
	copy(fde.LastAccessDate[:], parts[18:20])
	copy(fde.FirstClusHigh[:], parts[20:22])
	copy(fde.WriteTime[:], parts[22:24])
	copy(fde.WriteDate[:], parts[24:26])
	copy(fde.FirstClustLow[:], parts[26:28])
	copy(fde.FileSize[:], parts[28:])
	return &fde, nil
}

// 36 bytes
type BPB struct {
  BS_jmpBoot [3]byte 
  BS_OEMName [8]byte
  BPB_BytsPerSec [2]byte // How many bytes per sector
  BPB_SecPerClus byte    // How many sectors per cluster
  BPB_RsvdSecCnt [2]byte // How many sectors are reserved (32 including the BPB)
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

type Sector [SECTOR_SIZE]byte

type Cluster []Sector

func (c Cluster) NumSectors() uint{
	return uint(len([]Sector(c)))
}


type FATEntry [4]byte


type FAT32 struct{
	BPB BPB            // 36 bytes
  Fat32BPB BPB_FAT32 // 90 bytes
	BootCode [422]byte 
  ReservedSectors [RES_SEC_COUNT - 1]Sector  // - 1 because of the Boot Sector 
	FAT [][]FATEntry  // FAT tables	
  Data []Cluster 
}

// NewFAT32 Creates a new FAT32 object
func NewFAT32(
  ptr unsafe.Pointer,
	diskSizeSectors int,
	bootloaderCode []byte,
  secondStageCode []byte) (*FAT32, error) {

	fat32 := (*FAT32)(ptr)
   
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
	copy(fat32.BPB.BPB_RsvdSecCnt[:], shortToBytes(int16(RES_SEC_COUNT)))
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
	fat32.Fat32BPB.BPB_ExtFlags[0] = 0x41 // 01000001
	fat32.Fat32BPB.BPB_ExtFlags[1] = 0
	fat32.Fat32BPB.BPB_FSVer[0] = 0
	fat32.Fat32BPB.BPB_FSVer[1] = 0
	copy(fat32.Fat32BPB.BPB_RootClus[:], intToBytes(2))
	copy(fat32.Fat32BPB.BPB_FSInfo[:], shortToBytes(0))
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

	for i := range RES_SEC_COUNT - 1 {
		if i * SECTOR_SIZE > len(secondStageCode) { break }
		end := min(i * SECTOR_SIZE + SECTOR_SIZE - 1, len(secondStageCode))
		fat32.ReservedSectors[i] = [512]byte{}
		copy(fat32.ReservedSectors[i][:], secondStageCode[i*SECTOR_SIZE:end])
	}

	FATEntriesCount := (FATSizeSectors * SECTOR_SIZE) / 4 
	fat32.FAT = make([][]FATEntry, 2) 
	for idx := range fat32.FAT{
		fat32.FAT[idx] = make([]FATEntry, FATEntriesCount)
		fat32.FAT[idx][0][3] = fat32.BPB.BPB_Media
		copy(fat32.FAT[idx][1][:], EOC_MARKER)
	} 
  
  fat32.Data = make([]Cluster, FATEntriesCount)
  return fat32, nil
}

func (fs *FAT32) FirstDataSector() (int, error) {
	reservedSecCnt := bytesToInt(fs.BPB.BPB_RsvdSecCnt[:])
  numFats := int(fs.BPB.BPB_NumFATs) 
  fatSize := bytesToInt(fs.Fat32BPB.BPB_FATSz32[:]) 
  return reservedSecCnt + (numFats * fatSize), nil
}

// Return the index of the first sector in cluster number N
func (fs *FAT32) GetFirstSectorOfCluster(N int)(int, error){
  secPerCluster := int(fs.BPB.BPB_SecPerClus)
 
  firstDataSec, err := fs.FirstDataSector()
  if err != nil {
		return 0, fmt.Errorf("failed to get first data sector: %w", err)
  }

  return ((N - 2) * secPerCluster) + firstDataSec, nil
}

func (fs *FAT32) GetFatEntryForCluster(ClusterNum int) (int, int, error) {
	fatOffset := ClusterNum * 4
	resSecCount := bytesToInt(fs.BPB.BPB_RsvdSecCnt[:])	
	bytesPerSec := bytesToInt(fs.BPB.BPB_BytsPerSec[:])	
	secNum := resSecCount + (fatOffset / bytesPerSec)
	entryOffset := fatOffset % bytesPerSec
	return secNum, entryOffset, nil
}

func (fs *FAT32) GetEmptyClusters(clusterCount uint) []uint {
	clusterIndexes := make([]uint, clusterCount)
	clusterPtr := 0

	for idx, fe := range fs.FAT[0]{
		if len(clusterIndexes) == int(clusterCount) {
			break
		}

		if SameBytes(fe[:], EMT_MARKER){
			clusterIndexes[clusterPtr] = uint(idx)
			clusterPtr++
		}
	}

	return clusterIndexes
}

func (fs *FAT32) CopyFileToClusters(path string, emptyClusters []uint) error {
	meta := NewFsMetadata(fs)
		
	fileBytes, err := os.ReadFile(path)
	if err != nil {
		return fmt.Errorf("failed to read %s content: %w", path, err)
	}

	fmt.Printf("copying %s with size %d bytes in %d cluster(s)\n", path, len(fileBytes), len(emptyClusters))
	bytesPerCluster := meta.SecPerClus * SECTOR_SIZE

	for idx := range emptyClusters {
		fs.Data[idx] = make([]Sector, meta.SecPerClus)
    
		for cc := range int(meta.SecPerClus) {
		  fileClusterStart := idx * int(bytesPerCluster) + cc * SECTOR_SIZE
			fileClusterBoundary := min(idx * int(bytesPerCluster) + cc * SECTOR_SIZE + SECTOR_SIZE, len(fileBytes))
			if fileClusterStart >= len(fileBytes) {
				break
			}
			
			copy(fs.Data[idx][cc][:], fileBytes[fileClusterStart : fileClusterBoundary]) 
		}
	}
	return nil
}

func (fs *FAT32) UpdateFAT(clusterChain []uint) error {
	if len(clusterChain) == 0 {
		return fmt.Errorf("can't update the fat with empty cluster chain")
	}

	for idx := range clusterChain {
		if idx == len(clusterChain) - 1 {
			copy(fs.FAT[0][idx][:], EOC_MARKER)
		}
		copy(fs.FAT[0][idx][:], intToBytes(int(clusterChain[idx + 1]))[:])
	}
	return nil
}

func sameFsNames(fsDirName [11]byte, ostr string) bool {
	upper := strings.ToUpper(ostr)
	if len(upper) > 8 {
		// TODO: This is an info
		fmt.Println("should not set a fat 32 file name to more than 8 bytes. Taking only the first 8 bytes.")
		upper = upper[0:8]
	}
	// name in FAT is 11 bytes. 8 for name and 3 for extension
  for idx := range len(upper) {
     if fsDirName[idx] != upper[idx]{
				return false
		}
	}
	return true
} 

// Locates the cluster that contains the directory
func (fs *FAT32) GetClusterNumberForPath(dirPath string) (uint, error) {
	rc := bytesToInt(fs.Fat32BPB.BPB_RootClus[:])
	rs, err := fs.GetFirstSectorOfCluster(rc)
	if err != nil {
		return 0, fmt.Errorf("failed to get first sector of cluster: %w", err)
	}

	// return root dir entry
	if dirPath == "." {
		return uint(rs), nil 
	}
 
	parts := filepath.SplitList(dirPath)
	partsPtr := 0
	currCluster := rc
	// Start at the root cluster
	itr := NewDirEntryItr(fs.Data[currCluster])
  for de := itr.GetNextDirEntry(); de != nil; {
		if !sameFsNames(de.Name, parts[partsPtr]){
			continue
		}
		
		// read cluster and reset the iterator

	}
	
  
	return 0, nil
}

func (fs *FAT32) prepareRootDirEntry() error {
	_ = NewFsMetadata(fs)	
	return nil
}

/*
 * Given a path that is added to the FS and the FAT, this method
 * updates the dir entry with this path.
 * 
 * To do this:
 * - Handle the case for doing this to the root dir entry. If not,
 * 
 */
func (fs *FAT32) UpdateDirEntry(fsParent, fsPath, diskPath string) error {
	fmt.Printf("going to update dir %s that contains %s\n", fsParent, fsPath)
	if fsParent == fsPath {
		// Prep root dir entry
		fs.prepareRootDirEntry()
	}

	_, err := fs.GetClusterNumberForPath(fsParent)
	if err != nil {
		return fmt.Errorf("failed to locate sector dir for %s:%w", fsPath, err)
	}
	return nil	
}

/*
 * Adds a file or dir to the FAT fs.
 * To add a file we need to:
 * - First we need to find n number of clusters that are empty. 
 * - We copy the file into those clusters
 * - We update the FAT entries
 * - We update the dir enteries in the dir containing the file
 * 
 * TODO: Next task on your plate is to handle directory vs file addition
 */
func (fs *FAT32) Add(parent, fsPath, diskPath string) error {	
	isDir, err := IsDirPath(diskPath)
  if err != nil {
		return fmt.Errorf("can not figure out if %s is a dir or not: %w", diskPath, err)
	}
  
	if isDir {
		return fs.addDir(parent, fsPath, diskPath)
	} 
	
	return fs. addFile(parent, fsPath, diskPath)
}

func (fs *FAT32) addDir(parent, fsPath, diskPath string) error {
	dirClust := fs.GetEmptyClusters(1)
  fs.UpdateFAT(dirClust)
	fs.UpdateDirEntry(parent, fsPath, diskPath)
	return nil
}

func (fs *FAT32) addFile(parent, fsPath, diskPath string) error {
	meta := NewFsMetadata(fs)	
	fileSize, err := GetFileSize(diskPath)
	if err != nil {
		return fmt.Errorf("error getting the path %s size: %w", diskPath, err) 
	}

	numberOfClusters := CalculateClustersFromSize(meta.SecPerClus, fileSize)

	emptyClusters := fs.GetEmptyClusters(numberOfClusters)
  
	if err := fs.CopyFileToClusters(diskPath, emptyClusters); err != nil {
		return fmt.Errorf("failed to copy file to clusters: %w", err)
	}


	if err := fs.UpdateFAT(emptyClusters); err != nil {
		return fmt.Errorf("failed to update the FAT: %w", err)
	}

	if err := fs.UpdateDirEntry(parent, fsPath, diskPath); err != nil {
		return fmt.Errorf("failed to update the dir entry: %w", err)
	}

	return nil
}

func (fs *FAT32) BuildFSFromRoot(rootPath string) error {
	return filepath.WalkDir(rootPath, func(diskPath string, dirEntry os.DirEntry, err error) error {
		if err != nil {
			return err	
		}
    
		relativePath, err := filepath.Rel(rootPath, diskPath)
		if err != nil {
      return fmt.Errorf("failed to fetch relative path: %w", err)
		}
    
		fsPath := filepath.Base(relativePath)
		parentPath := filepath.Dir(fsPath)
		fmt.Println(fmt.Sprintf("Adding %s", diskPath))
		fmt.Println(fmt.Sprintf("Adding %s with parent %s", fsPath, parentPath))
		return fs.Add(parentPath, fsPath, diskPath)
	})
}

func SameBytes(buf1 []byte, buf2 []byte) bool {
	if len(buf1) != len(buf2){
		return false
	}

	for idx, b := range buf1{
		if b != buf2[idx] {
			return false
		}
	}

	return true
}

func CalculateClustersFromSize(sectorsPerCluster, fileSize uint) uint {
	clusterBytes := sectorsPerCluster * SECTOR_SIZE

	return (fileSize + clusterBytes) / clusterBytes
}


