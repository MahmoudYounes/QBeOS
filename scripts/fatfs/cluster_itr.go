package main

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
		currClusIdx:       2,
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

func (ci *ClusterIterator) GetFirstSectorOfCluster(N uint) uint {
  secPerCluster := ci.metadata.SecPerClus
  firstDataSec := ci.GetFirstDataSector()
  return ((N - 2) * secPerCluster) + firstDataSec
}

func (ci *ClusterIterator) NextCluster() Cluster {
	nextClusterIdx := uint(bytesToInt(ci.fat[ci.currClusIdx][:]))
	ci.currClusIdx = int(nextClusterIdx)
	return ci.clusters[ci.currClusIdx]
}

