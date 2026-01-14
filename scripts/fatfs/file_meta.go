package main

import (
	"os"
	"fmt"
)

func IsDirPath(path string) (bool, error){
	fstat, err := os.Lstat(path)
	if err != nil {
		return false, fmt.Errorf("failed to lstat path %s: %w", err) 
	}

	return fstat.IsDir(), nil
}

func GetFileSize(path string) (uint, error) {
	stat, err := os.Lstat(path)
	if err != nil {
		return 0, fmt.Errorf("failed to lstat path %s: %w", err)
	}

	return uint(stat.Size()), nil 
}
