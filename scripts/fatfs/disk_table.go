package main

func MapDiskSzToSecPerClus(diskSizeSec int) (int, bool){
	if diskSizeSec <= 66600 {
 		// disks up to 32.5MB, set 0 to trip an error. should be FAT16
		return 0, false
	} else if diskSizeSec <= 532480 {
		// disks up to 260MB, 5kb clusters
		return 1, true
	} else if diskSizeSec <= 16777216 {
		// disks up to 8 GB, 4kb cluster
		return 8, true
	} else if diskSizeSec <= 33554432 {
		// disks up to 16 GB, 8kb cluster
		return 16, true
	} else if diskSizeSec <= 67108864 {
		// disks up to 32 GB, 16kb cluster
		return 32, true
	}
	// disks greater than 32GB, 32kb cluster
	return 64, true
}
