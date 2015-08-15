#include "FileSys.h"

extern SuperBlock_Disk superblock_disk;								//磁盘中的超级块
extern SuperBlock_Memory superblock_mem;							//内存中的超级块


extern int GetNextInodeNum();
extern int GetNextBlockNum();
extern void MKDIR(char* name);	//创建目录

extern int FindBlockbyInode(int id, int *a);

void Install();     //初始化文件系统
void Format();		//格式化文件系统

void Install(){
	superblock_disk.Initial();

	//IN DISK
	//超级块
	//basic infomation
	superblock_disk.DiskSize = DISK_MAXLEN;             //磁盘大小
	superblock_mem.DiskSize = DISK_MAXLEN;				//磁盘大小
	strcpy(superblock_disk.FileType, "x_mini");			//文件系统类型
	strcpy(superblock_mem.FileType, superblock_disk.FileType);			//文件系统类型


	superblock_disk.freeblocksize_total = DATADISTRICTION_MAXSIZE;   //数据区所占最大的块数
	superblock_mem.freeblocksize_total = superblock_disk.freeblocksize_total;   //数据区所占最大的块数
	superblock_disk.freeinodesize_total = INODE_MAXSIZE;    //最多可存文件/目录500个
	superblock_mem.freeinodesize_total = superblock_disk.freeinodesize_total;    //最多可存文件/目录500个


	//空闲块表第一组
	superblock_mem.b_index = superblock_disk.freeblockQ.front().index;
	superblock_mem.fbtr = 0;
	superblock_mem.freeblocksize = FREEBLOCKGROUP_MAXSIZE;
	for (int i = 0; i < FREEBLOCKGROUP_MAXSIZE; i++){
		superblock_mem.FreeBlockNum[i] = superblock_disk.freeblockQ.front().id[i];
	}
	superblock_disk.freeblockQ.pop();

	//空闲inode索引表第一组
	superblock_mem.i_index = superblock_disk.freeinodeQ.front().index;
	superblock_mem.fitr = 0;
	superblock_mem.freeinodesize = FREEBLOCKGROUP_MAXSIZE;
	for (int i = 0; i < FREEBLOCKGROUP_MAXSIZE; i++){
		superblock_mem.FreeInodeNum[i] = superblock_disk.freeinodeQ.front().id[i];
	}
	superblock_disk.freeinodeQ.pop();

	superblock_mem.OpenList.clear();

	//inode节点索引表
	for (int i = 0; i < INODE_MAXSIZE; i++){
		inode[i] = INode();
	}
}

void Format(){
	Install();
	for (int i = 0; i < DATADISTRICTION_MAXSIZE; i++)
		data[i] = '#';

	int inode_num_tmp = GetNextInodeNum();						//分配inode
	int block_num_tmp = GetNextBlockNum();						//分配block
	inode[inode_num_tmp].CreateInode(inode_num_tmp,1,ROOT,DIRFILE,"rrrrrrrrr");     //在inode节点表中添加新项
	inode[inode_num_tmp].i_zone[0] = block_num_tmp;				//inode指向物理块
	Directory root = Directory();
	writeInBlock(block_num_tmp, (char*)&root);					//此时目录对应的物理块的表为空

	strcpy(superblock_mem.CurDir, "/");							//当前目录为根
	superblock_mem.curind = inode_num_tmp;						//得到当前目录对应的inode编号


	//用户1
	char user1name[16] = "PNT";
	strcpy(superblock_mem.username[0], user1name);
	strcpy(superblock_mem.password[0], "PNT");
	MKDIR(user1name);												//在当前目录（/）下创建用户1

	//用户2
	char user2name[16] = "user2";
	strcpy(superblock_mem.username[1], user2name);
	strcpy(superblock_mem.password[1], "user2");
	MKDIR(user2name);												//在当前目录（/）下创建用户2

	//用户3
	char user3name[16] = "user3";
	strcpy(superblock_mem.username[2], user3name);
	strcpy(superblock_mem.password[2], "user3");
	MKDIR(user3name);												//在当前目录（/）下创建用户3
	
}
