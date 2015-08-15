#include "FileSys.h"

extern INode inode[INODE_MAXSIZE];        //Inode�ڵ�������
extern char data[DATADISTRICTION_MAXSIZE];//�����е�������
extern const char cmd[15][10] = { "ls", "cd", "mkdir", "rmdir", "mk", "rm", "rename", "open",
"close", "read", "write", "chmod", "format", "exit" };

extern void readInBlock(int blockNum, char *a);						//����ĳ���̿�����Ϣ
extern void writeInBlock(int blockNum, char *a);					//д��ĳ�����
extern void writedirItem(DirectoryItem* x, char* name);				//���½���Ŀ¼д�����
extern void writefiletodirItem(DirectoryItem* x, char* name);		//���½����ļ�д�����extern int AddBlockbyInode(int id);
extern int FindBlockbyInode(int id, int *a);
extern int AddBlockbyInode(int id);									//ͨ��inode���Ѱ�����п�����飬���ҵ�����������
extern void convertToAbs(char dir[], char* abspath);				//�������dirת���ɾ���·��
extern char* strcut(char* dst, char *src, int n, int m);			//nΪ���ȣ�mΪλ�� 
extern void ReleaseInode(int ino);									//�ͷ�inode
extern void ReleaseBlock(int blo);									//�ͷſ�
extern int getInodebyAbs(char* absPath);							//��������·�������ض�Ӧinode

extern time_t GetCurrentTime();										//��ȡ��ǰʱ��

extern bool isOpen(int ind);										//�жϸ��ļ��Ƿ񱻴�

extern void PrintFileInfo(DirectoryItem a);							//�������a����Ϣ

extern void AddInOpenFileList(char* name, int id);					//���ļ��򿪱������Ӹ���
extern void SubInOpenFileList(int id);								//���ļ��򿪱��и�����-1
extern void DeleteInOpenFileList(int id);							//������ɾ������,���򱨴�
extern void ModifyTimeinMem(int id);								//���ڴ���ļ��򿪱��и������һ���޸�ʱ��


void LS(char absPath[]);				//�иþ���·�������е��ļ�����Ŀ¼
void HELP();							//������ʾ
void MK(char* name);					//�����ļ�
void RM(char* name);					//ɾ���ļ�
void MKDIR(char* name);					//����Ŀ¼
void RMDIR(char* name);					//ɾ��Ŀ¼
void CD(char* abspath);					//����Ŀ¼
void WRITE(char* name);					//д�ļ�
void READ(char* name);					//���ļ�
void OPEN(char* name);					//���ļ�
void CLOSE(char* name);					//�ر��ļ�
void LOOKATOPENFILE();					//�鿴�ڴ����ļ��򿪱�
void RENAME(char* exname,char* name);	//�������ļ���Ŀ¼	
void CHMOD(char* name, char* mod);		//�޸�Ȩ��

void LS(){  //�иþ���·�������е��ļ�����Ŀ¼
	int curind = superblock_mem.curind;
	
	if (inode[curind].file_type != DIRFILE){  //��·�������һ�����ļ���������
		printf("Path is illegal!\n");
		return;
	}
	for (int i = 0; i < 9; i++){			//ֱ��
			int bl = inode[curind].i_zone[i];
			if (bl == -1) break;
			Directory aa = Directory();
			readInBlock(bl, (char *)&aa);

			//printf("c_time/m_time/inode/name/type/length/\n\n");
			//����ʱ�� �ϴ��޸�ʱ�� inode��� �ļ��� ���� ���� Ȩ��
			for (int j = 0; j < 5; j++){
				int file_i = aa.it[j].d_ino;
				if (file_i != -1){
					
					PrintFileInfo(aa.it[j]);
				}
			}
			//return;
	}
	int bl = inode[curind].i_zone[9];    //���
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

void HELP(){				//������ʾ
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

void MK(char* name){								//�����ļ�
	int id = superblock_mem.curind;					//��ǰ���inode

	int block[35];
	int cnt = FindBlockbyInode(id, block);			//ͨ��inode���Ѱ������飬���ؼ�¼������ŵ�����a
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
	readInBlock(block[cnt - 1], (char*)&a);			//�������һ�飬��Ϊ���һ�����δ��
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
			writefiletodirItem(&a.it[i], name);		//��ӱ���,������������ļ�
			writeInBlock(block[cnt - 1], (char*)&a);//������д�ش���
			return;
		}
	}
	int newblocknum = AddBlockbyInode(id);		    //����inode��Ŷ�Ӧ���¿��
	Directory b = Directory();
	writefiletodirItem(&b.it[0], name);				//��ӱ���,������������ļ�
	writeInBlock(newblocknum, (char*)&b);			//������д�ش���
}

