package iterator 

import (
	"encoding/binary"
	"fmt"
	"unsafe"

	"qbefat/pkg/entity"
	"qbefat/pkg/constants"
	"qbefat/pkg/archive"
)

type DirEntryItr struct {
	// internal sector
	cluster []byte 

	// current dir index
	currDirIdx int

	// DirEntrySize
	dirEntrySize int
}

func NewDirEntryItr(cluster entity.Cluster) *DirEntryItr {
	cbytes := make([]byte, cluster.NumSectors() * constants.SECTOR_SIZE)
	binary.Encode(cbytes, binary.LittleEndian, cluster)
	/*
  for si := range cluster {
		copy(cbytes[si * SECTOR_SIZE: si * SECTOR_SIZE + 1][:], cluster[si][:])
	}
  */ 
	return &DirEntryItr{
		cluster: cbytes, 
		currDirIdx: -int(unsafe.Sizeof(entity.FatDirEntry{})),
		dirEntrySize: int(unsafe.Sizeof(entity.FatDirEntry{})),
	}
}

func (si *DirEntryItr) reset() {
	si.currDirIdx = -int(unsafe.Sizeof(entity.FatDirEntry{}))	
}

func (si *DirEntryItr) GetNextDirEntry() (*entity.FatDirEntry, error) {
	si.currDirIdx += si.dirEntrySize
	if si.currDirIdx >= len(si.cluster){ return nil, nil }

  buf := si.cluster[si.currDirIdx:si.currDirIdx + si.dirEntrySize]
	return entity.GetFatDirEntry(buf)
}

func (si *DirEntryItr) GetFirstEmptyDirEntryOffset() (int, error) {
  defer si.reset()
	for si.currDirIdx = 0;si.currDirIdx < len(si.cluster); si.currDirIdx += si.dirEntrySize {
		de, err := entity.GetFatDirEntry(si.cluster[si.currDirIdx:si.currDirIdx + si.dirEntrySize])
		if err != nil {
			return 0, err
		}
		
		if de == nil {
			return si.currDirIdx, nil
		}
    
		isEmptyName := archive.IsEmptyFsName(de.Name)

		if isEmptyName {
			return si.currDirIdx, nil
		}

	}
	return 0, fmt.Errorf("could not find empty file in cluster")
}

