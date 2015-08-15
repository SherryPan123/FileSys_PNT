#include "FileSys.h"

void freeblockPushToDisk();										//�ڴ��еĿ��п������
void freeinodePushToDisk();										//�ڴ��еĿ���inode�����

void freeblockPushToDisk(){										//�ڴ��еĿ��п������
	FreeList tempold;											//���ڴ��л����Ŀ�����
	tempold.index = superblock_mem.b_index;
	for (int i = FREEBLOCKGROUP_MAXSIZE - superblock_mem.freeblocksize; i < FREEBLOCKGROUP_MAXSIZE; i++){
		tempold.id[i] = superblock_mem.FreeBlockNum[i];
	}
	superblock_disk.freeblockQ.push(tempold);
}

void freeinodePushToDisk(){										//�ڴ��еĿ���inode�����
	FreeList tempold;											//���ڴ��л����Ŀ�����
	tempold.index = superblock_mem.i_index;
	for (int i = FREEBLOCKGROUP_MAXSIZE - superblock_mem.freeinodesize; i < FREEBLOCKGROUP_MAXSIZE; i++){
		tempold.id[i] = superblock_mem.FreeInodeNum[i];
	}
	superblock_disk.freeinodeQ.push(tempold);
}


//�ͷ�inode
void ReleaseInode(int ino){
	int curp = superblock_mem.fitr;
	if (curp<0){   
		//��ʼ����
		freeinodePushToDisk();				//�ڴ��еĿ���inode�����
		superblock_mem.fitr = 49;
		superblock_mem.freeinodesize = 1;
		superblock_mem.FreeInodeNum[superblock_mem.fitr] = ino;
		return;
	}
	superblock_mem.fitr--;
	superblock_mem.freeinodesize++;
	superblock_mem.FreeInodeNum[superblock_mem.fitr] = ino;
}

//�ͷſ�
void ReleaseBlock(int blo){
	int curp = superblock_mem.fbtr;
	if (curp<0){
		//��ʼ����
		freeblockPushToDisk();				//�ڴ��еĿ���inode�����
		superblock_mem.fbtr = 49;
		superblock_mem.freeblocksize = 1;
		superblock_mem.FreeBlockNum[superblock_mem.fbtr] = blo;
		return;
	}
	superblock_mem.fbtr--;
	superblock_mem.freeblocksize++;
	superblock_mem.FreeBlockNum[superblock_mem.fbtr] = blo;
}
