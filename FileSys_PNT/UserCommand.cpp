#include "FileSys.h"

extern INode inode[INODE_MAXSIZE];        //Inode节点索引表
extern char data[DATADISTRICTION_MAXSIZE];//磁盘中的数据区
extern const char cmd[15][10] = { "ls", "cd", "mkdir", "rmdir", "mk", "rm", "rename", "open",
"close", "read", "write", "chmod", "format", "exit" };

extern void readInBlock(int blockNum, char *a);						//读出某磁盘块内信息
extern void writeInBlock(int blockNum, char *a);					//写入某块磁盘
extern void writedirItem(DirectoryItem* x, char* name);				//将新建的目录写入表项
extern void writefiletodirItem(DirectoryItem* x, char* name);		//将新建的文件写入表项extern int AddBlockbyInode(int id);
extern int FindBlockbyInode(int id, int *a);
extern int AddBlockbyInode(int id);									//通过inode编号寻找其中空物理块，若找到返回物理块号
extern void convertToAbs(char dir[], char* abspath);				//对输入的dir转换成绝对路径
extern char* strcut(char* dst, char *src, int n, int m);			//n为长度，m为位置 
extern void ReleaseInode(int ino);									//释放inode
extern void ReleaseBlock(int blo);									//释放块
extern int getInodebyAbs(char* absPath);							//给定绝对路径，返回对应inode

extern time_t GetCurrentTime();										//获取当前时间

extern bool isOpen(int ind);										//判断该文件是否被打开

extern void PrintFileInfo(DirectoryItem a);							//输出表项a的信息

extern void AddInOpenFileList(char* name, int id);					//在文件打开表中增加该项
extern void SubInOpenFileList(int id);								//在文件打开表中该项数-1
extern void DeleteInOpenFileList(int id);							//若有则删除该项,否则报错
extern void ModifyTimeinMem(int id);								//在内存的文件打开表中更新最后一次修改时间


void LS(char absPath[]);				//列该绝对路径下所有的文件或者目录
void HELP();							//命令提示
void MK(char* name);					//创建文件
void RM(char* name);					//删除文件
void MKDIR(char* name);					//创建目录
void RMDIR(char* name);					//删除目录
void CD(char* abspath);					//更改目录
void WRITE(char* name);					//写文件
void READ(char* name);					//读文件
void OPEN(char* name);					//打开文件
void CLOSE(char* name);					//关闭文件
void LOOKATOPENFILE();					//查看内存中文件打开表
void RENAME(char* exname,char* name);	//重命名文件或目录	
void CHMOD(char* name, char* mod);		//修改权限

void LS(){  //列该绝对路径下所有的文件或者目录
	int curind = superblock_mem.curind;
	
	if (inode[curind].file_type != DIRFILE){  //若路径的最后一个是文件名，报错
		printf("Path is illegal!\n");
		return;
	}
	for (int i = 0; i < 9; i++){			//直接
			int bl = inode[curind].i_zone[i];
			if (bl == -1) break;
			Directory aa = Directory();
			readInBlock(bl, (char *)&aa);

			//printf("c_time/m_time/inode/name/type/length/\n\n");
			//创建时间 上次修改时间 inode编号 文件名 类型 长度 权限
			for (int j = 0; j < 5; j++){
				int file_i = aa.it[j].d_ino;
				if (file_i != -1){
					
					PrintFileInfo(aa.it[j]);
				}
			}
			//return;
	}
	int bl = inode[curind].i_zone[9];    //间接
	if (bl == -1) return;
	Index a;
	readInBlock(bl, (char *)&a);
	for (int j = 0; j < 25; j++){
		int bl1 = a.idx[j];
		if (bl1 == -1) break;
		Directory aa = Directory();
		readInBlock(bl, (char *)&aa);

		for (int k = 0; k < 5; k++){
			int file_i = aa.it[j].d_ino;
			if (file_i != -1){
				PrintFileInfo(aa.it[j]);
			}
		}
		return;
	}
}

