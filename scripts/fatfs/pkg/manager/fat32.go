package manager 

import (
	"errors"
	"fmt"
	"os"
	"path/filepath"
  
	"qbefat/pkg/entity"
	"qbefat/pkg/constants"
	"qbefat/pkg/mappers"
	"qbefat/pkg/iterator"
	"qbefat/pkg/archive"
)


type Manager struct {
	fs *entity.FAT32
	*entity.FsMetadata
	params *entity.FsParams
	writtenClusters int

	
}

func NewManager(params *entity.FsParams) (*Manager, error) {
	//bootloaderCode []byte
  //secondStageCode []byte
  
	fat32 := entity.FAT32{}
	m := Manager{
		fs: &fat32,
		params: params,
		FsMetadata: &entity.FsMetadata{},
		writtenClusters: 0,
	}
	bootloaderCode, secondStageCode, err := m.loadBootBuffers()
	if err != nil {
		return nil, fmt.Errorf("failed to load boot buffers:%w", err)
	}

	secPerClus, ok := mappers.MapDiskSzToSecPerClus(m.params.DiskSizeSectors) 
  if !ok {
    return nil, errors.New("unsupported FAT32 size")
  }
	m.SecPerClus = uint(secPerClus)

  
	m.loadBPB(bootloaderCode)
	m.loadExtendedBPB()
	m.loadBootCode(bootloaderCode)
	m.loadSecondStage(secondStageCode)
	m.loadFats()
	m.loadDataRegion()
  m.prepareRootDirEntry()	
  m.initializeFsInfo()	   
	return &m, nil
}

func (m *Manager) loadBootBuffers() ([]byte, []byte, error) {
	bootloaderBuffer, err := os.ReadFile(m.params.MbrPath)
  if err != nil {
		return nil, nil, fmt.Errorf("failed to read first stage bootloader:%w", err)
  }

  if len(bootloaderBuffer) > constants.SECTOR_SIZE {
    return nil, nil, fmt.Errorf("MBR must be %d bytes or less", constants.SECTOR_SIZE) 
  }

	secondStageBuffer, err := os.ReadFile(m.params.SecondStagePath)
  if err != nil {
		return nil, nil, fmt.Errorf("failed to read second stage bootloader:%w", err)
  }
  secondStageBuffer = archive.PadBufferToLength(secondStageBuffer, 16 * constants.SECTOR_SIZE) 

  if len(secondStageBuffer) > 16 * constants.SECTOR_SIZE {
    return nil, nil, fmt.Errorf("Second stage bootloader must be %d bytes or less",
			16 * constants.SECTOR_SIZE) 
  }

	if len(bootloaderBuffer[90:]) >= len(m.fs.BootCode){
		return nil, nil, fmt.Errorf("bootload code is bigger than a boot sector")
	}


	return bootloaderBuffer, secondStageBuffer, nil
}

func (m *Manager) loadBPB(bootloaderCode []byte) {    
	m.fs.BPB.BS_jmpBoot[0] = bootloaderCode[0]
	m.fs.BPB.BS_jmpBoot[1] = bootloaderCode[1]
	m.fs.BPB.BS_jmpBoot[2] = 0x00
 
	copy(m.fs.BPB.BS_OEMName[:], constants.TOOL_NAME) 

	copy(m.fs.BPB.BPB_BytsPerSec[:], mappers.ShortToBytes(constants.SECTOR_SIZE))
	m.fs.BPB.BPB_SecPerClus = byte(m.SecPerClus)
	copy(m.fs.BPB.BPB_RsvdSecCnt[:], mappers.ShortToBytes(int16(constants.RES_SEC_COUNT + 2)))
	m.fs.BPB.BPB_NumFATs = byte(constants.NUM_FATS)
	copy(m.fs.BPB.BPB_RootEntCnt[:], mappers.ShortToBytes(0))
	m.fs.BPB.BPB_TotSec16 = [2]byte{0, 0}
	m.fs.BPB.BPB_Media = 0xf0
	m.fs.BPB.BPB_FATSz16 = [2]byte{0, 0}
	m.fs.BPB.BPB_SecPerTrk = [2]byte{0, 0}
	m.fs.BPB.BPB_NumHeads = [2]byte{0, 0}
  m.fs.BPB.BPB_HiddSec = [4]byte{0, 0, 0, 0}
	copy(m.fs.BPB.BPB_TotSec32[:], mappers.IntToBytes(m.params.DiskSizeSectors))	
}

