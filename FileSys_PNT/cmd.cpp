#include "FileSys.h"

const char cmd[20][10] = {"ls","cd","mkdir","rmdir","mk","rm","rename","open",
"close","read","write","chmod","format","exit","help","look"};

extern void Login();
extern void LS();					//列文件或目录
extern void CD(char* abspath);		//更改当前目录
extern void MKDIR(char* name);		//创建目录
extern void RMDIR(char* name);		//移除目录(仅当目录为空)
extern void MK(char* name);			//创建目录，初始时分配一块物理块
extern void RM(char* name);			//移除文件(仅当目录为空)
extern void RENAME(char* exname,char* name);//重命名
extern void Format();				//格式化磁盘
extern void HELP();					//提供帮助
extern void WRITE(char* name);		//写文件
extern void READ(char* name);		//读文件
extern void CHMOD(char* name,char* mod);//修改权限
extern void OPEN(char* name);		//打开文件
extern void CLOSE(char* name);		//关闭文件
extern void LOOKATOPENFILE();		//查看内存中文件打开表

void convertToAbs(char dir[], char* abspath);		//对输入的dir转换成绝对路径
bool getCommand();									//总命令接口


void convertToAbs(char dir[], char* abspath){		//对输入的dir转换成绝对路径
	if (dir[0] == '/') strcpy(abspath, dir);		//dir为绝对路径
	else{
		strcpy(abspath, superblock_mem.CurDir);
		strcat(abspath, "/");
		strcat(abspath, dir);
	}
}

bool getCommand(){
	char order[10];
	scanf("%s", order); //读取命令
	if (strcmp(order, cmd[13]) == 0) return false;   //结束命令

	if (strcmp(order, cmd[0]) == 0){				//列当前目录
		LS();
		return true;
	}
	else if (strcmp(order, cmd[1]) == 0){			//改变目录
		char dir[16];
		scanf("%s", dir);							//读取目录名称
		if (dir == "") return true;					//读入无效
		char abspath[200];
		convertToAbs(dir, abspath);					//转成绝对路径
		CD(abspath);
	}
	else if (strcmp(order, cmd[2]) == 0){			//创建目录
		char dir[16];
		scanf("%s", dir);							//读目录名
		if (strcmp(dir, "") == 0){
			printf("Fail to Create Directory\n");
			return true;
		}
		MKDIR(dir);									//创建目录
		return true;
	}
	else if (strcmp(order, cmd[3]) == 0){			//删除目录
		char dir[16];
		scanf("%s", dir);							//读目录名
		if (strcmp(dir, "") == 0){
			printf("Fail to Delete Directory\n");
			return true;
		}
		RMDIR(dir);									//删除目录
		return true;
	}
	else if (strcmp(order, cmd[4]) == 0){			//创建文件
		char filename[16];
		scanf("%s", filename);						//读文件名
		if (strcmp(filename, "") == 0){
			printf("Fail to Create File\n");
			return true;
		}
		MK(filename);								//创建文件
		return true;
	}
	else if (strcmp(order, cmd[5]) == 0){			//删除文件
		char filename[16];
		scanf("%s", filename);						//读文件名
		if (strcmp(filename, "") == 0){
			printf("Fail to Delete File\n");
			return true;
		}
		RM(filename);								//删除文件
		return true;
	}
	else if (strcmp(order, cmd[6]) == 0){			//重命名
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
	else if (strcmp(order, cmd[7]) == 0){			//打开文件
		char filename[16];
		scanf("%s", filename);						//读文件名
		if (strcmp(filename, "") == 0){
			printf("Fail to Open File\n");
			return true;
		}
		OPEN(filename);
		return true;
	}
	else if (strcmp(order, cmd[8]) == 0){
		char filename[16];
		scanf("%s", filename);						//读文件名
		if (strcmp(filename, "") == 0){
			printf("Fail to Open File\n");
			return true;
		}
		CLOSE(filename);
		return true;
	}
	
	else if (strcmp(order, cmd[9]) == 0){			//读文件
		char filename[16];
		scanf("%s", filename);
		if (strcmp(filename, "") == 0){
			printf("Fail to Find File\n");
			return true;
		}
		READ(filename);
		return true;
	}
	else if (strcmp(order, cmd[10]) == 0){			//写文件
		char filename[16];
		scanf("%s", filename);
		if (strcmp(filename, "") == 0){
			printf("Fail to Find File\n");
			return true;
		}
		WRITE(filename);
		return true;
	}
	else if (strcmp(order, cmd[11]) == 0){			//修改权限
		char name[16];
		char mod[16];
		scanf("%s%s",name, mod);
		CHMOD(name,mod);
		return true;
	}
	else if (strcmp(order, cmd[12]) == 0){			//格式化
		Format();
		Login();
		return true;
	}
	
	else if (strcmp(order, cmd[14]) == 0){			//帮助
		HELP();
		return true;
	}	
	else if (strcmp(order, cmd[15]) == 0){			//查看内存中文件打开表
		LOOKATOPENFILE();	
		return true;
	}
	else{
		printf("Invalid Command\n");				//命令无效
		return true;
	}
}