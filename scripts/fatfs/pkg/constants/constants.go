package constants 

const (
   SECTOR_SIZE = 512
	 IMG_SIZE_BYTES = (3 * 1024 * 1024 * 1024) - 1
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

const (
	BYTES_PER_SEC = 512
	RES_SEC_COUNT = 30
	NUM_FATS = 2
)

var (
	TOOL_NAME = []byte("qbeosfat")

	EOC_MARKER = []byte{0xFF, 0xFF, 0xFF, 0x0F}
	EMT_MARKER = []byte{0x00, 0x00, 0x00, 0x00}
)
