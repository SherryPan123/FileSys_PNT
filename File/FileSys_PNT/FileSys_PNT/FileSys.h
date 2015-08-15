#ifndef _FILESYS_H
#define _FILESYS_H

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <ctime>
#include <queue>
#include <map>

using namespace std;

#define DATADISTRICTION_MAXSIZE 7000
#define INODE_MAXSIZE 500
#define FREEBLOCKGROUP_MAXSIZE 50     //一组中的最大空闲块数
#define BLOCK_SIZE 100				  //每块物理块的大小
#define ROOT 0                        //文件宿主  根
#define USER1 1						  //文件宿主  用户1
#define USER2 2						  //文件宿主  用户2
#define USER3 3						  //文件宿主  用户3
#define NORMALFILE 2                  //普通文件
#define DIRFILE 1                     //目录文件

#define DISK_MAXLEN  8000			  //模拟磁盘的最大容量 
#define MEMORY_MAXLEN   100			  //模拟内存的最大容量  
#define USER_MAXNUM 3				  //用户数量3个


struct FreeList{   //空闲组队列  
	int id[50];
	int index;
};

struct OpenFileListItem{		 //文件打开表
	char name[16];				 //文件名称
	int id;						 //inode编号
	time_t m_time;				 //上次修改时间
	time_t c_time;				 //创建时间
	int cnt;					 //打开次数
	OpenFileListItem(char na[16], int i, time_t ct, time_t mt){
		strcpy(name, na);
		id = i;
		c_time = ct;
		m_time = mt;
		cnt = 1;
	}
};


struct SuperBlock_Memory{		 //超级块in内存
	int DiskSize;                //磁盘总块数
	char FileType[15];           //文件系统类型
	time_t s_mtime;              //超级块上次被修改的时间
	int freeblocksize_total;     //总空闲块数
	int freeinodesize_total;     //总空闲节点数


	//空闲块表，存第一组空闲块组的信息
	//采用成组链分配空闲块  内存
	int b_index;				 //块的组号
	int freeblocksize;           //当前空闲块组的空闲块数
	int FreeBlockNum[50];            //空闲块的块号
	int fbtr;                    //模拟栈指针

	//空闲inode索引节点表，存第一组空闲索引节点的信息
	//采用成组链分配空闲索引节点  内存
	int i_index;				  //inode组号
	int freeinodesize;           //当前空闲索引节点组的空闲节点数
	int FreeInodeNum[50];            //空闲节点的节点号
	int fitr;                    //模拟栈指针
	

	char username[3][16];         //用户名
	char password[3][16];		  //密码

	char CurDir[150];               //当前目录   应该放在superblock_mem中
	int curind;						//当前目录的inode编号
	int curuserid;					//当前用户id
	
	vector<OpenFileListItem> OpenList;			//文件打开表		
};


struct SuperBlock_Disk{			//超级块in磁盘
	int DiskSize;                //磁盘总块数
	char FileType[15];           //文件系统类型
	time_t s_mtime;              //超级块上次被修改的时间
	int freeblocksize_total;     //总空闲块数
	int freeinodesize_total;     //总空闲节点数


	//空闲块表，存所有空闲块组的信息
	//采用成组链分配空闲块 
	queue <FreeList> freeblockQ;    //空闲块组队列
	

	//空闲inode索引节点表，存所有组空闲索引节点的信息
	//采用成组链分配空闲索引节点 
	queue <FreeList> freeinodeQ;    //空闲inode队列

	char username[3][15];         //用户名
	char password[3][15];		  //密码

	void Initial(){
		////////////////////////////////空闲块和inode开始划分
		while (!freeblockQ.empty()) freeblockQ.pop();
		while (!freeinodeQ.empty()) freeinodeQ.pop();

		FreeList temp;
		int cnt = 0;
		for (int i = 0; i < DATADISTRICTION_MAXSIZE; i++){
			temp.index = cnt / FREEBLOCKGROUP_MAXSIZE;
			temp.id[cnt % FREEBLOCKGROUP_MAXSIZE] = i;
			cnt++;
			if (cnt % FREEBLOCKGROUP_MAXSIZE == 0){
				freeblockQ.push(temp);
			}
		}
		cnt = 0;
		for (int i = 0; i < INODE_MAXSIZE; i++){
			temp.index = cnt / FREEBLOCKGROUP_MAXSIZE;
			temp.id[cnt % FREEBLOCKGROUP_MAXSIZE] = i;
			cnt++;
			if (cnt % FREEBLOCKGROUP_MAXSIZE == 0){
				freeinodeQ.push(temp);
			}
		}
	}
};

struct INode{				   //节点索引表in磁盘和in内存        
	int i_number;              //索引节点号

	int file_size;             //对应文件的长度(块数*100)
	int file_userid;           //文件宿主的id
	int file_type;             //文件类型（普通文件/目录文件/特殊文件）

	char file_attri[9];           //文件访问类型

	time_t file_mtime;     //文件最后一次被修改的时间
	time_t file_ctime;     //文件创建时间

	int i_zone[10];         //i_zone[0-8]为直接索引，[9]为一次间接索引 块号

	//以下为内存中增加的
	char i_flag;  //内存索引节点状态
	int i_count;  //引用计数

	INode(){
		i_number = -1;
		memset(i_zone, -1, sizeof(i_zone));
	}

	void CreateInode(int inum,int filesize,int fileuserid,int type,const char fileattri[]){
		i_number = inum;
		file_size = filesize;
		file_userid = fileuserid;
		file_type = type;
		strcpy(file_attri, fileattri);
	}

};

//目录项
struct DirectoryItem{
	int  d_ino;     	         //该目录项对应的索引节点号
	char d_name[16];	         //该目录项对应的文件的名称
	DirectoryItem(){
		d_ino = -1;
	}
};

//目录
struct Directory{			 //物理块存的是目录，每块的大小为100
	DirectoryItem it[5];
	Directory(){
		for (int i = 0; i < 5; i++)
			it[i] = DirectoryItem();
	}
};

//文件
struct File{				//物理块存的是文件，每块的大小为100
	char content[100];
	File(){
		for (int i = 0; i < 100; i++)
			content[i] = '\0';
	}
};

//一次间接索引表
struct Index{				//物理块存的是一次间接索引表，每块的大小为100
	int idx[25];
	Index(){
		memset(idx, -1,sizeof(idx));
	}
};

///全局变量

extern char disk[DISK_MAXLEN];					    //模拟磁盘
extern char data[DATADISTRICTION_MAXSIZE];			//磁盘中的数据区

extern SuperBlock_Disk superblock_disk;				//磁盘中的超级块
extern SuperBlock_Memory superblock_mem;			//内存中的超级块

extern INode inode[INODE_MAXSIZE];					//Inode节点索引表




extern void Format();
extern void Install();

extern int GetNextInodeNum();	//得到下个空闲的inode
extern int GetNextBlockNum();	//得到下个空闲的块号

extern void readInBlock(int blockNum, char *a);
extern void writeInBlock(int blockNum, char *a);

#endif