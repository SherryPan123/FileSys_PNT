#include "FileSys.h"

///全局变量

char disk[DISK_MAXLEN];					   //模拟磁盘
char data[DATADISTRICTION_MAXSIZE];        //磁盘中的数据区
char memory[MEMORY_MAXLEN];				   //模拟内存

SuperBlock_Disk superblock_disk;		   //磁盘中的超级块
SuperBlock_Memory superblock_mem;		   //内存中的超级块

INode inode[INODE_MAXSIZE];				   //Inode节点索引表


extern bool getCommand();

void Login();//登陆

void Login(){
	while (1){
		char input_username[16];
		char input_password[16];
		printf("Login......\n");
		printf("username:");
		scanf("%s", input_username);
		printf("password:");
		scanf("%s", input_password);
		for (int i = 0; i < USER_MAXNUM; i++){
			if (strcmp(superblock_mem.username[i], input_username)==0){		//输入有效的用户名和密码
				strcat(superblock_mem.CurDir, input_username);				//改变当前目录
				superblock_mem.curind = i+1;								//当前目录对应的inode编号
				superblock_mem.curuserid = i;
				return;
			}
		}
		system("cls");    //清屏
		printf("Invalid User..\n");
	}
	
}




int main(){
	printf("Welcome to the filesystem created by Sherry\n\n");
	Format();
	Login();
	system("cls");
	printf("%s>", superblock_mem.CurDir);
	while (getCommand()){
		printf("%s>", superblock_mem.CurDir);
	}
	getchar();
	return 0;
}