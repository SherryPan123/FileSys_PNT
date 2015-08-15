#include "FileSys.h"

void readInBlock(int blockNum, char *a);		//读出磁盘块中的内容到字符串a
void writeInBlock(int blockNum, char *a);		//把字符串a写入磁盘
int FindBlockbyInode(int id, int *a);			//通过inode编号寻找物理块，返回记录关联块号的数组a
int AddBlockbyInode(int id);					//通过inode编号寻找其中空物理块，若找到返回物理块号



void readInBlock(int blockNum, char *a) {
	memcpy(a, data + blockNum * 100, 100);		//读出磁盘块中的内容到字符串a
}

void writeInBlock(int blockNum, char *a) {
	memcpy(data + blockNum * 100, a, 100);		//把字符串a写入磁盘
}

int FindBlockbyInode(int id, int *a){					//通过inode编号寻找物理块，返回记录关联块号的数组a
	int cnt = 0;
	int bl;
	for (int i = 0; i < 9; i++){		//直接索引
		bl = inode[id].i_zone[i];		//bl为块号
		if (bl == -1) return cnt;		
		a[cnt++] = bl;
	}
	bl = inode[id].i_zone[9];			//间接索引
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

int AddBlockbyInode(int id){					//通过inode编号寻找其中空物理块，若找到返回物理块号
	int cnt = 0;
	int bl;
	for (int i = 0; i < 9; i++){		//直接索引
		bl = inode[id].i_zone[i];		//bl为块号
		if (bl == -1){
			bl = inode[id].i_zone[i]=GetNextBlockNum();
			return bl;
		}
		
	}
	bl = inode[id].i_zone[9];

	if (bl == -1){		//间接索引不存在,则创建
		bl = inode[id].i_zone[9] = GetNextBlockNum();	//获得一块存索引表的物理块
		Index b = Index();
		int bl1 = b.idx[0] = GetNextBlockNum();			//索引表的第一块
		writeInBlock(bl, (char*)&b);
		return bl1;
	}
	Index b;
	readInBlock(bl, (char*)&b);      //间接索引对应的块号bl
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