void HELP(){				//命令提示
	char tmp[100];
	strcpy(tmp, "List the FILE or DIRECTORY.");
	printf("%-10s%20s\n", cmd[0],tmp);
	
	strcpy(tmp, "change the DIRECTORY(absolute or relative).");
	printf("%-10s%20s\n", cmd[1], tmp);
	
	strcpy(tmp, "create the DIRECTORY, if it do not already exist.");
	printf("%-10s%20s\n", cmd[2], tmp);
	
	strcpy(tmp, "Remove the DIRECTORY.");
	printf("%-10s%20s\n", cmd[3], tmp);
	
	strcpy(tmp, "Create the FILE, if it does not already exist.");
	printf("%-10s%20s\n", cmd[4], tmp);
	
	strcpy(tmp, "Remove the FILE.");
	printf("%-10s%10s\n", cmd[5], tmp);
	
	strcpy(tmp, "Rename the FILE or DIRECTORY.");
	printf("%-10s%20s\n", cmd[6], tmp);
	
	strcpy(tmp, "Open the FILE, if it exists.");
	printf("%-10s%20s\n", cmd[7], tmp);
	
	strcpy(tmp, "Close the FILE, if it has already open.");
	printf("%-10s%20s\n", cmd[8], tmp);
	
	strcpy(tmp, "Read the FILE, if it has already open.");
	printf("%-10s%20s\n", cmd[9], tmp);
	
	strcpy(tmp, "Write the FILE, if it has already open.");
	printf("%-10s%20s\n", cmd[10], tmp);
	
	strcpy(tmp, "Change the mode of the FILE or DIRECTORY.");
	printf("%-10s%20s\n", cmd[11], tmp);
	
	strcpy(tmp, "Format the FILE System.");
	printf("%-10s%20s\n", cmd[12], tmp);
	
	strcpy(tmp, "Exit the FILE System.");
	printf("%-10s%20s\n", cmd[13], tmp);
}

void MK(char* name){								//创建文件
	int id = superblock_mem.curind;					//当前块的inode

	int block[35];
	int cnt = FindBlockbyInode(id, block);			//通过inode编号寻找物理块，返回记录关联块号的数组a
	for (int i = 0; i < cnt-1; i++) {
		Directory a = Directory();
		readInBlock(block[i], (char*)&a);
		for (int j = 0; j < 5; j++){
			if (a.it[j].d_ino != -1 && strcmp(a.it[j].d_name, name) == 0){
				printf("There is already a file with same name\n");
				return;
			}
		}
	}

	Directory a = Directory();
	readInBlock(block[cnt - 1], (char*)&a);			//读出最后一块，因为最后一块可能未满
	for (int i = 0; i < 5; i++){
		Directory a = Directory();
		readInBlock(block[i], (char*)&a);
		for (int j = 0; j < 5; j++){
			if (a.it[j].d_ino != -1 && strcmp(a.it[j].d_name, name) == 0){
				printf("There is already a file with same name\n");
				return;
			}
		}
		if (a.it[i].d_ino == -1){
			writefiletodirItem(&a.it[i], name);		//添加表项,表项的内容是文件
			writeInBlock(block[cnt - 1], (char*)&a);//将整块写回磁盘
			return;
		}
	}
	int newblocknum = AddBlockbyInode(id);		    //返回inode编号对应的新块号
	Directory b = Directory();
	writefiletodirItem(&b.it[0], name);				//添加表项,表项的内容是文件
	writeInBlock(newblocknum, (char*)&b);			//将整块写回磁盘
}

void RM(char* name){
	int id = superblock_mem.curind;

	int block[35];
	int cnt = FindBlockbyInode(id, block);			//通过inode编号寻找物理块，返回记录关联块号的数组block

	for (int i = 0; i < cnt; i++){
		Directory x = Directory();
		readInBlock(block[i], (char*)&x);			//读出第i块
		for (int j = 0; j < 5; j++){
			//if (x.it[j].d_ino == -1) continue;
			if (strcmp(x.it[j].d_name, name) == 0){
				//名字匹配
				int del_i = x.it[j].d_ino;
				if (inode[del_i].file_type == NORMALFILE){
					//是文件
					//要被替换的块 块号为block[i],项号为j
					//最后一块的块号为block[cnt-1]
					Directory y = Directory();
					readInBlock(block[cnt - 1], (char*)&y);			//读出最后一块
					for (int k = 4; k >= 0; k--){
						if (y.it[k].d_ino != -1){
							//当前目录下最后一个目录

							//释放inode和块
							int blocknum;
							for (int k = 0; k < 9; k++){
								blocknum = inode[del_i].i_zone[k];
								if (blocknum != -1){
									ReleaseBlock(blocknum);
									inode[del_i].i_zone[k] = -1;
								}
							}
							blocknum = inode[del_i].i_zone[9];
							if (blocknum != -1){
								Index a = Index();
								readInBlock(blocknum, (char*)&a);
								for (int k = 0; k < 25; k++){
									if (a.idx[k] != -1){
										ReleaseBlock(a.idx[k]);
									}
								}
								ReleaseBlock(blocknum);
							}

							if(isOpen(del_i)) DeleteInOpenFileList(del_i);	//若文件已打开则在文件打开表中删除

							ReleaseInode(del_i);		//最后释放inode

							x.it[j].d_ino = y.it[k].d_ino;
							strcpy(x.it[j].d_name, y.it[k].d_name);
							if (block[i] == block[cnt - 1]){
								y.it[j].d_ino = y.it[k].d_ino;
								strcpy(y.it[j].d_name, y.it[k].d_name);
							}
							y.it[k].d_ino = -1;    //删除最后一个节点
							strcpy(y.it[k].d_name, "");

							writeInBlock(block[cnt - 1], (char*)&y);			//写回最后一块

							if (k == 0 && cnt != 1){
								//移动的最后一项是最后一块的第一项，要把最后一块删除
								if (cnt > 9){
									//一次间接索引
									Index xxx;
									readInBlock(inode[id].i_zone[9], (char*)&xxx);
									xxx.idx[cnt - 10] = -1;
									writeInBlock(inode[id].i_zone[9], (char*)&xxx);
								}
								else{
									//直接索引
									inode[id].i_zone[cnt-1] = -1;
								}
							}
							if (block[i] != block[cnt - 1]) writeInBlock(block[i], (char*)&x);			//写回第block[i]块
							printf("Delete Succeed!\n");
							return;
						}
					}
				}
				printf("It's not a file\n");
				return;
			}
		}
	}

	printf("Cannot Find the File\n");
	return;
}

