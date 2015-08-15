#include "FileSys.h"

extern void freeinodePushToDisk();											//�ڴ��еĿ���inode�����
extern void freeblockPushToDisk();											//�ڴ��еĿ��п������

void freeblockPushToMem();											//�����еĿ��п����ڴ�
void freeinodePushToMem();											//�����еĿ���inode���ڴ�

int GetNextInodeNum();												//�õ��¸����е�inode
int GetNextBlockNum();												//�õ��¸����еĿ��


void freeblockPushToMem(){										//�����еĿ��п����ڴ�
	FreeList tempnew = superblock_disk.freeblockQ.front();		 //������superblock_mem��һ�����inode
	superblock_disk.freeblockQ.pop();

	superblock_mem.b_index = tempnew.index;
	for (int i = 0; i < FREEBLOCKGROUP_MAXSIZE; i++){
		superblock_mem.FreeBlockNum[i] = tempnew.id[i];
	}
}



void freeinodePushToMem(){										//�����еĿ���inode���ڴ�
	FreeList tempnew = superblock_disk.freeinodeQ.front();		 //������superblock_mem��һ�����inode
	superblock_disk.freeinodeQ.pop();
	superblock_mem.i_index = tempnew.index;
	for (int i = 0; i < FREEBLOCKGROUP_MAXSIZE; i++){
		superblock_mem.FreeInodeNum[i] = tempnew.id[i];
	}
}

int GetNextInodeNum(){					  //�õ��¸����е�inode
	int curp = superblock_mem.fitr;       //ȡ����ǰ����inode��ָ��
	if (curp >= 50){
		//��ʼ����
		freeinodePushToMem();				//�����еĿ���inode���ڴ�
		superblock_mem.fitr = 1;
		superblock_mem.freeinodesize = FREEBLOCKGROUP_MAXSIZE-1;
		return superblock_mem.FreeInodeNum[superblock_mem.fitr-1];

	}
	superblock_mem.fitr++;
	superblock_mem.freeinodesize--;
	return superblock_mem.FreeInodeNum[curp];
}



int GetNextBlockNum(){						//�õ��¸����еĿ��
	int curp = superblock_mem.fbtr;			//ȡ����ǰ����inode��ָ��
	if (curp >= FREEBLOCKGROUP_MAXSIZE){
		//��ʼ����
		freeblockPushToMem();					//�����еĿ��п����ڴ�
		superblock_mem.fbtr = 1;
		superblock_mem.freeblocksize = FREEBLOCKGROUP_MAXSIZE - 1;
		return superblock_mem.FreeBlockNum[superblock_mem.fbtr - 1];

	}
	superblock_mem.fbtr++;
	superblock_mem.freeblocksize--;
	return superblock_mem.FreeBlockNum[curp];
}