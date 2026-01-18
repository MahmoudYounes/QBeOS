package entity 

import (
	"encoding/binary"
	"fmt"
	"os"

	"qbefat/pkg/constants"
	"qbefat/pkg/mappers"
)

type FatDirEntry struct {
  Name [11]byte
	Attr byte
	NTRes byte
	CreateTimeTenth byte
	CreateTime [2]byte
	CreateDate [2]byte 
	LastAccessDate [2]byte
	FirstClusHigh [2]byte
	WriteTime [2]byte
	WriteDate [2]byte
	FirstClustLow [2]byte
	FileSize [4]byte
}

func (fde *FatDirEntry) IsEmpty() bool {
	for _, b := range fde.Name {
		if b != 0 {
			return false
		}
	}
	return true
}

func (fde *FatDirEntry) IsDir() bool {
  return fde.Attr & byte(constants.ATTR_DIRECTORY) != 0 
}

func (fde *FatDirEntry) Print() {
	fmt.Printf("Name: %s\n", string(fde.Name[:])) 
	fmt.Printf("Is Dir: %v\n", fde.IsDir())
	fmt.Println()
}

func GetFatDirEntry(parts []byte) (*FatDirEntry, error) {
	if len(parts) < 32 {
		return nil, fmt.Errorf("passed buffer can't be deserialized to fat dir entry") 
	}
	fde := FatDirEntry{}
	copy(fde.Name[:], parts[0:11])
	fde.Attr = parts[11]
	fde.NTRes = parts[12]
	fde.CreateTimeTenth = parts[13]
	copy(fde.CreateTime[:], parts[14:16])
	copy(fde.CreateDate[:], parts[16:18])
	copy(fde.LastAccessDate[:], parts[18:20])
	copy(fde.FirstClusHigh[:], parts[20:22])
	copy(fde.WriteTime[:], parts[22:24])
	copy(fde.WriteDate[:], parts[24:26])
	copy(fde.FirstClustLow[:], parts[26:28])
	copy(fde.FileSize[:], parts[28:])
	if fde.IsEmpty() {
		return nil, nil
	}

	return &fde, nil
}

func GetDirEntryBytesFromFstat(firstClust uint, fstat os.FileInfo) []byte {
	name := GetFnameBytes(fstat.Name()) 
	fSize := 0
	var attr constants.DirAttr
	if fstat.IsDir(){
		attr = constants.ATTR_DIRECTORY
	} else {
		attr = 0x20
		fSize = int(fstat.Size())
	}
	
	de := FatDirEntry{}
	copy(de.Name[:], name)
	de.FileSize = [4]byte(mappers.IntToBytes(fSize))
	de.Attr = byte(attr)
	copy(de.FirstClusHigh[:], mappers.IntToBytes(int(firstClust))[2:4]) 
	copy(de.FirstClustLow[:], mappers.IntToBytes(int(firstClust))[0:2]) 
	//copy(de.CreateDate[:], 
 
	buf := make([]byte, 32)
	binary.Encode(buf, binary.LittleEndian, de)
	return buf
}

func GetFnameBytes(name string) []byte {
	buf := make([]byte, 11)
	ptr := 0
	extPtr := 0
	for _, ru := range name {
		if ptr >= 8 {
		 break 
		} 
		if ru == '.' {
			extPtr = ptr
			break
		}

		buf[ptr] = byte(ru)
		ptr++
	}

	for ;ptr < 8;ptr++ {
		buf[ptr] = ' '
	}

  if extPtr == 0 {
		for ;extPtr < len(name); extPtr++ {
			if name[extPtr] == '.' { break }
		}
    
		if extPtr == len(name) {
			// no extension found
			buf[8] = 'u'
			buf[9] = 'n'
			buf[10] = 'k'
		} else {
			for i := 1; i <= 3; i++ {
				if extPtr + i >= len(name) {
					panic("Wrong extension detection")
				}
				buf[7 + i] = name[extPtr + i]
			}
		}
	} else {
		for i := 1; i <= 3; i++ {
			if extPtr + i >= len(name) {
				panic("Wrong extension detection")
			}
			buf[7 + i] = name[extPtr + i]
		}
	}
	return buf
}
