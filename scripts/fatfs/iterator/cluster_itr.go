package iterator 

type ClusterIterator struct {
	fat []FATEntry
	clusters []Cluster
	metadata *FsMetadata
 
	currClusIdx int
}

// A cluster iterator is an iterator that loops on all the clusters that contain
// a directory.
func NewClusterIterator(fs *FAT32) *ClusterIterator {
	meta := NewFsMetadata(fs)
	return &ClusterIterator{
		fat:               fs.FAT[0],
		clusters:           fs.Data,
		metadata: 				 meta,
		currClusIdx:       -1,
	}
}

func (ci *ClusterIterator) Reset(){
	// cluster 0 and 1 are reserved
	ci.currClusIdx = 2
}

func (ci *ClusterIterator) GetFirstDataSector() uint {
	reservedSecCnt := ci.metadata.ReservedSectors
  numFats := ci.metadata.NumFats 
  fatSize := ci.metadata.FatSizeSectors 
  return uint(reservedSecCnt + (numFats * fatSize))
}

func (ci *ClusterIterator) NextCluster() Cluster {
	if ci.currClusIdx == -1 {
		ci.currClusIdx = 2
		return ci.clusters[ci.currClusIdx]
	}

	nextClusterIdx := uint(bytesToInt(ci.fat[ci.currClusIdx][:]))
	if nextClusterIdx == uint(bytesToInt(EOC_MARKER[:])) ||
	   nextClusterIdx == uint(bytesToInt(EMT_MARKER[:])) {
		return nil
	}

	ci.currClusIdx = int(nextClusterIdx)
	return ci.clusters[ci.currClusIdx]
}