func (m *Manager) loadExtendedBPB() {
	rootEntCnt := 0
  rootDirSectors := (rootEntCnt * 32 + (constants.BYTES_PER_SEC - 1)) / constants.BYTES_PER_SEC
  // Left over sectors after removing reserved sectors and root dir sectors
	tmpVal1 := m.params.DiskSizeSectors - (constants.RES_SEC_COUNT + rootDirSectors)
	// (256 clusters + 1 FAT) / 2 <- don't quite understand this
  tmpVal2 := (256 * int(m.SecPerClus) + constants.NUM_FATS) / 2;
  FATSizeSectors := (tmpVal1 + (tmpVal2 - 1)) / tmpVal2;
	m.FatSizeSectors = uint(FATSizeSectors)

	copy(m.fs.Fat32BPB.BPB_FATSz32[:], mappers.IntToBytes(FATSizeSectors))
	m.fs.Fat32BPB.BPB_ExtFlags[0] = 0x40 // 01000000
	m.fs.Fat32BPB.BPB_ExtFlags[1] = 0
	m.fs.Fat32BPB.BPB_FSVer[0] = 0
	m.fs.Fat32BPB.BPB_FSVer[1] = 0
	copy(m.fs.Fat32BPB.BPB_RootClus[:], mappers.IntToBytes(2))
	copy(m.fs.Fat32BPB.BPB_FSInfo[:], mappers.ShortToBytes(1))
	m.fs.Fat32BPB.BPB_BkBootSec = [2]byte{0, 0}
	m.fs.Fat32BPB.BPB_Reserved = [12]byte{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	m.fs.Fat32BPB.BS_DrvNum[0] = 0x80
	m.fs.Fat32BPB.BS_Reserved1[0] = 0x0
	m.fs.Fat32BPB.BS_BootSig[0] = 0x29
	m.fs.Fat32BPB.BS_VolID = [4]byte{0x0, 0x0, 0x0, 0x0}
	m.fs.Fat32BPB.BS_VolLab = [11]byte{'N', 'O', ' ', 'N', 'A', 'M', 'E', ' ', ' ', ' ', ' '}
	copy(m.fs.Fat32BPB.BS_FilSysType[:], "FAT32   ")
}

func (m *Manager) loadBootCode(bootloaderCode []byte) {
	copy(m.fs.BootCode[:], bootloaderCode[90:])	
	m.fs.BootCode[420] = 0x55
	m.fs.BootCode[421] = 0xaa
}

func (m *Manager) loadSecondStage(secondStageCode []byte) {
	for i := range constants.RES_SEC_COUNT {
		if i * constants.SECTOR_SIZE > len(secondStageCode) { break }
		end := min(i * constants.SECTOR_SIZE + constants.SECTOR_SIZE - 1, len(secondStageCode))
		m.fs.ReservedSectors[i] = [512]byte{}
		copy(m.fs.ReservedSectors[i][:], secondStageCode[i*constants.SECTOR_SIZE:end])
	}
}

func (m *Manager) loadFats(){
	FATEntriesCount := (m.FatSizeSectors * constants.SECTOR_SIZE) / 4 
	m.FatEntriesCount = FATEntriesCount
	m.fs.FAT = make([][]entity.FATEntry, 2) 
	for idx := range m.fs.FAT{
		m.fs.FAT[idx] = make([]entity.FATEntry, FATEntriesCount)
		copy(m.fs.FAT[idx][0][:], []byte{0xf8,0xff,0xff,0x0f})
		copy(m.fs.FAT[idx][1][:], constants.EOC_MARKER)
	} 
}

func (m *Manager) loadDataRegion(){
	dataSectors := m.params.DiskSizeSectors - (constants.RES_SEC_COUNT + 2 + int(m.FatSizeSectors) * 2)
	dataClusterCount := (uint(dataSectors) + m.SecPerClus) / m.SecPerClus
	m.DataClusterCount = dataClusterCount

	m.fs.Data = make([]entity.Cluster, dataClusterCount)
	for idx := range dataClusterCount{
		m.fs.Data[idx] = make([]entity.Sector, m.SecPerClus) 
	}
}

func (m *Manager) initializeFsInfo(){
	copy(m.fs.FSInfo[0:4], []byte{0x52, 0x52, 0x61, 0x41})
	copy(m.fs.FSInfo[484:488], []byte{0x72, 0x72, 0x41, 0x61})
	copy(m.fs.FSInfo[488:492], []byte{0xff, 0xff, 0xff, 0xff})
	copy(m.fs.FSInfo[492:496], []byte{0xff, 0xff, 0xff, 0xff})
	copy(m.fs.FSInfo[508:512], []byte{0x00, 0x00, 0x55, 0xaa})
}

func (m *Manager) prepareRootDirEntry() error {
	m.fs.Data[2] = make([]entity.Sector, m.SecPerClus)
	
	err := m.UpdateFAT([]uint{2})
	if err != nil {
		return fmt.Errorf("failed to update FAT entry for root cluster:%w", err)
	}
	return nil
}

func (m *Manager) FirstDataSector() (int, error) {
	reservedSecCnt := mappers.BytesToInt(m.fs.BPB.BPB_RsvdSecCnt[:])
  numFats := int(m.fs.BPB.BPB_NumFATs) 
  fatSize := mappers.BytesToInt(m.fs.Fat32BPB.BPB_FATSz32[:]) 
  return reservedSecCnt + (numFats * fatSize), nil
}

func (m *Manager) GetFatEntryForCluster(ClusterNum int) (int, int, error) {
	fatOffset := ClusterNum * 4
	resSecCount := mappers.BytesToInt(m.fs.BPB.BPB_RsvdSecCnt[:])	
	bytesPerSec := mappers.BytesToInt(m.fs.BPB.BPB_BytsPerSec[:])	
	secNum := resSecCount + (fatOffset / bytesPerSec)
	entryOffset := fatOffset % bytesPerSec
	return secNum, entryOffset, nil
}

func (m *Manager) GetEmptyClusters(clusterCount uint) []uint {
	clusterIndexes := make([]uint, clusterCount)
	clusterPtr := 0
  found := 0
	for idx := 3; idx < len(m.fs.FAT[0]) - 3; idx++ {
		fe := m.fs.FAT[0][idx]

		if found == int(clusterCount) {
			break
		}

		if archive.SameBytes(fe[:], constants.EMT_MARKER){
			found++
			clusterIndexes[clusterPtr] = uint(idx)
			clusterPtr++
		}
	}

	return clusterIndexes
}

func (m *Manager) CopyFileToClusters(path string, emptyClusters []uint) error {	
	fileBytes, err := os.ReadFile(path)
	if err != nil {
		return fmt.Errorf("failed to read %s content: %w", path, err)
	}

	bytesPerCluster := m.SecPerClus * constants.SECTOR_SIZE

	for idx, clusterIdx := range emptyClusters {
		m.fs.Data[clusterIdx] = make([]entity.Sector, m.SecPerClus)
    
		for cc := range m.SecPerClus {
		  fileClusterStart := uint(idx) * bytesPerCluster + cc * constants.SECTOR_SIZE
			fileClusterBoundary := min(fileClusterStart + constants.SECTOR_SIZE, uint(len(fileBytes)))
			if int(fileClusterStart) >= len(fileBytes) {
				break
			}
			
			copy(m.fs.Data[clusterIdx][cc][:], fileBytes[fileClusterStart : fileClusterBoundary]) 
		}
	}
	return nil
}

func (m *Manager) UpdateFAT(clusterChain []uint) error {
	if len(clusterChain) == 0 {
		return fmt.Errorf("can't update the fat with empty cluster chain")
	}
  
	for idx := range len(clusterChain) {	
		clusterNum := clusterChain[idx]
		nextClusterNum := uint(mappers.BytesToInt(constants.EOC_MARKER))
		if idx + 1 < len(clusterChain) {
			nextClusterNum = clusterChain[idx + 1]
		}
		copy(m.fs.FAT[0][clusterNum][:], mappers.IntToBytes(int(nextClusterNum))[:])
		copy(m.fs.FAT[1][clusterNum][:], mappers.IntToBytes(int(nextClusterNum))[:])
	}

	return nil
}

// Locates the cluster that contains the directory
func (m *Manager) GetClusterNumberForPath(dirPath string) (uint, error) {
	rc := mappers.BytesToInt(m.fs.Fat32BPB.BPB_RootClus[:])	
	// return root dir entry
	if dirPath == "." {
		return uint(rc), nil 
	}
 
	parts := filepath.SplitList(dirPath)
	partsPtr := 0
	currCluster := rc
	// Start at the root cluster
	itr := iterator.NewDirEntryItr(m.fs.Data[currCluster])
	
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

		if !archive.SameFsNames(de.Name, parts[partsPtr]){
			continue
		}
    
		// found a dir in path. Let's look for the next dir
		partsPtr++

		// read cluster and reset the iterator if should go to next
		firstClusLow := mappers.BytesToInt(de.FirstClustLow[:])
		firstClusHigh := mappers.BytesToInt(de.FirstClusHigh[:])
		firstClust := (firstClusHigh << 16) | firstClusLow

		if partsPtr == len(parts) {
      return uint(firstClust), nil
		} else {
			itr = iterator.NewDirEntryItr(m.fs.Data[firstClust])	
		}
	}
  
	return 0, nil
}

