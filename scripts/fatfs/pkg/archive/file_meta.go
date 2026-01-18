package archive 

import (
	"os"
	"fmt"
	"strings"

	"qbefat/pkg/constants"
)

func IsDirPath(path string) (bool, error){
	fstat, err := os.Lstat(path)
	if err != nil {
		return false, fmt.Errorf("failed to lstat path %s: %w", path, err) 
	}

	return fstat.IsDir(), nil
}

func GetFileSize(path string) (uint, error) {
	stat, err := os.Lstat(path)
	if err != nil {
		return 0, fmt.Errorf("failed to lstat path %s: %w", path, err)
	}

	return uint(stat.Size()), nil 
}

func SameFsNames(fsDirName [11]byte, ostr string) bool {
	upper := strings.ToUpper(ostr)
	if len(upper) > 8 {
		// TODO: This is an info
		fmt.Println("should not set a fat 32 file name to more than 8 bytes. Taking only the first 8 bytes.")
		upper = upper[0:8]
	}
	// name in FAT is 11 bytes. 8 for name and 3 for extension
  for idx := range len(upper) {
     if fsDirName[idx] != upper[idx]{
				return false
		}
	}
	return true
}

func IsEmptyFsName(fsDirName [11]byte) bool {
	for _, b := range fsDirName {
		if b != 0 {
			return false
		}
	}
	return true
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
	clusterBytes := sectorsPerCluster * constants.SECTOR_SIZE

	return (fileSize + clusterBytes) / clusterBytes
}

func PadBufferToLength(buf []byte, finalLength uint) []byte {
	nbuf := make([]byte, finalLength)
	copy(nbuf, buf)
	return nbuf
}

func SectorsCountFromBytes(sizeBytes int) int {
	return (sizeBytes + (constants.BYTES_PER_SEC + 1)) / constants.BYTES_PER_SEC
}
