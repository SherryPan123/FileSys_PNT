#include "FileSys.h"

void readInBlock(int blockNum, char *a);		//�������̿��е����ݵ��ַ���a
void writeInBlock(int blockNum, char *a);		//���ַ���aд�����
int FindBlockbyInode(int id, int *a);			//ͨ��inode���Ѱ������飬���ؼ�¼������ŵ�����a
int AddBlockbyInode(int id);					//ͨ��inode���Ѱ�����п�����飬���ҵ�����������



void readInBlock(int blockNum, char *a) {
	memcpy(a, data + blockNum * 100, 100);		//�������̿��е����ݵ��ַ���a
}

void writeInBlock(int blockNum, char *a) {
	memcpy(data + blockNum * 100, a, 100);		//���ַ���aд�����
}

int FindBlockbyInode(int id, int *a){					//ͨ��inode���Ѱ������飬���ؼ�¼������ŵ�����a
	int cnt = 0;
	int bl;
	for (int i = 0; i < 9; i++){		//ֱ������
		bl = inode[id].i_zone[i];		//blΪ���
		if (bl == -1) return cnt;		
		a[cnt++] = bl;
	}
	bl = inode[id].i_zone[9];			//�������
	if (bl == -1) return cnt;
	Index b;
	readInBlock(bl, (char*)&b);
	for (int i = 0; i < 25; i++){
		int bl1 = b.idx[i];
		if (bl1 == -1) return cnt;
		else a[cnt++] = bl1;
	}
	return cnt;
}

int AddBlockbyInode(int id){					//ͨ��inode���Ѱ�����п�����飬���ҵ�����������
	int cnt = 0;
	int bl;
	for (int i = 0; i < 9; i++){		//ֱ������
		bl = inode[id].i_zone[i];		//blΪ���
		if (bl == -1){
			bl = inode[id].i_zone[i]=GetNextBlockNum();
			return bl;
		}
		
	}
	bl = inode[id].i_zone[9];

	if (bl == -1){		//�������������,�򴴽�
		bl = inode[id].i_zone[9] = GetNextBlockNum();	//���һ���������������
		Index b = Index();
		int bl1 = b.idx[0] = GetNextBlockNum();			//������ĵ�һ��
		writeInBlock(bl, (char*)&b);
		return bl1;
	}
	Index b;
	readInBlock(bl, (char*)&b);      //���������Ӧ�Ŀ��bl
	for (int i = 0; i < 25; i++){
		if (b.idx[i] == -1){
			int bl1;
			bl1 = b.idx[i] = GetNextBlockNum();
			writeInBlock(bl, (char*)&b);
			return bl1;
		}
	}
	printf("Cannot Add more Blocks\n");
	return -1;
}