void MKDIR(char* name){								//创建目录
	int id = superblock_mem.curind;					//当前块的inode
	
	int block[35];
	int cnt = FindBlockbyInode(id, block);			//通过inode编号寻找物理块，返回记录关联块号的数组a
	for (int i = 0; i < cnt - 1; i++) {
		Directory a = Directory();
		readInBlock(block[i], (char*)&a);
		for (int j = 0; j < 5; j++){
			if (a.it[j].d_ino != -1 && strcmp(a.it[j].d_name, name) == 0){
				printf("There is already a file with same name\n");
				return;
			}
		}
	}

	Directory a = Directory();
	readInBlock(block[cnt - 1], (char*)&a);			//读出最后一块，因为最后一块可能未满
	for (int i = 0; i < 5; i++){
		if (a.it[i].d_ino != -1 && strcmp(a.it[i].d_name, name) == 0){
			printf("There is already a file with same name\n");
			return;
		}
		if (a.it[i].d_ino == -1){
			writedirItem(&a.it[i], name);
			writeInBlock(block[cnt - 1], (char*)&a);
			return;
		}
	}
	int newblocknum = AddBlockbyInode(id);		    //返回inode编号对应的新块号
	Directory b = Directory();
	writedirItem(&b.it[0], name);					//添加表项
	writeInBlock(newblocknum, (char*)&b);			//将整块写回磁盘
}

void RMDIR(char* name){
	int id = superblock_mem.curind;

	int block[35];
	int cnt = FindBlockbyInode(id, block);			//通过inode编号寻找物理块，返回记录关联块号的数组block

	for (int i = 0; i < cnt; i++){
		Directory x = Directory();
		readInBlock(block[i], (char*)&x);			//读出第i块
		for (int j = 0; j < 5; j++){
			//if (x.it[j].d_ino == -1) continue;
			if (strcmp(x.it[j].d_name, name) == 0){
				//名字匹配
				int del_i = x.it[j].d_ino; 
				if (inode[del_i].file_type == DIRFILE){
					//是目录文件
					Directory w = Directory();
					readInBlock(inode[del_i].i_zone[0], (char*)&w);
					if (w.it[0].d_ino == -1){
						//是空目录

						//要被替换的块 块号为block[i],项号为j
						//最后一块的块号为block[cnt-1]
						Directory y = Directory();
						readInBlock(block[cnt-1], (char*)&y);			//读出最后一块
						for (int k = 4; k >= 0; k--){

							if (y.it[k].d_ino != -1){
								//当前目录下最后一个目录
								
								//释放inode和块
								ReleaseBlock(inode[x.it[j].d_ino].i_zone[0]);
								ReleaseInode(x.it[j].d_ino);

								x.it[j].d_ino = y.it[k].d_ino;
								strcpy(x.it[j].d_name, y.it[k].d_name);
								if (block[i] == block[cnt - 1]){
									y.it[j].d_ino = y.it[k].d_ino;
									strcpy(y.it[j].d_name, y.it[k].d_name);
								}
								y.it[k].d_ino = -1;    //删除最后一个节点
								strcpy(y.it[k].d_name, "");

								writeInBlock(block[cnt - 1], (char*)&y);			//写回最后一块

								if (k == 0 && cnt!=1){
									//移动的最后一项是最后一块的第一项，要把最后一块删除
									if (cnt > 9){
										//一次间接索引
										Index xxx;
										readInBlock(inode[id].i_zone[9], (char*)&xxx);
										xxx.idx[cnt - 10] = -1;
										writeInBlock(inode[id].i_zone[9], (char*)&xxx);
									}
									else{
										//直接索引
										inode[id].i_zone[cnt-1] = -1;
									}
								}
								if(block[i]!=block[cnt-1]) writeInBlock(block[i], (char*)&x);			//写回第block[i]块
								printf("Delete Succeed!\n");
								return;
							}
						}
					}
					printf("The Directory is NOT Emtpy\n");
					return;
				}
			}
		}
	}

	printf("Cannot Find the Directory\n");
	return;
}

