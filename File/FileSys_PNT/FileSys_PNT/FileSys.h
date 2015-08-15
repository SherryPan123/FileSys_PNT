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
#define FREEBLOCKGROUP_MAXSIZE 50     //һ���е������п���
#define BLOCK_SIZE 100				  //ÿ�������Ĵ�С
#define ROOT 0                        //�ļ�����  ��
#define USER1 1						  //�ļ�����  �û�1
#define USER2 2						  //�ļ�����  �û�2
#define USER3 3						  //�ļ�����  �û�3
#define NORMALFILE 2                  //��ͨ�ļ�
#define DIRFILE 1                     //Ŀ¼�ļ�

#define DISK_MAXLEN  8000			  //ģ����̵�������� 
#define MEMORY_MAXLEN   100			  //ģ���ڴ���������  
#define USER_MAXNUM 3				  //�û�����3��


struct FreeList{   //���������  
	int id[50];
	int index;
};

struct OpenFileListItem{		 //�ļ��򿪱�
	char name[16];				 //�ļ�����
	int id;						 //inode���
	time_t m_time;				 //�ϴ��޸�ʱ��
	time_t c_time;				 //����ʱ��
	int cnt;					 //�򿪴���
	OpenFileListItem(char na[16], int i, time_t ct, time_t mt){
		strcpy(name, na);
		id = i;
		c_time = ct;
		m_time = mt;
		cnt = 1;
	}
};


struct SuperBlock_Memory{		 //������in�ڴ�
	int DiskSize;                //�����ܿ���
	char FileType[15];           //�ļ�ϵͳ����
	time_t s_mtime;              //�������ϴα��޸ĵ�ʱ��
	int freeblocksize_total;     //�ܿ��п���
	int freeinodesize_total;     //�ܿ��нڵ���


	//���п�����һ����п������Ϣ
	//���ó�����������п�  �ڴ�
	int b_index;				 //������
	int freeblocksize;           //��ǰ���п���Ŀ��п���
	int FreeBlockNum[50];            //���п�Ŀ��
	int fbtr;                    //ģ��ջָ��

	//����inode�����ڵ�����һ����������ڵ����Ϣ
	//���ó�����������������ڵ�  �ڴ�
	int i_index;				  //inode���
	int freeinodesize;           //��ǰ���������ڵ���Ŀ��нڵ���
	int FreeInodeNum[50];            //���нڵ�Ľڵ��
	int fitr;                    //ģ��ջָ��
	

	char username[3][16];         //�û���
	char password[3][16];		  //����

	char CurDir[150];               //��ǰĿ¼   Ӧ�÷���superblock_mem��
	int curind;						//��ǰĿ¼��inode���
	int curuserid;					//��ǰ�û�id
	
	vector<OpenFileListItem> OpenList;			//�ļ��򿪱�		
};


struct SuperBlock_Disk{			//������in����
	int DiskSize;                //�����ܿ���
	char FileType[15];           //�ļ�ϵͳ����
	time_t s_mtime;              //�������ϴα��޸ĵ�ʱ��
	int freeblocksize_total;     //�ܿ��п���
	int freeinodesize_total;     //�ܿ��нڵ���


	//���п�������п��п������Ϣ
	//���ó�����������п� 
	queue <FreeList> freeblockQ;    //���п������
	

	//����inode�����ڵ������������������ڵ����Ϣ
	//���ó�����������������ڵ� 
	queue <FreeList> freeinodeQ;    //����inode����

	char username[3][15];         //�û���
	char password[3][15];		  //����

	void Initial(){
		////////////////////////////////���п��inode��ʼ����
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

struct INode{				   //�ڵ�������in���̺�in�ڴ�        
	int i_number;              //�����ڵ��

	int file_size;             //��Ӧ�ļ��ĳ���(����*100)
	int file_userid;           //�ļ�������id
	int file_type;             //�ļ����ͣ���ͨ�ļ�/Ŀ¼�ļ�/�����ļ���

	char file_attri[9];           //�ļ���������

	time_t file_mtime;     //�ļ����һ�α��޸ĵ�ʱ��
	time_t file_ctime;     //�ļ�����ʱ��

	int i_zone[10];         //i_zone[0-8]Ϊֱ��������[9]Ϊһ�μ������ ���

	//����Ϊ�ڴ������ӵ�
	char i_flag;  //�ڴ������ڵ�״̬
	int i_count;  //���ü���

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

//Ŀ¼��
struct DirectoryItem{
	int  d_ino;     	         //��Ŀ¼���Ӧ�������ڵ��
	char d_name[16];	         //��Ŀ¼���Ӧ���ļ�������
	DirectoryItem(){
		d_ino = -1;
	}
};

//Ŀ¼
struct Directory{			 //���������Ŀ¼��ÿ��Ĵ�СΪ100
	DirectoryItem it[5];
	Directory(){
		for (int i = 0; i < 5; i++)
			it[i] = DirectoryItem();
	}
};

//�ļ�
struct File{				//����������ļ���ÿ��Ĵ�СΪ100
	char content[100];
	File(){
		for (int i = 0; i < 100; i++)
			content[i] = '\0';
	}
};

//һ�μ��������
struct Index{				//���������һ�μ��������ÿ��Ĵ�СΪ100
	int idx[25];
	Index(){
		memset(idx, -1,sizeof(idx));
	}
};

///ȫ�ֱ���

extern char disk[DISK_MAXLEN];					    //ģ�����
extern char data[DATADISTRICTION_MAXSIZE];			//�����е�������

extern SuperBlock_Disk superblock_disk;				//�����еĳ�����
extern SuperBlock_Memory superblock_mem;			//�ڴ��еĳ�����

extern INode inode[INODE_MAXSIZE];					//Inode�ڵ�������




extern void Format();
extern void Install();

extern int GetNextInodeNum();	//�õ��¸����е�inode
extern int GetNextBlockNum();	//�õ��¸����еĿ��

extern void readInBlock(int blockNum, char *a);
extern void writeInBlock(int blockNum, char *a);

#endif