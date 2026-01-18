package entity

import (
	"qbefat/pkg/constants"
)

type Sector [constants.SECTOR_SIZE]byte

type Cluster []Sector

func (c Cluster) NumSectors() uint{
	return uint(len([]Sector(c)))
}

