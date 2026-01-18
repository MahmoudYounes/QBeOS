package entity 

// Handy struct to preserve fs metadata
type FsMetadata struct {
	SecPerClus uint
		
	ReservedSectors uint
  
	FatSizeSectors uint
	FatEntriesCount uint
	
	DataClusterCount uint
	RootClusterNum uint
}

