package mappers 

import (
	"encoding/binary"
)

func BytesToInt(numBytes []byte) int{
  var num int	
  b := make([]byte, 4)
	copy(b, numBytes)
	num = int(binary.LittleEndian.Uint32(b))
	return num
}

func IntToBytes(num int) []byte {
  bs := [4]byte{}
  bst := make([]byte, 4)
  binary.LittleEndian.PutUint32(bst, uint32(num))
  copy(bs[:], bst[:4])
	return bs[:]
}

func ShortToBytes(num int16) ([]byte) {
  bs := [2]byte{}
  bst := make([]byte, 2)
  binary.LittleEndian.PutUint16(bst, uint16(num))
  copy(bs[:], bst[:2])
	return bs[:]
}

func SizeGbToBytes(gbs int) uint {
	return uint(gbs << 30)
}
