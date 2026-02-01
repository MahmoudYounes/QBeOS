package main

import (
	"errors"
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"

	"qbefat/pkg/manager"
	"qbefat/pkg/constants"
	"qbefat/pkg/entity"

)

// Fatal reports a fatal error and exists
func Fatal(err error){
  log.Fatalf("Fatal Error: %s", err)
}

func parseCliFlags() *entity.FsParams {
	var directoryPath = flag.String("d", "", "specify the root directory")
	var bootImagePath = flag.String("a", "", "specify the path of the MBR create")
	var secondStagePath = flag.String("b", "", "specify the location to the second stage bootloader")
  var outputPath = flag.String("o", "qbeos.img", "the output image file")
	var imgSizeGbs = flag.Int("sb", 4, "Specify image size in gigabytes") 
	flag.Parse()

  diskSizeSectors := (*imgSizeGbs << 30) / constants.SECTOR_SIZE

	return &entity.FsParams{
		RootDirPath:     *directoryPath,
		MbrPath:         *bootImagePath,
		SecondStagePath: *secondStagePath,
		OutputPath:      *outputPath,
		ImgSizeGbs:      uint(*imgSizeGbs),
		DiskSizeSectors: diskSizeSectors,
	}
}

func validateParams(params *entity.FsParams) error {
	if params.DiskSizeSectors == 0 {
		return fmt.Errorf("must specify an image size")
	}

	if len(params.OutputPath) == 0 {
		return fmt.Errorf("must specify an output path")
	}

	// TODO: Validate that the output path is writable by writing a file

	if _, err := os.Stat(params.RootDirPath); errors.Is(err, fs.ErrNotExist) {
		return fmt.Errorf("failed to stat root path: %w", err)
  }

  if blf, err := os.Stat(params.MbrPath); errors.Is(err, fs.ErrNotExist) {
		return fmt.Errorf("failed to stat MBR:%w", err)
  } else if blf.Size() > constants.SECTOR_SIZE {
    return fmt.Errorf("MBR must be %d bytes or less", constants.SECTOR_SIZE)
  }

	if _, err := os.Stat(params.SecondStagePath); errors.Is(err, fs.ErrNotExist) {
		fmt.Println("optional second stage not found:%w", err)
  }

  return nil	
}


func main(){
	p := parseCliFlags()
	
	err := validateParams(p)
	if err != nil {
		Fatal(err)
	}

	// TODO: Log with fs params representation
  
	fsman, err := manager.NewManager(p)
	if err != nil {
		Fatal(fmt.Errorf("failed to generate fat image: %w", err))
	}

	if err := fsman.BuildFSFromRoot(); err != nil {
		Fatal(fmt.Errorf("failed to build the file system image: %w", err))
	}
  	
	err = fsman.Serialize()
	if err != nil {
		Fatal(err)
	}
}


