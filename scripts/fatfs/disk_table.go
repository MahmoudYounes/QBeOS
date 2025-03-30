package main

var DiskSzToSecPerClus = map[int]int{
	66600:      0,  // disks up to 32.5MB, set 0 to trip an error. should be FAT16
	532480:     1,  // disks up to 260MB, 5k clusters
	16777216:   8,  // disks up to 8 GB, 4k cluster
	33554432:   16, // disks up to 16 GB, 8k cluster
	67108864:   32, // disks up to 32 GB, 16k cluster
	0xFFFFFFFF: 64, // disks greater than 32GB, 32k cluster
}
