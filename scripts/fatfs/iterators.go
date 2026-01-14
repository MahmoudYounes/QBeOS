package main

import "unsafe"

type DirEntryItr struct {
	// internal sector
	c []byte 

	// current dir index
	cdi int

	// DirEntrySize
	des int
}

func NewDirEntryItr(cluster Cluster) *DirEntryItr {
	cbytes := make([]byte, cluster.NumSectors() * SECTOR_SIZE)
  for si := range cluster {
		copy(cbytes[si * SECTOR_SIZE: si * SECTOR_SIZE + 1], cluster[si][:])
	}

	return &DirEntryItr{
		c: cbytes, 
		cdi: -int(unsafe.Sizeof(FatDirEntry{})),
		des: -int(unsafe.Sizeof(FatDirEntry{})),
	}
}

func (si *DirEntryItr) GetNextDirEntry() *FatDirEntry {
	si.cdi += si.des
	if si.cdi >= len(si.c){ return nil }

  buf := si.c[si.cdi:si.cdi + si.des]
	de := FatDirEntry{}
	copy(de.Name[:], buf[:len(de.Name)])
	de.Attr = buf[int(unsafe.Offsetof(de.Attr))]
	de.NTRes = buf[int(unsafe.Offsetof(de.NTRes))] 
	de.CreateTimeTenth = buf[int(unsafe.Offsetof(de.CreateTimeTenth))]
	copy(de.CreateTime[:], buf[int(unsafe.Offsetof(de.CreateTime)) : int(unsafe.Offsetof(de.CreateTime) + unsafe.Sizeof(de.CreateTime))])
	copy(de.CreateDate[:], buf[int(unsafe.Offsetof(de.CreateDate)) : int(unsafe.Offsetof(de.CreateDate) + unsafe.Sizeof(de.CreateDate))])
	copy(de.LastAccessDate[:], buf[int(unsafe.Offsetof(de.LastAccessDate)) : int(unsafe.Offsetof(de.LastAccessDate) + unsafe.Sizeof(de.LastAccessDate))])
	copy(de.FirstClusHigh[:], buf[int(unsafe.Offsetof(de.FirstClusHigh)) : int(unsafe.Offsetof(de.FirstClusHigh) + unsafe.Sizeof(de.FirstClusHigh))])
	copy(de.WriteTime[:], buf[int(unsafe.Offsetof(de.WriteTime)) : int(unsafe.Offsetof(de.WriteTime) + unsafe.Sizeof(de.WriteTime))])
	copy(de.WriteDate[:], buf[int(unsafe.Offsetof(de.WriteDate)) : int(unsafe.Offsetof(de.WriteDate) + unsafe.Sizeof(de.WriteDate))])
	copy(de.FirstClustLow[:], buf[int(unsafe.Offsetof(de.FirstClustLow)) : int(unsafe.Offsetof(de.FirstClustLow) + unsafe.Sizeof(de.FirstClustLow))])
	copy(de.FileSize[:], buf[int(unsafe.Offsetof(de.FileSize)) : int(unsafe.Offsetof(de.FileSize) + unsafe.Sizeof(de.FileSize))])
	
	return &de
} 

// Handy struct to preserve fs metadata
type FsMetadata struct {
	SecPerClus uint
	NumFats uint
	ReservedSectors uint
  FatSizeSectors uint
	RootClusterNum uint
}

func NewFsMetadata(fs *FAT32) *FsMetadata {
	rootClusterNum := uint(bytesToInt(fs.Fat32BPB.BPB_RootClus[:]))	
	sectorsPerCluster := uint(fs.BPB.BPB_SecPerClus)	
	fatSizeSectors := uint(bytesToInt((fs.Fat32BPB.BPB_FATSz32[:])))
	rsSecCount := uint(bytesToInt(fs.BPB.BPB_RsvdSecCnt[:]))

	return &FsMetadata{
		SecPerClus:      sectorsPerCluster,
		NumFats:         2,
		ReservedSectors: rsSecCount,
		FatSizeSectors:  fatSizeSectors,
		RootClusterNum:  rootClusterNum,
	}
}

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

