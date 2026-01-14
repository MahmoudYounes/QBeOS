package main

import (
	"encoding/binary"
	"fmt"
	"os"
	"strings"
)

type DirAttr byte

const (
	ATTR_READ_ONLY DirAttr = 0x01
	ATTR_HIDDEN DirAttr = 0x02
	ATTR_SYSTEM DirAttr = 0x04
	ATTR_VOLUME_ID DirAttr = 0x08
	ATTR_DIRECTORY DirAttr = 0x10
	ATTR_ARCHIVE DirAttr = 0x20
	ATTR_LONG_NAME = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID
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
	return false	
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
	return &fde, nil
}

func GetDirEntryBytesFromFstat(firstClust uint, fstat os.FileInfo) []byte {
	name := strings.ToUpper(fstat.Name())
	if len(name) > 8 {
		name = strings.ToUpper(fstat.Name()[0:8])
	}
	
	fSize := 0
	var attr DirAttr
	if fstat.IsDir(){
		attr = ATTR_DIRECTORY
	} else {
		attr = 0x0
		fSize = int(fstat.Size())
	}
	
	de := FatDirEntry{}
	copy(de.Name[:], name)
	de.FileSize = [4]byte(intToBytes(fSize))
	de.Attr = byte(attr)
	copy(de.FirstClusHigh[:], intToBytes(int(firstClust))[2:4]) 
	copy(de.FirstClustLow[:], intToBytes(int(firstClust))[0:2]) 
 
	buf := make([]byte, 32)
	binary.Encode(buf, binary.LittleEndian, de)
	return buf
}
