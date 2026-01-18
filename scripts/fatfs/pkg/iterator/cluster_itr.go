package iterator 

import (
	"qbefat/pkg/entity"
	"qbefat/pkg/constants"
	"qbefat/pkg/mappers"
)

type ClusterIterator struct {
	fat []entity.FATEntry
	meta *entity.FsMetadata
	clusters []entity.Cluster
 
	currClusIdx int
}

// A cluster iterator is an iterator that loops on all the clusters that contain
// a directory.
func NewClusterIterator(fsMeta *entity.FsMetadata, fs *entity.FAT32) *ClusterIterator {
	return &ClusterIterator{
		fat:               fs.FAT[0],
		clusters:           fs.Data,
		currClusIdx:       -1,
		meta: fsMeta,
	}
}

func (ci *ClusterIterator) Reset(){
	// cluster 0 and 1 are reserved
	ci.currClusIdx = 2
}

func (ci *ClusterIterator) GetFirstDataSector() uint {
	reservedSecCnt := ci.meta.ReservedSectors
  numFats := uint(constants.NUM_FATS)
  fatSize := ci.meta.FatSizeSectors 
  return uint(reservedSecCnt + (numFats * fatSize))
}

func (ci *ClusterIterator) NextCluster() entity.Cluster {
	if ci.currClusIdx == -1 {
		ci.currClusIdx = 2
		return ci.clusters[ci.currClusIdx]
	}

	nextClusterIdx := uint(mappers.BytesToInt(ci.fat[ci.currClusIdx][:]))
	if nextClusterIdx == uint(mappers.BytesToInt(constants.EOC_MARKER[:])) ||
	   nextClusterIdx == uint(mappers.BytesToInt(constants.EMT_MARKER[:])) {
		return nil
	}

	ci.currClusIdx = int(nextClusterIdx)
	return ci.clusters[ci.currClusIdx]
}

