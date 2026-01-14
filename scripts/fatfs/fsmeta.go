package main

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

