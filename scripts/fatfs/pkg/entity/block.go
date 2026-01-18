package entity

type Sector [SECTOR_SIZE]byte

type Cluster []Sector

func (c Cluster) NumSectors() uint{
	return uint(len([]Sector(c)))
}

