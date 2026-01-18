package main

import (
	"os"
	"fmt"
	"strings"
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

func sameFsNames(fsDirName [11]byte, ostr string) bool {
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

func isEmptyFsName(fsDirName [11]byte) bool {
	for _, b := range fsDirName {
		if b != 0 {
			return false
		}
	}
	return true
}
