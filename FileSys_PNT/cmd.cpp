#include "FileSys.h"

const char cmd[20][10] = {"ls","cd","mkdir","rmdir","mk","rm","rename","open",
"close","read","write","chmod","format","exit","help","look"};

extern void Login();
extern void LS();					//���ļ���Ŀ¼
extern void CD(char* abspath);		//���ĵ�ǰĿ¼
extern void MKDIR(char* name);		//����Ŀ¼
extern void RMDIR(char* name);		//�Ƴ�Ŀ¼(����Ŀ¼Ϊ��)
extern void MK(char* name);			//����Ŀ¼����ʼʱ����һ�������
extern void RM(char* name);			//�Ƴ��ļ�(����Ŀ¼Ϊ��)
extern void RENAME(char* exname,char* name);//������
extern void Format();				//��ʽ������
extern void HELP();					//�ṩ����
extern void WRITE(char* name);		//д�ļ�
extern void READ(char* name);		//���ļ�
extern void CHMOD(char* name,char* mod);//�޸�Ȩ��
extern void OPEN(char* name);		//���ļ�
extern void CLOSE(char* name);		//�ر��ļ�
extern void LOOKATOPENFILE();		//�鿴�ڴ����ļ��򿪱�

void convertToAbs(char dir[], char* abspath);		//�������dirת���ɾ���·��
bool getCommand();									//������ӿ�


void convertToAbs(char dir[], char* abspath){		//�������dirת���ɾ���·��
	if (dir[0] == '/') strcpy(abspath, dir);		//dirΪ����·��
	else{
		strcpy(abspath, superblock_mem.CurDir);
		strcat(abspath, "/");
		strcat(abspath, dir);
	}
}

bool getCommand(){
	char order[10];
	scanf("%s", order); //��ȡ����
	if (strcmp(order, cmd[13]) == 0) return false;   //��������

	if (strcmp(order, cmd[0]) == 0){				//�е�ǰĿ¼
		LS();
		return true;
	}
	else if (strcmp(order, cmd[1]) == 0){			//�ı�Ŀ¼
		char dir[16];
		scanf("%s", dir);							//��ȡĿ¼����
		if (dir == "") return true;					//������Ч
		char abspath[200];
		convertToAbs(dir, abspath);					//ת�ɾ���·��
		CD(abspath);
	}
	else if (strcmp(order, cmd[2]) == 0){			//����Ŀ¼
		char dir[16];
		scanf("%s", dir);							//��Ŀ¼��
		if (strcmp(dir, "") == 0){
			printf("Fail to Create Directory\n");
			return true;
		}
		MKDIR(dir);									//����Ŀ¼
		return true;
	}
	else if (strcmp(order, cmd[3]) == 0){			//ɾ��Ŀ¼
		char dir[16];
		scanf("%s", dir);							//��Ŀ¼��
		if (strcmp(dir, "") == 0){
			printf("Fail to Delete Directory\n");
			return true;
		}
		RMDIR(dir);									//ɾ��Ŀ¼
		return true;
	}
	else if (strcmp(order, cmd[4]) == 0){			//�����ļ�
		char filename[16];
		scanf("%s", filename);						//���ļ���
		if (strcmp(filename, "") == 0){
			printf("Fail to Create File\n");
			return true;
		}
		MK(filename);								//�����ļ�
		return true;
	}
	else if (strcmp(order, cmd[5]) == 0){			//ɾ���ļ�
		char filename[16];
		scanf("%s", filename);						//���ļ���
		if (strcmp(filename, "") == 0){
			printf("Fail to Delete File\n");
			return true;
		}
		RM(filename);								//ɾ���ļ�
		return true;
	}
	else if (strcmp(order, cmd[6]) == 0){			//������
		char exname[16];
		char name[16];
		scanf("%s%s", exname, name);
		if (strcmp(exname, "") == 0 || strcmp(name, "") == 0){
			printf("Fail to Rename File\n");
			return true;
		}
		RENAME(exname,name);
		return true;
	}
	else if (strcmp(order, cmd[7]) == 0){			//���ļ�
		char filename[16];
		scanf("%s", filename);						//���ļ���
		if (strcmp(filename, "") == 0){
			printf("Fail to Open File\n");
			return true;
		}
		OPEN(filename);
		return true;
	}
	else if (strcmp(order, cmd[8]) == 0){
		char filename[16];
		scanf("%s", filename);						//���ļ���
		if (strcmp(filename, "") == 0){
			printf("Fail to Open File\n");
			return true;
		}
		CLOSE(filename);
		return true;
	}
	
	else if (strcmp(order, cmd[9]) == 0){			//���ļ�
		char filename[16];
		scanf("%s", filename);
		if (strcmp(filename, "") == 0){
			printf("Fail to Find File\n");
			return true;
		}
		READ(filename);
		return true;
	}
	else if (strcmp(order, cmd[10]) == 0){			//д�ļ�
		char filename[16];
		scanf("%s", filename);
		if (strcmp(filename, "") == 0){
			printf("Fail to Find File\n");
			return true;
		}
		WRITE(filename);
		return true;
	}
	else if (strcmp(order, cmd[11]) == 0){			//�޸�Ȩ��
		char name[16];
		char mod[16];
		scanf("%s%s",name, mod);
		CHMOD(name,mod);
		return true;
	}
	else if (strcmp(order, cmd[12]) == 0){			//��ʽ��
		Format();
		Login();
		return true;
	}
	
	else if (strcmp(order, cmd[14]) == 0){			//����
		HELP();
		return true;
	}	
	else if (strcmp(order, cmd[15]) == 0){			//�鿴�ڴ����ļ��򿪱�
		LOOKATOPENFILE();	
		return true;
	}
	else{
		printf("Invalid Command\n");				//������Ч
		return true;
	}
}