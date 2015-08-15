#include "FileSys.h"


time_t GetCurrentTime();									//获取当前时间
void ModifyTimeinMem(int id);								//在内存的文件打开表中更新最后一次修改时间
void AddInOpenFileList(char* name, int id);					//若已存在，不添加，inode[id].i_count++(引用次数+1)
void SubInOpenFileList(int id);								//在文件打开表中该项数-1
void DeleteInOpenFileList(int id);							//若有则删除该项,否则报错

void writedirItem(DirectoryItem* x, char* name);			//将新建的目录写入表项
void writefiletodirItem(DirectoryItem* x, char* name);		//将新建的文件写入表项
char* strcut(char* dst, char *src, int n, int m);			//n为长度，m为位置 
int getInodebyAbs(char* absPath);							//给定绝对路径，返回对应inode

bool isOpen(int ind);										//判断该文件是否被打开

void PrintFileInfo(DirectoryItem a);						//输出文件信息

void writedirItem(DirectoryItem* x, char* name){		//将新建的目录写入表项
	int newid;
	newid = x->d_ino = GetNextInodeNum();		//获取inode编号
	strcpy(x->d_name, name);					//inode编号对应目录名
	inode[newid].CreateInode(newid, 1, superblock_mem.curuserid, DIRFILE, "xxxxxxxx");    //创建新inode

	time_t ct;
	ct = time(NULL);
	inode[newid].file_ctime = ct;			//创建时间
	inode[newid].file_mtime = ct;			//上次修改时间

	int newblock;
	newblock = inode[newid].i_zone[0] = GetNextBlockNum();		//获取物理块号
	Directory xx = Directory();
	writeInBlock(newblock, (char*)&xx);
}

void writefiletodirItem(DirectoryItem* x, char* name){		//将新建的文件写入表项
	int newid;
	newid = x->d_ino = GetNextInodeNum();					//获取inode编号
	strcpy(x->d_name, name);								//inode编号对应文件名
	inode[newid].CreateInode(newid, 1 * 100, superblock_mem.curuserid, NORMALFILE, "rwwrwwrww");    //创建新inode
	
	time_t ct;
	ct = time(NULL);
	inode[newid].file_ctime = ct;
	inode[newid].file_mtime = ct;
	inode[newid].i_count = 0;

	int newblock;
	newblock = inode[newid].i_zone[0] = GetNextBlockNum();	 //获取物理块号
	File xx = File();
	writeInBlock(newblock, (char*)&xx);
}

int getInodebyAbs(char* absPath){							//给定绝对路径，返回对应inode
	if (strcmp(absPath, "/") == 0) return 0;				//绝对路径为根目录

	int cnt = 0;
	int start = 1;
	char str[100][15];     //最多100层，文件名最长15


	if (absPath[0] != '/'){
		printf("Path is illegal!\n");
		return -1;
	}

	//得到第一个目录名
	for (int i = 1; i < strlen(absPath); i++){
		if (absPath[i] == '/'){
			strncpy(str[cnt], absPath + start, i - start);
			str[cnt][i - start] = '\0';
			cnt++;
			int x = strlen(str[cnt - 1]);
			start = i + 1;
		}
	}
	strncpy(str[cnt], absPath + start, strlen(absPath) - start);   //最后一个目录
	str[cnt][strlen(absPath) - start] = '\0';
	cnt++;

	//int x = strlen(str[cnt - 1]);

	//bool find = false;
	int curind = 0;    //当前目录对应的inode
	for (int i = 0; i < cnt; i++){
		if (inode[curind].file_type != DIRFILE){   //不是目录文件
			printf("Path is illegal!\n");
			return -1;
		}

		bool flag = false;
		//直接索引
		for (int j = 0; j < 9; j++){
			int bl = inode[curind].i_zone[j];
			if (bl == -1) break;    //不存在目录或文件
			Directory a = Directory();
			readInBlock(bl, (char *)&a);
			for (int k = 0; k < 5; k++){
				if (strcmp(a.it[k].d_name, str[i]) == 0){
					curind = a.it[k].d_ino;
					flag = true;
					break;
				}
			}
			if (flag) break;
		}
		//一次间接索引
		if (!flag){
			int bl = inode[curind].i_zone[9];
			if (bl != -1){
				Index a;
				readInBlock(bl, (char *)&a);
				for (int j = 0; j < 25; j++){
					int bl1 = a.idx[j];
					if (bl1 == -1) break;
					Directory aa = Directory();
					readInBlock(bl1, (char *)&aa);
					for (int k = 0; k < 5; k++){
						if (strcmp(aa.it[k].d_name, str[i]) == 0){
							curind = aa.it[k].d_ino;
							flag = true;
							break;
						}
					}
					if (flag) break;
				}
			}
		}
		if (!flag){
			printf("Cannot Find\n");
			return -1;
		}
	}

	return curind;
}

char* strcut(char* dst, char *src, int n, int m){//n为长度，m为位置 
	char *p = src;
	char *q = dst;
	int len = strlen(src);
	if (n>len) n = len - m;
	if (m<0) m = 0;
	if (m>len) return NULL;
	p += m;
	while (n--){
		*q = *p;
		q++;
		p++;
	}
	*q = '\0';
	return dst;
}


void AddInOpenFileList(char* name,int id){			//若已存在，不添加，inode[id].i_count++(引用次数+1)
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		if (superblock_mem.OpenList[i].id == id){
			superblock_mem.OpenList[i].cnt++;
			return;
		}
	}
	superblock_mem.OpenList.push_back(OpenFileListItem(name, id, inode[id].file_ctime, inode[id].file_mtime));
	inode[id].i_count++;
}

void SubInOpenFileList(int id){							//在文件打开表中该项数-1
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		if (superblock_mem.OpenList[i].id == id){
			superblock_mem.OpenList[i].cnt--;
			if (superblock_mem.OpenList[i].cnt == 0) DeleteInOpenFileList(id);
			return;
		}
	}
	printf("Cannot Find In OpenFileList\n");
	return;
}

void DeleteInOpenFileList(int id){	//若有则删除该项
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		if (superblock_mem.OpenList[i].id == id){
			superblock_mem.OpenList.erase(superblock_mem.OpenList.begin() + i);
			return;
		}
	}
	printf("Cannot Find In OpenFileList\n");
	return;
}

time_t GetCurrentTime(){
	time_t tl;
	tl = time(NULL);
	return tl;
}

void ModifyTimeinMem(int id){		//在内存的文件打开表中更新最后一次修改时间
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		if (superblock_mem.OpenList[i].id == id){
			superblock_mem.OpenList[i].m_time = inode[id].file_mtime;
			return;
		}
	}
}

void PrintFileInfo(DirectoryItem a){
	int file_i = a.d_ino;
	static char str_time[100];
	struct tm *local_time = NULL;

	local_time = localtime(&inode[file_i].file_ctime);
	strftime(str_time, sizeof(str_time), "%Y-%m-%d %H:%M", local_time);

	printf(" %s\t", str_time);

	local_time = localtime(&inode[file_i].file_mtime);
	strftime(str_time, sizeof(str_time), "%Y-%m-%d %H:%M", local_time);

	printf(" %s\t", str_time);

	printf(" %d\t", file_i);
	printf(" %s\t", a.d_name);
	if (inode[file_i].file_type == 1) printf(" Dir\t");
	else if (inode[file_i].file_type == 2) printf(" File\t");
	printf(" %d\n", inode[file_i].file_size);         //输出文件信息
}


bool isOpen(int ind){
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		if (superblock_mem.OpenList[i].id == ind){
			return true;
		}
	}
	return false;
}