#include "FileSys.h"

///ȫ�ֱ���

char disk[DISK_MAXLEN];					   //ģ�����
char data[DATADISTRICTION_MAXSIZE];        //�����е�������
char memory[MEMORY_MAXLEN];				   //ģ���ڴ�

SuperBlock_Disk superblock_disk;		   //�����еĳ�����
SuperBlock_Memory superblock_mem;		   //�ڴ��еĳ�����

INode inode[INODE_MAXSIZE];				   //Inode�ڵ�������


extern bool getCommand();

void Login();//��½

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
			if (strcmp(superblock_mem.username[i], input_username)==0){		//������Ч���û���������
				strcat(superblock_mem.CurDir, input_username);				//�ı䵱ǰĿ¼
				superblock_mem.curind = i+1;								//��ǰĿ¼��Ӧ��inode���
				superblock_mem.curuserid = i;
				return;
			}
		}
		system("cls");    //����
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