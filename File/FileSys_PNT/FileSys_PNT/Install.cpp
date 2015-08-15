#include "FileSys.h"

extern SuperBlock_Disk superblock_disk;								//�����еĳ�����
extern SuperBlock_Memory superblock_mem;							//�ڴ��еĳ�����


extern int GetNextInodeNum();
extern int GetNextBlockNum();
extern void MKDIR(char* name);	//����Ŀ¼

extern int FindBlockbyInode(int id, int *a);

void Install();     //��ʼ���ļ�ϵͳ
void Format();		//��ʽ���ļ�ϵͳ

void Install(){
	superblock_disk.Initial();

	//IN DISK
	//������
	//basic infomation
	superblock_disk.DiskSize = DISK_MAXLEN;             //���̴�С
	superblock_mem.DiskSize = DISK_MAXLEN;				//���̴�С
	strcpy(superblock_disk.FileType, "x_mini");			//�ļ�ϵͳ����
	strcpy(superblock_mem.FileType, superblock_disk.FileType);			//�ļ�ϵͳ����


	superblock_disk.freeblocksize_total = DATADISTRICTION_MAXSIZE;   //��������ռ���Ŀ���
	superblock_mem.freeblocksize_total = superblock_disk.freeblocksize_total;   //��������ռ���Ŀ���
	superblock_disk.freeinodesize_total = INODE_MAXSIZE;    //���ɴ��ļ�/Ŀ¼500��
	superblock_mem.freeinodesize_total = superblock_disk.freeinodesize_total;    //���ɴ��ļ�/Ŀ¼500��


	//���п���һ��
	superblock_mem.b_index = superblock_disk.freeblockQ.front().index;
	superblock_mem.fbtr = 0;
	superblock_mem.freeblocksize = FREEBLOCKGROUP_MAXSIZE;
	for (int i = 0; i < FREEBLOCKGROUP_MAXSIZE; i++){
		superblock_mem.FreeBlockNum[i] = superblock_disk.freeblockQ.front().id[i];
	}
	superblock_disk.freeblockQ.pop();

	//����inode�������һ��
	superblock_mem.i_index = superblock_disk.freeinodeQ.front().index;
	superblock_mem.fitr = 0;
	superblock_mem.freeinodesize = FREEBLOCKGROUP_MAXSIZE;
	for (int i = 0; i < FREEBLOCKGROUP_MAXSIZE; i++){
		superblock_mem.FreeInodeNum[i] = superblock_disk.freeinodeQ.front().id[i];
	}
	superblock_disk.freeinodeQ.pop();

	superblock_mem.OpenList.clear();

	//inode�ڵ�������
	for (int i = 0; i < INODE_MAXSIZE; i++){
		inode[i] = INode();
	}
}

void Format(){
	Install();
	for (int i = 0; i < DATADISTRICTION_MAXSIZE; i++)
		data[i] = '#';

	int inode_num_tmp = GetNextInodeNum();						//����inode
	int block_num_tmp = GetNextBlockNum();						//����block
	inode[inode_num_tmp].CreateInode(inode_num_tmp,1,ROOT,DIRFILE,"rrrrrrrrr");     //��inode�ڵ�����������
	inode[inode_num_tmp].i_zone[0] = block_num_tmp;				//inodeָ�������
	Directory root = Directory();
	writeInBlock(block_num_tmp, (char*)&root);					//��ʱĿ¼��Ӧ�������ı�Ϊ��

	strcpy(superblock_mem.CurDir, "/");							//��ǰĿ¼Ϊ��
	superblock_mem.curind = inode_num_tmp;						//�õ���ǰĿ¼��Ӧ��inode���


	//�û�1
	char user1name[16] = "PNT";
	strcpy(superblock_mem.username[0], user1name);
	strcpy(superblock_mem.password[0], "PNT");
	MKDIR(user1name);												//�ڵ�ǰĿ¼��/���´����û�1

	//�û�2
	char user2name[16] = "user2";
	strcpy(superblock_mem.username[1], user2name);
	strcpy(superblock_mem.password[1], "user2");
	MKDIR(user2name);												//�ڵ�ǰĿ¼��/���´����û�2

	//�û�3
	char user3name[16] = "user3";
	strcpy(superblock_mem.username[2], user3name);
	strcpy(superblock_mem.password[2], "user3");
	MKDIR(user3name);												//�ڵ�ǰĿ¼��/���´����û�3
	
}
