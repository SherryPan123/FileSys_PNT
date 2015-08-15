#include "FileSys.h"

void freeblockPushToDisk();										//内存中的空闲块入磁盘
void freeinodePushToDisk();										//内存中的空闲inode入磁盘

void freeblockPushToDisk(){										//内存中的空闲块入磁盘
	FreeList tempold;											//从内存中换出的空闲组
	tempold.index = superblock_mem.b_index;
	for (int i = FREEBLOCKGROUP_MAXSIZE - superblock_mem.freeblocksize; i < FREEBLOCKGROUP_MAXSIZE; i++){
		tempold.id[i] = superblock_mem.FreeBlockNum[i];
	}
	superblock_disk.freeblockQ.push(tempold);
}

void freeinodePushToDisk(){										//内存中的空闲inode入磁盘
	FreeList tempold;											//从内存中换出的空闲组
	tempold.index = superblock_mem.i_index;
	for (int i = FREEBLOCKGROUP_MAXSIZE - superblock_mem.freeinodesize; i < FREEBLOCKGROUP_MAXSIZE; i++){
		tempold.id[i] = superblock_mem.FreeInodeNum[i];
	}
	superblock_disk.freeinodeQ.push(tempold);
}


//释放inode
void ReleaseInode(int ino){
	int curp = superblock_mem.fitr;
	if (curp<0){   
		//开始换出
		freeinodePushToDisk();				//内存中的空闲inode入磁盘
		superblock_mem.fitr = 49;
		superblock_mem.freeinodesize = 1;
		superblock_mem.FreeInodeNum[superblock_mem.fitr] = ino;
		return;
	}
	superblock_mem.fitr--;
	superblock_mem.freeinodesize++;
	superblock_mem.FreeInodeNum[superblock_mem.fitr] = ino;
}

//释放块
void ReleaseBlock(int blo){
	int curp = superblock_mem.fbtr;
	if (curp<0){
		//开始换出
		freeblockPushToDisk();				//内存中的空闲inode入磁盘
		superblock_mem.fbtr = 49;
		superblock_mem.freeblocksize = 1;
		superblock_mem.FreeBlockNum[superblock_mem.fbtr] = blo;
		return;
	}
	superblock_mem.fbtr--;
	superblock_mem.freeblocksize++;
	superblock_mem.FreeBlockNum[superblock_mem.fbtr] = blo;
}