void CD(char* abspath){
	int id = getInodebyAbs(abspath);
	if (id == -1){
		return;
	}
	superblock_mem.curind = id;
	strcpy(superblock_mem.CurDir, abspath);
}

void OPEN(char* name){
	char abspath[100];
	convertToAbs(name, abspath);		//对输入的dir转换成绝对路径
	int id = getInodebyAbs(abspath);	//得到该文件对应的inode
	if (id == -1) return;
	if (inode[id].file_type != NORMALFILE){
		printf("It is not a file\n");
		return;
	}

	//此处加权限判断
	AddInOpenFileList(name,id);
	
}

void CLOSE(char* name){
	char abspath[100];
	convertToAbs(name, abspath);		//对输入的dir转换成绝对路径
	int id = getInodebyAbs(abspath);	//得到该文件对应的inode
	if (id == -1) return;
	if (inode[id].file_type != NORMALFILE){
		printf("It is not a file\n");
		return;
	}

	SubInOpenFileList(id);
}

void WRITE(char* name){		//写文件
	char abspath[100];
	convertToAbs(name, abspath);		//对输入的dir转换成绝对路径
	int id = getInodebyAbs(abspath);	//得到该文件对应的inode
	if (id == -1) return;
	if (inode[id].file_type != NORMALFILE){
		printf("It is not a file\n");
		return;
	}
	if (!isOpen(id)){
		printf("The file is not open\n");
		return;
	}

	//此处加权限判断

	//最后一块的信息会被不断更新
	getchar();		//吸收上一个回车

	int blocksize = 0;    //新增块数

	char str[1000];
	while (gets(str)){		//读一行
		if (strcmp(str, "") == 0) break;  //文件写完
		
		strcat(str, "\n");		//末位增加一个回车

		int block[35];
		int cnt = FindBlockbyInode(id, block);   //已连接的块号
		int lastblock = block[cnt - 1];			//最后一块相连的

		File a = File();
		readInBlock(lastblock, (char*)&a);		//读出最后一块的内容


		int existlen = strlen(a.content);        //已存的长度
		int leftlen = 100 - existlen-1;			 //剩余的长度
		int input_len = strlen(str);
		if (input_len <= leftlen){			//输入串能放在最后一块中，不必新申请
			strcat(a.content, str);			//在content后增加输入的串
			writeInBlock(lastblock, (char*)&a);			//写回磁盘
			continue;
		}
		//在ss中截取长度为leftlen的字符存入最后一块中
		char ss[1000];
		memset(ss, 0, sizeof(ss));
		strcut(ss, str, leftlen, 0);
		strcat(a.content,ss);

		//strncpy(a.content + existlen, str, leftlen);
		//将str更新为减去前leftlen长度的字符串后剩下的
		
		strcpy(ss, str);
		strcut(str, ss, strlen(ss) - leftlen,leftlen);

		int xxxx = strlen(str);

		writeInBlock(lastblock, (char*)&a);

		//将str分裂成每100个一块，最多10块
		while(strlen(str)>0){
			int newblock = AddBlockbyInode(id);
			if (newblock == -1){
				printf("File is out of range\n");
				//此处更新修改时间
				inode[id].file_mtime = GetCurrentTime();
				ModifyTimeinMem(id);					//在内存的文件打开表中更新最后一次修改时间
				inode[id].file_size += blocksize * 100;	//修改文件的大小
				return;
			}
			blocksize++;
			char strblock[100];
			memset(strblock, 0, sizeof(strblock));
			int len;
			if (strlen(str) < 100-1) len = strlen(str);
			else len = 100-1;
			strncpy(strblock, str, len);
			//strcpy(strblock, "\0");
			char ss[1000];  //临时变量
			memset(ss, 0, sizeof(ss));
			strcpy(ss, str);
			strcut(str, ss, strlen(ss) - len,len);

			File aa = File();
			strcpy(aa.content, strblock);
			writeInBlock(newblock, (char*)&aa);     //将块写回磁盘
		}
	}

	//此处更新修改时间
	inode[id].file_mtime = GetCurrentTime();
	ModifyTimeinMem(id);					//在内存的文件打开表中更新最后一次修改时间
	inode[id].file_size += blocksize * 100;	//修改文件的大小

	/*int xxxbl[35];
	int num = FindBlockbyInode(id,xxxbl);
	for (int i = 0; i < num; i++){
		cout << xxxbl[i] << " ";
	}
	cout << endl;*/

	return;
}

