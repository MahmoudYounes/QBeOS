package manager 

import (
	"encoding/binary"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"unsafe"

)

type Params struct {
	
}

type Manager struct {
	
}

func NewManager(params Params) *Manager {

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