void RM(char* name){
	int id = superblock_mem.curind;

	int block[35];
	int cnt = FindBlockbyInode(id, block);			//ͨ��inode���Ѱ������飬���ؼ�¼������ŵ�����block

	for (int i = 0; i < cnt; i++){
		Directory x = Directory();
		readInBlock(block[i], (char*)&x);			//������i��
		for (int j = 0; j < 5; j++){
			//if (x.it[j].d_ino == -1) continue;
			if (strcmp(x.it[j].d_name, name) == 0){
				//����ƥ��
				int del_i = x.it[j].d_ino;
				if (inode[del_i].file_type == NORMALFILE){
					//���ļ�
					//Ҫ���滻�Ŀ� ���Ϊblock[i],���Ϊj
					//���һ��Ŀ��Ϊblock[cnt-1]
					Directory y = Directory();
					readInBlock(block[cnt - 1], (char*)&y);			//�������һ��
					for (int k = 4; k >= 0; k--){
						if (y.it[k].d_ino != -1){
							//��ǰĿ¼�����һ��Ŀ¼

							//�ͷ�inode�Ϳ�
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

							if(isOpen(del_i)) DeleteInOpenFileList(del_i);	//���ļ��Ѵ������ļ��򿪱���ɾ��

							ReleaseInode(del_i);		//����ͷ�inode

							x.it[j].d_ino = y.it[k].d_ino;
							strcpy(x.it[j].d_name, y.it[k].d_name);
							if (block[i] == block[cnt - 1]){
								y.it[j].d_ino = y.it[k].d_ino;
								strcpy(y.it[j].d_name, y.it[k].d_name);
							}
							y.it[k].d_ino = -1;    //ɾ�����һ���ڵ�
							strcpy(y.it[k].d_name, "");

							writeInBlock(block[cnt - 1], (char*)&y);			//д�����һ��

							if (k == 0 && cnt != 1){
								//�ƶ������һ�������һ��ĵ�һ�Ҫ�����һ��ɾ��
								if (cnt > 9){
									//һ�μ������
									Index xxx;
									readInBlock(inode[id].i_zone[9], (char*)&xxx);
									xxx.idx[cnt - 10] = -1;
									writeInBlock(inode[id].i_zone[9], (char*)&xxx);
								}
								else{
									//ֱ������
									inode[id].i_zone[cnt-1] = -1;
								}
							}
							if (block[i] != block[cnt - 1]) writeInBlock(block[i], (char*)&x);			//д�ص�block[i]��
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

void MKDIR(char* name){								//����Ŀ¼
	int id = superblock_mem.curind;					//��ǰ���inode
	
	int block[35];
	int cnt = FindBlockbyInode(id, block);			//ͨ��inode���Ѱ������飬���ؼ�¼������ŵ�����a
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
	readInBlock(block[cnt - 1], (char*)&a);			//�������һ�飬��Ϊ���һ�����δ��
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
	int newblocknum = AddBlockbyInode(id);		    //����inode��Ŷ�Ӧ���¿��
	Directory b = Directory();
	writedirItem(&b.it[0], name);					//��ӱ���
	writeInBlock(newblocknum, (char*)&b);			//������д�ش���
}

void RMDIR(char* name){
	int id = superblock_mem.curind;

	int block[35];
	int cnt = FindBlockbyInode(id, block);			//ͨ��inode���Ѱ������飬���ؼ�¼������ŵ�����block

	for (int i = 0; i < cnt; i++){
		Directory x = Directory();
		readInBlock(block[i], (char*)&x);			//������i��
		for (int j = 0; j < 5; j++){
			//if (x.it[j].d_ino == -1) continue;
			if (strcmp(x.it[j].d_name, name) == 0){
				//����ƥ��
				int del_i = x.it[j].d_ino; 
				if (inode[del_i].file_type == DIRFILE){
					//��Ŀ¼�ļ�
					Directory w = Directory();
					readInBlock(inode[del_i].i_zone[0], (char*)&w);
					if (w.it[0].d_ino == -1){
						//�ǿ�Ŀ¼

						//Ҫ���滻�Ŀ� ���Ϊblock[i],���Ϊj
						//���һ��Ŀ��Ϊblock[cnt-1]
						Directory y = Directory();
						readInBlock(block[cnt-1], (char*)&y);			//�������һ��
						for (int k = 4; k >= 0; k--){

							if (y.it[k].d_ino != -1){
								//��ǰĿ¼�����һ��Ŀ¼
								
								//�ͷ�inode�Ϳ�
								ReleaseBlock(inode[x.it[j].d_ino].i_zone[0]);
								ReleaseInode(x.it[j].d_ino);

								x.it[j].d_ino = y.it[k].d_ino;
								strcpy(x.it[j].d_name, y.it[k].d_name);
								if (block[i] == block[cnt - 1]){
									y.it[j].d_ino = y.it[k].d_ino;
									strcpy(y.it[j].d_name, y.it[k].d_name);
								}
								y.it[k].d_ino = -1;    //ɾ�����һ���ڵ�
								strcpy(y.it[k].d_name, "");

								writeInBlock(block[cnt - 1], (char*)&y);			//д�����һ��

								if (k == 0 && cnt!=1){
									//�ƶ������һ�������һ��ĵ�һ�Ҫ�����һ��ɾ��
									if (cnt > 9){
										//һ�μ������
										Index xxx;
										readInBlock(inode[id].i_zone[9], (char*)&xxx);
										xxx.idx[cnt - 10] = -1;
										writeInBlock(inode[id].i_zone[9], (char*)&xxx);
									}
									else{
										//ֱ������
										inode[id].i_zone[cnt-1] = -1;
									}
								}
								if(block[i]!=block[cnt-1]) writeInBlock(block[i], (char*)&x);			//д�ص�block[i]��
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
	convertToAbs(name, abspath);		//�������dirת���ɾ���·��
	int id = getInodebyAbs(abspath);	//�õ����ļ���Ӧ��inode
	if (id == -1) return;
	if (inode[id].file_type != NORMALFILE){
		printf("It is not a file\n");
		return;
	}

	//�˴���Ȩ���ж�
	AddInOpenFileList(name,id);
	
}

void CLOSE(char* name){
	char abspath[100];
	convertToAbs(name, abspath);		//�������dirת���ɾ���·��
	int id = getInodebyAbs(abspath);	//�õ����ļ���Ӧ��inode
	if (id == -1) return;
	if (inode[id].file_type != NORMALFILE){
		printf("It is not a file\n");
		return;
	}

	SubInOpenFileList(id);
}

void WRITE(char* name){		//д�ļ�
	char abspath[100];
	convertToAbs(name, abspath);		//�������dirת���ɾ���·��
	int id = getInodebyAbs(abspath);	//�õ����ļ���Ӧ��inode
	if (id == -1) return;
	if (inode[id].file_type != NORMALFILE){
		printf("It is not a file\n");
		return;
	}
	if (!isOpen(id)){
		printf("The file is not open\n");
		return;
	}

	//�˴���Ȩ���ж�

	//���һ�����Ϣ�ᱻ���ϸ���
	getchar();		//������һ���س�

	int blocksize = 0;    //��������

	char str[1000];
	while (gets(str)){		//��һ��
		if (strcmp(str, "") == 0) break;  //�ļ�д��
		
		strcat(str, "\n");		//ĩλ����һ���س�

		int block[35];
		int cnt = FindBlockbyInode(id, block);   //�����ӵĿ��
		int lastblock = block[cnt - 1];			//���һ��������

		File a = File();
		readInBlock(lastblock, (char*)&a);		//�������һ�������


		int existlen = strlen(a.content);        //�Ѵ�ĳ���
		int leftlen = 100 - existlen-1;			 //ʣ��ĳ���
		int input_len = strlen(str);
		if (input_len <= leftlen){			//���봮�ܷ������һ���У�����������
			strcat(a.content, str);			//��content����������Ĵ�
			writeInBlock(lastblock, (char*)&a);			//д�ش���
			continue;
		}
		//��ss�н�ȡ����Ϊleftlen���ַ��������һ����
		char ss[1000];
		memset(ss, 0, sizeof(ss));
		strcut(ss, str, leftlen, 0);
		strcat(a.content,ss);

		//strncpy(a.content + existlen, str, leftlen);
		//��str����Ϊ��ȥǰleftlen���ȵ��ַ�����ʣ�µ�
		
		strcpy(ss, str);
		strcut(str, ss, strlen(ss) - leftlen,leftlen);

		int xxxx = strlen(str);

		writeInBlock(lastblock, (char*)&a);

		//��str���ѳ�ÿ100��һ�飬���10��
		while(strlen(str)>0){
			int newblock = AddBlockbyInode(id);
			if (newblock == -1){
				printf("File is out of range\n");
				//�˴������޸�ʱ��
				inode[id].file_mtime = GetCurrentTime();
				ModifyTimeinMem(id);					//���ڴ���ļ��򿪱��и������һ���޸�ʱ��
				inode[id].file_size += blocksize * 100;	//�޸��ļ��Ĵ�С
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
			char ss[1000];  //��ʱ����
			memset(ss, 0, sizeof(ss));
			strcpy(ss, str);
			strcut(str, ss, strlen(ss) - len,len);

			File aa = File();
			strcpy(aa.content, strblock);
			writeInBlock(newblock, (char*)&aa);     //����д�ش���
		}
	}

	//�˴������޸�ʱ��
	inode[id].file_mtime = GetCurrentTime();
	ModifyTimeinMem(id);					//���ڴ���ļ��򿪱��и������һ���޸�ʱ��
	inode[id].file_size += blocksize * 100;	//�޸��ļ��Ĵ�С

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
	convertToAbs(name, abspath);		//�������dirת���ɾ���·��
	int id = getInodebyAbs(abspath);	//�õ����ļ���Ӧ��inode
	if (id == -1) return;
	if (inode[id].file_type != NORMALFILE){
		printf("It is not a file\n");
		return;
	}

	if (!isOpen(id)){
		printf("The file is not open\n");
		return;
	}

	//�˴��ɼ�Ȩ���ж�

	int block[35];
	int cnt = FindBlockbyInode(id, block);
	for (int i = 0; i < cnt; i++){
		int bl = block[i];
		File f;
		readInBlock(bl, (char*)&f);
		printf("%s", f.content);
	}
}


void LOOKATOPENFILE(){							//�鿴�ڴ����ļ��򿪱�
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		printf("%s(%d) ", superblock_mem.OpenList[i].name,superblock_mem.OpenList[i].cnt);
	}
	printf("\n");
	return;
}

void RENAME(char* exname, char* name){				//������
	int curind = superblock_mem.curind;

	for (int i = 0; i < 9; i++){			//ֱ��
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
	int bl = inode[curind].i_zone[9];    //���
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
	//�ж�Ȩ���Ƿ�Ϸ�
	if (strlen(mod) != 9) return false;
	for (int i = 0; i < strlen(mod); i++){
		if (mod[i] != 'r' && mod[i] != 'w'&& mod[i] != '_')
			return false;
	}
	return true;
}

void CHMOD(char* name,char* mod){					//�޸�Ȩ��
	if (!isLegal(mod)){
		printf("mod is not legal\n");
		return;
	}
	int curind = superblock_mem.curind;

	for (int i = 0; i < 9; i++){					//ֱ��
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
	int bl = inode[curind].i_zone[9];				//���
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