void READ(char* name){
	char abspath[100];
	convertToAbs(name, abspath);		//对输入的dir转换成绝对路径
	int id = getInodebyAbs(abspath);	//得到该文件对应的inode
	if (id == -1) return;
	if (inode[id].file_type != NORMALFILE){
		printf("It is not a file\n");
		return;
	}

	if (!isOpen(id)){
		printf("The file is not open\n");
		return;
	}

	//此处可加权限判断

	int block[35];
	int cnt = FindBlockbyInode(id, block);
	for (int i = 0; i < cnt; i++){
		int bl = block[i];
		File f;
		readInBlock(bl, (char*)&f);
		printf("%s", f.content);
	}
}


void LOOKATOPENFILE(){							//查看内存中文件打开表
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		printf("%s(%d) ", superblock_mem.OpenList[i].name,superblock_mem.OpenList[i].cnt);
	}
	printf("\n");
	return;
}

void RENAME(char* exname, char* name){				//重命名
	int curind = superblock_mem.curind;

	for (int i = 0; i < 9; i++){			//直接
		int bl = inode[curind].i_zone[i];
		if (bl == -1) break;
		Directory aa = Directory();
		readInBlock(bl, (char *)&aa);

		for (int j = 0; j < 5; j++){
			int file_i = aa.it[j].d_ino;
			if (file_i != -1 && strcmp(aa.it[j].d_name,exname)==0){
				strcpy(aa.it[j].d_name, name);
				writeInBlock(bl, (char *)&aa);
				return;
			}
		}
	}
	int bl = inode[curind].i_zone[9];    //间接
	if (bl == -1) return;
	Index a;
	readInBlock(bl, (char *)&a);
	for (int j = 0; j < 25; j++){
		int bl1 = a.idx[j];
		if (bl1 == -1) break;
		Directory aa = Directory();
		readInBlock(bl, (char *)&aa);

		for (int k = 0; k < 5; k++){
			int file_i = aa.it[j].d_ino;
			if (file_i != -1 && strcmp(aa.it[j].d_name, exname) == 0){
				strcpy(aa.it[j].d_name, name);
				writeInBlock(bl, (char *)&aa);
				return;
			}
		}
	}
	printf("Cannot find %s\n", exname);
	return;

	
}

bool isLegal(char* mod){
	//判断权限是否合法
	if (strlen(mod) != 9) return false;
	for (int i = 0; i < strlen(mod); i++){
		if (mod[i] != 'r' && mod[i] != 'w'&& mod[i] != '_')
			return false;
	}
	return true;
}

void CHMOD(char* name,char* mod){					//修改权限
	if (!isLegal(mod)){
		printf("mod is not legal\n");
		return;
	}
	int curind = superblock_mem.curind;

	for (int i = 0; i < 9; i++){					//直接
		int bl = inode[curind].i_zone[i];
		if (bl == -1) break;
		Directory aa = Directory();
		readInBlock(bl, (char *)&aa);

		for (int j = 0; j < 5; j++){
			int file_i = aa.it[j].d_ino;
			if (file_i != -1 && strcmp(aa.it[j].d_name, name) == 0){
				strcpy(inode[file_i].file_attri, mod);
				writeInBlock(bl, (char *)&aa);
				return;
			}
		}
	}
	int bl = inode[curind].i_zone[9];				//间接
	if (bl == -1) return;
	Index a;
	readInBlock(bl, (char *)&a);
	for (int j = 0; j < 25; j++){
		int bl1 = a.idx[j];
		if (bl1 == -1) break;
		Directory aa = Directory();
		readInBlock(bl, (char *)&aa);

		for (int k = 0; k < 5; k++){
			int file_i = aa.it[j].d_ino;
			if (file_i != -1 && strcmp(aa.it[j].d_name, name) == 0){
				strcpy(inode[file_i].file_attri, mod);
				writeInBlock(bl, (char *)&aa);
				return;
			}
		}
	}
	printf("Cannot find %s\n", name);
	return;

}