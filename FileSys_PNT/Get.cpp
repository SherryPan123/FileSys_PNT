#include "FileSys.h"

extern void freeinodePushToDisk();											//内存中的空闲inode入磁盘
extern void freeblockPushToDisk();											//内存中的空闲块入磁盘

void freeblockPushToMem();											//磁盘中的空闲块入内存
void freeinodePushToMem();											//磁盘中的空闲inode入内存

int GetNextInodeNum();												//得到下个空闲的inode
int GetNextBlockNum();												//得到下个空闲的块号


void freeblockPushToMem(){										//磁盘中的空闲块入内存
	FreeList tempnew = superblock_disk.freeblockQ.front();		 //将进入superblock_mem的一组空闲inode
	superblock_disk.freeblockQ.pop();

	superblock_mem.b_index = tempnew.index;
	for (int i = 0; i < FREEBLOCKGROUP_MAXSIZE; i++){
		superblock_mem.FreeBlockNum[i] = tempnew.id[i];
	}
}



void freeinodePushToMem(){										//磁盘中的空闲inode入内存
	FreeList tempnew = superblock_disk.freeinodeQ.front();		 //将进入superblock_mem的一组空闲inode
	superblock_disk.freeinodeQ.pop();
	superblock_mem.i_index = tempnew.index;
	for (int i = 0; i < FREEBLOCKGROUP_MAXSIZE; i++){
		superblock_mem.FreeInodeNum[i] = tempnew.id[i];
	}
}

int GetNextInodeNum(){					  //得到下个空闲的inode
	int curp = superblock_mem.fitr;       //取出当前空闲inode的指针
	if (curp >= 50){
		//开始换出
		freeinodePushToMem();				//磁盘中的空闲inode入内存
		superblock_mem.fitr = 1;
		superblock_mem.freeinodesize = FREEBLOCKGROUP_MAXSIZE-1;
		return superblock_mem.FreeInodeNum[superblock_mem.fitr-1];

	}
	superblock_mem.fitr++;
	superblock_mem.freeinodesize--;
	return superblock_mem.FreeInodeNum[curp];
}



int GetNextBlockNum(){						//得到下个空闲的块号
	int curp = superblock_mem.fbtr;			//取出当前空闲inode的指针
	if (curp >= FREEBLOCKGROUP_MAXSIZE){
		//开始换出
		freeblockPushToMem();					//磁盘中的空闲块入内存
		superblock_mem.fbtr = 1;
		superblock_mem.freeblocksize = FREEBLOCKGROUP_MAXSIZE - 1;
		return superblock_mem.FreeBlockNum[superblock_mem.fbtr - 1];

	}
	superblock_mem.fbtr++;
	superblock_mem.freeblocksize--;
	return superblock_mem.FreeBlockNum[curp];
}