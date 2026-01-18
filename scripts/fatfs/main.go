package main

import (
	"errors"
	"fmt"
	"io/fs"
	"log"
	"os"
)

const (
   SECTOR_SIZE = 512
   INIT_FAT_SIZE = 4097
	 IMG_SIZE_BYTES = (3 * 1024 * 1024 * 1024) - 1
)

// Fatal reports a fatal error and exists
func Fatal(errMsg string){
  log.Fatalf("Fatal Error: %s", errMsg)
}

func main(){
	//var directoryPath = flag.String("d", "", "specify the root directory")
  //var bootImagePath = flag.String("b", "", `specify the location of the MBR create. this
  //MBR must have the FAT32 data reserved at the end as they will be overwritten.`)
  //var outputPath = flag.String("o", "qbeos.img", "the output image file")
 
	// TODO: Enable flag parse
	//flag.Parse()
	dp := "/home/myonaiz/repos/QBeOS/iso_root"
	directoryPath := &dp
	bip := "/home/myonaiz/repos/QBeOS/iso_root/bootloader.bin"
	bootImagePath := &bip
	sstg := "/home/myonaiz/repos/QBeOS/iso_root/kloader.bin"
	secondStagePath := &sstg
	op := "/home/myonaiz/repos/QBeOS/scripts/fatfs/QBeOS.hdd"
	outputPath := &op
 
  if _, err := os.Stat(*directoryPath); errors.Is(err, fs.ErrNotExist) {
    Fatal(err.Error())
  }

  if blf, err := os.Stat(*bootImagePath); errors.Is(err, fs.ErrNotExist) {
    Fatal(err.Error())
  } else if blf.Size() > SECTOR_SIZE {
    Fatal(fmt.Sprintf("MBR must be %d bytes or less", SECTOR_SIZE))
  }
 
  fmt.Printf(`going to use directory %s to create a FAT32 img
  with bootable image at %s. will write the output at %s`, *directoryPath,
  *bootImagePath, *outputPath) 
	fmt.Println()

  bootloaderBuffer, err := os.ReadFile(*bootImagePath)
  if err != nil {
    Fatal(err.Error())
  }

  if len(bootloaderBuffer) > SECTOR_SIZE {
    Fatal(fmt.Sprintf("MBR must be %d bytes or less", SECTOR_SIZE)) 
  }

	secondStageBuffer, err := os.ReadFile(*secondStagePath)
  if err != nil {
    Fatal(err.Error())
  }
  secondStageBuffer = padBufferToLength(secondStageBuffer, 16 * SECTOR_SIZE) 

  if len(secondStageBuffer) > 16 * SECTOR_SIZE {
    Fatal(fmt.Sprintf("Second stage bootloader must be %d bytes or less", 16 * SECTOR_SIZE)) 
  }
	
	fileSysImg, err := NewFAT32(
		SectorsCountFromBytes(IMG_SIZE_BYTES),
		bootloaderBuffer,
		secondStageBuffer)
	if err != nil {
		Fatal(fmt.Sprintf("failed to generate fat image: %s", err.Error()))
	}

	if err := fileSysImg.BuildFSFromRoot(dp); err != nil {
		Fatal(fmt.Errorf("failed to build the file system image: %w", err).Error())
	}

	fileSysImg.PrintLS()
  	
	err = fileSysImg.Serialize(*outputPath)
	if err != nil {
		Fatal(err.Error())
	}
}

func padBufferToLength(buf []byte, finalLength uint) []byte {
	nbuf := make([]byte, finalLength)
	copy(nbuf, buf)
	return nbuf
}

func SectorsCountFromBytes(sizeBytes int) int {
	return (sizeBytes + (BYTES_PER_SEC + 1)) / BYTES_PER_SEC
}
