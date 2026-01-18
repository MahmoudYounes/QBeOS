package main

import (
	"encoding/binary"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"unsafe"

)

const (
	BYTES_PER_SEC = 512
	RES_SEC_COUNT = 30
	NUM_FATS = 2
)

var (
	EOC_MARKER = []byte{0xFF, 0xFF, 0xFF, 0x0F}
	EMT_MARKER = []byte{0x00, 0x00, 0x00, 0x00}

	writtenClusters = 0
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

type Sector [SECTOR_SIZE]byte

type Cluster []Sector

func (c Cluster) NumSectors() uint{
	return uint(len([]Sector(c)))
}


type FATEntry [4]byte

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
	fmt.Printf("Wrote %d clusters\n", writtenClusters)
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

func (fs *FAT32) FirstDataSector() (int, error) {
	reservedSecCnt := bytesToInt(fs.BPB.BPB_RsvdSecCnt[:])
  numFats := int(fs.BPB.BPB_NumFATs) 
  fatSize := bytesToInt(fs.Fat32BPB.BPB_FATSz32[:]) 
  return reservedSecCnt + (numFats * fatSize), nil
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
  found := 0
	for idx := 3; idx < len(fs.FAT[0]) - 3; idx++ {
		fe := fs.FAT[0][idx]

		if found == int(clusterCount) {
			break
		}

		if SameBytes(fe[:], EMT_MARKER){
			found++
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

	bytesPerCluster := meta.SecPerClus * SECTOR_SIZE

	for idx, clusterIdx := range emptyClusters {
		fs.Data[clusterIdx] = make([]Sector, meta.SecPerClus)
    
		for cc := range meta.SecPerClus {
		  fileClusterStart := uint(idx) * bytesPerCluster + cc * SECTOR_SIZE
			fileClusterBoundary := min(fileClusterStart + SECTOR_SIZE, uint(len(fileBytes)))
			if int(fileClusterStart) >= len(fileBytes) {
				break
			}
			
			copy(fs.Data[clusterIdx][cc][:], fileBytes[fileClusterStart : fileClusterBoundary]) 
		}
	}
	return nil
}

func (fs *FAT32) UpdateFAT(clusterChain []uint) error {
	if len(clusterChain) == 0 {
		return fmt.Errorf("can't update the fat with empty cluster chain")
	}
  
	for idx := range len(clusterChain) {	
		clusterNum := clusterChain[idx]
		nextClusterNum := uint(bytesToInt(EOC_MARKER))
		if idx + 1 < len(clusterChain) {
			nextClusterNum = clusterChain[idx + 1]
		}
		copy(fs.FAT[0][clusterNum][:], intToBytes(int(nextClusterNum))[:])
		copy(fs.FAT[1][clusterNum][:], intToBytes(int(nextClusterNum))[:])
	}

	return nil
}

// Locates the cluster that contains the directory
func (fs *FAT32) GetClusterNumberForPath(dirPath string) (uint, error) {
	rc := bytesToInt(fs.Fat32BPB.BPB_RootClus[:])	
	// return root dir entry
	if dirPath == "." {
		return uint(rc), nil 
	}
 
	parts := filepath.SplitList(dirPath)
	partsPtr := 0
	currCluster := rc
	// Start at the root cluster
	itr := NewDirEntryItr(fs.Data[currCluster])
	
	// TODO: Looks like this error needs to be handled
	// Error cases here could be that the path is not reachable
  for ;; {
		de, err := itr.GetNextDirEntry()
		if err != nil {
			panic("can't iterate on de")
		}
		if de == nil {
			break
		}

		if !sameFsNames(de.Name, parts[partsPtr]){
			continue
		}
    
		// found a dir in path. Let's look for the next dir
		partsPtr++

		// read cluster and reset the iterator if should go to next
		firstClusLow := bytesToInt(de.FirstClustLow[:])
		firstClusHigh := bytesToInt(de.FirstClusHigh[:])
		firstClust := (firstClusHigh << 16) | firstClusLow

		if partsPtr == len(parts) {
      return uint(firstClust), nil
		} else {
			itr = NewDirEntryItr(fs.Data[firstClust])	
		}
	}
  
	return 0, nil
}

func (fs *FAT32) prepareRootDirEntry() error {
	meta := NewFsMetadata(fs)
	fs.Data[2] = make([]Sector, meta.SecPerClus)
	
	err := fs.UpdateFAT([]uint{2})
	if err != nil {
		return fmt.Errorf("failed to update FAT entry for root cluster:%w", err)
	}
	return nil
}

// Given a cluster, iterate until we find the next empty location to append a dir entry
// and place a dir entry there given the metadata.
func (fs *FAT32) UpdateClusterWithDirEntry(pDirClusNumber uint, fileClusNumber uint, fsPath, diskPath string) error {
  cluster := fs.Data[pDirClusNumber]
	itr := NewDirEntryItr(cluster)
	bDirOffset, err := itr.GetFirstEmptyDirEntryOffset()
	if err != nil {
		return fmt.Errorf("failed to update cluster %d with dir entry for path %s:%w", pDirClusNumber,
			fsPath, err)
	}

	// Get file metadata
	fstat, err := os.Lstat(diskPath)
	if err != nil {
		return err
	}
  
	dirEntryBytes := GetDirEntryBytesFromFstat(fileClusNumber, fstat)
	
	sectorIdx := bDirOffset / SECTOR_SIZE 
  inSector := bDirOffset % SECTOR_SIZE 


  copy(fs.Data[pDirClusNumber][sectorIdx][inSector:inSector+32], dirEntryBytes)
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
func (fs *FAT32) UpdateDirEntry(fileFirstCluster uint, fsParent, fsPath, diskPath string) error {
	if fsParent == fsPath {
		// Prep root dir entry
		fs.prepareRootDirEntry()
	}

	pDirClusNumber, err := fs.GetClusterNumberForPath(fsParent)
	if err != nil {
		return fmt.Errorf("failed to locate sector dir for %s:%w", fsPath, err)
	}

	if err := fs.UpdateClusterWithDirEntry(pDirClusNumber, fileFirstCluster, fsPath, diskPath); err != nil {
		return fmt.Errorf("failed to update cluster %d with dir entry for path %s:%w", pDirClusNumber,
			fsPath, err) 
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
	// Handle the case for the root dir (.)
	if parent == fsPath {
		return nil
	}

	isDir, err := IsDirPath(diskPath)
  if err != nil {
		return fmt.Errorf("can not figure out if %s is a dir or not: %w", diskPath, err)
	}
  
	if isDir {
		return fs.addDir(parent, fsPath, diskPath)
	} 
	
	return fs.addFile(parent, fsPath, diskPath)
}

func (fs *FAT32) addDir(parent, fsPath, diskPath string) error {
	dirClust := fs.GetEmptyClusters(1)
  fs.UpdateFAT(dirClust)
	fs.UpdateDirEntry(dirClust[0], parent, fsPath, diskPath)
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

	writtenClusters += len(emptyClusters)

	if err := fs.UpdateDirEntry(emptyClusters[0], parent, fsPath, diskPath); err != nil {
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
		fmt.Printf("Adding %s\n", diskPath)
		fmt.Printf("Adding %s with parent %s\n", fsPath, parentPath)
		return fs.Add(parentPath, fsPath, diskPath)
	})
}

func (fs *FAT32) PrintLS(){
	fmt.Println()
	fmt.Println()
	fmt.Printf("iterating through file system. Root Cluster Number: %v\n", bytesToInt(fs.Fat32BPB.BPB_RootClus[:]))
	clusItr := NewClusterIterator(fs)
	for ;; {
		ne := clusItr.NextCluster() 
		if ne == nil {
			break
		}

		dirEntItr := NewDirEntryItr(ne)
		for ;; {
			de, err := dirEntItr.GetNextDirEntry()
      if err != nil {
				fmt.Println(err)
				os.Exit(1)
			}

			if de == nil || de.IsEmpty() {
				break
			}

			de.Print()
		}
	}
	
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
