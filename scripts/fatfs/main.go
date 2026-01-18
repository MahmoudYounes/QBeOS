package main

import (
	"errors"
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"

	"github.com/MahmoudYounes/QBeOS/scripts/fatfs/pkg/constants"

)

type Params struct {
	rootDirPath     string
	mbrPath         string
	secondStagePath string
	outputPath      string
	imgSizeGbs      uint
}

// Fatal reports a fatal error and exists
func Fatal(err error){
  log.Fatalf("Fatal Error: %s", err)
}

func parseCliFlags() *Params {
	var directoryPath = flag.String("d", "", "specify the root directory")
	var bootImagePath = flag.String("a", "", "specify the path of the MBR create")
	var secondStagePath = flag.String("b", "", "specify the location to the second stage bootloader")
  var outputPath = flag.String("o", "qbeos.img", "the output image file")
	var imgSizeGigaBytes = flag.Int("sb", 4, "Specify image size in gigabytes") 
	flag.Parse()

	return &Params{
		rootDirPath:     *directoryPath,
		mbrPath:         *bootImagePath,
		secondStagePath: *secondStagePath,
		outputPath:      *outputPath,
		imgSizeGbs:      uint(*imgSizeGigaBytes),
	}
}

func validateParams(params *Params) error {
	if _, err := os.Stat(params.rootDirPath); errors.Is(err, fs.ErrNotExist) {
    return err
  }

  if blf, err := os.Stat(params.mbrPath); errors.Is(err, fs.ErrNotExist) {
    return err
  } else if blf.Size() > constants.SECTOR_SIZE {
    return fmt.Errorf("MBR must be %d bytes or less", constants.SECTOR_SIZE)
  }
  return nil	
}


func main(){
	p := parseCliFlags()
	
	err := validateParams(p)
	if err != nil {
		Fatal(err)
	}

  
  fmt.Printf("creating Fat32 img from dir %s output at %s\n", p.rootDirPath, p.outputPath) 


  bootloaderBuffer, err := os.ReadFile(p.mbrPath)
  if err != nil {
    Fatal(err.Error())
  }

  if len(bootloaderBuffer) > constants.SECTOR_SIZE {
    Fatal(fmt.Sprintf("MBR must be %d bytes or less", constants.SECTOR_SIZE)) 
  }

	secondStageBuffer, err := os.ReadFile(p.secondStagePath)
  if err != nil {
    Fatal(err.Error())
  }
  secondStageBuffer = padBufferToLength(secondStageBuffer, 16 * constants.SECTOR_SIZE) 

  if len(secondStageBuffer) > 16 * constants.SECTOR_SIZE {
    Fatal(fmt.Sprintf("Second stage bootloader must be %d bytes or less", 16 * constants.SECTOR_SIZE)) 
  }
	
	fileSysImg, err := NewFAT32(
		SectorsCountFromBytes(imgSizeBytes),
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