// Given a cluster, iterate until we find the next empty location to append a dir entry
// and place a dir entry there given the metadata.
func (m *Manager) UpdateClusterWithDirEntry(pDirClusNumber uint, fileClusNumber uint, fsPath, diskPath string) error {
  cluster := m.fs.Data[pDirClusNumber]
	itr := iterator.NewDirEntryItr(cluster)
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
  
	dirEntryBytes := entity.GetDirEntryBytesFromFstat(fileClusNumber, fstat)
	
	sectorIdx := bDirOffset / constants.SECTOR_SIZE 
  offsetInSector := bDirOffset % constants.SECTOR_SIZE 

  copy(m.fs.Data[pDirClusNumber][sectorIdx][offsetInSector:offsetInSector+32], dirEntryBytes)
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
func (m *Manager) UpdateDirEntry(fileFirstCluster uint, fsParent, fsPath, diskPath string) error {
	if fsParent == fsPath {
		return nil
	}

	pDirClusNumber, err := m.GetClusterNumberForPath(fsParent)
	if err != nil {
		return fmt.Errorf("failed to locate sector dir for %s:%w", fsPath, err)
	}

	if err := m.UpdateClusterWithDirEntry(pDirClusNumber, fileFirstCluster, fsPath, diskPath); err != nil {
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
func (m *Manager) Add(parent, fsPath, diskPath string) error {
	// Handle the case for the root dir (.)
	if parent == fsPath {
		return nil
	}

	isDir, err := archive.IsDirPath(diskPath)
  if err != nil {
		return fmt.Errorf("can not figure out if %s is a dir or not: %w", diskPath, err)
	}
  
	if isDir {
		return m.addDir(parent, fsPath, diskPath)
	} 
	
	return m.addFile(parent, fsPath, diskPath)
}

func (m *Manager) addDir(parent, fsPath, diskPath string) error {
	dirClust := m.GetEmptyClusters(1)
  m.UpdateFAT(dirClust)
	m.UpdateDirEntry(dirClust[0], parent, fsPath, diskPath)
	return nil
}

func (m *Manager) addFile(parent, fsPath, diskPath string) error {
	fileSize, err := archive.GetFileSize(diskPath)
	if err != nil {
		return fmt.Errorf("error getting the path %s size: %w", diskPath, err) 
	}

	numberOfClusters := archive.CalculateClustersFromSize(m.SecPerClus, fileSize)

	emptyClusters := m.GetEmptyClusters(numberOfClusters)
  
	if err := m.CopyFileToClusters(diskPath, emptyClusters); err != nil {
		return fmt.Errorf("failed to copy file to clusters: %w", err)
	}


	if err := m.UpdateFAT(emptyClusters); err != nil {
		return fmt.Errorf("failed to update the FAT: %w", err)
	}

	m.writtenClusters += len(emptyClusters)

	if err := m.UpdateDirEntry(emptyClusters[0], parent, fsPath, diskPath); err != nil {
		return fmt.Errorf("failed to update the dir entry: %w", err)
	}
	

	return nil
}

func (m *Manager) BuildFSFromRoot() error {
	return filepath.WalkDir(m.params.RootDirPath, func(diskPath string, dirEntry os.DirEntry, err error) error {
		if err != nil {
			return err	
		}
    
		relativePath, err := filepath.Rel(m.params.RootDirPath, diskPath)
		if err != nil {
      return fmt.Errorf("failed to fetch relative path: %w", err)
		}
    
		fsPath := filepath.Base(relativePath)
		parentPath := filepath.Dir(fsPath)
		fmt.Printf("Adding %s\n", diskPath)
		fmt.Printf("Adding %s with parent %s\n", fsPath, parentPath)
		return m.Add(parentPath, fsPath, diskPath)
	})
}

func (m *Manager) PrintLS(){
	fmt.Println()
	fmt.Println()
	fmt.Printf("iterating through file system.\n")
	fmt.Printf("Root Cluster Number: %v\n", mappers.BytesToInt(m.fs.Fat32BPB.BPB_RootClus[:]))
	clusItr := iterator.NewClusterIterator(m.FsMetadata, m.fs)
	for ;; {
		ne := clusItr.NextCluster() 
		if ne == nil {
			break
		}

		dirEntItr := iterator.NewDirEntryItr(ne)
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

func (m *Manager) Serialize() error {
	// TODO: create backup boot sector at sector 6 here
	// TODO: ensure Fats are synced
	// TODO: update fsinfo here
	return m.fs.Serialize(m.params.OutputPath)
}
