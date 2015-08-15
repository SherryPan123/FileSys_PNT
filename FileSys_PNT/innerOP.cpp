#include "FileSys.h"


time_t GetCurrentTime();									//��ȡ��ǰʱ��
void ModifyTimeinMem(int id);								//���ڴ���ļ��򿪱��и������һ���޸�ʱ��
void AddInOpenFileList(char* name, int id);					//���Ѵ��ڣ�����ӣ�inode[id].i_count++(���ô���+1)
void SubInOpenFileList(int id);								//���ļ��򿪱��и�����-1
void DeleteInOpenFileList(int id);							//������ɾ������,���򱨴�

void writedirItem(DirectoryItem* x, char* name);			//���½���Ŀ¼д�����
void writefiletodirItem(DirectoryItem* x, char* name);		//���½����ļ�д�����
char* strcut(char* dst, char *src, int n, int m);			//nΪ���ȣ�mΪλ�� 
int getInodebyAbs(char* absPath);							//��������·�������ض�Ӧinode

bool isOpen(int ind);										//�жϸ��ļ��Ƿ񱻴�

void PrintFileInfo(DirectoryItem a);						//����ļ���Ϣ

void writedirItem(DirectoryItem* x, char* name){		//���½���Ŀ¼д�����
	int newid;
	newid = x->d_ino = GetNextInodeNum();		//��ȡinode���
	strcpy(x->d_name, name);					//inode��Ŷ�ӦĿ¼��
	inode[newid].CreateInode(newid, 1, superblock_mem.curuserid, DIRFILE, "xxxxxxxx");    //������inode

	time_t ct;
	ct = time(NULL);
	inode[newid].file_ctime = ct;			//����ʱ��
	inode[newid].file_mtime = ct;			//�ϴ��޸�ʱ��

	int newblock;
	newblock = inode[newid].i_zone[0] = GetNextBlockNum();		//��ȡ������
	Directory xx = Directory();
	writeInBlock(newblock, (char*)&xx);
}

void writefiletodirItem(DirectoryItem* x, char* name){		//���½����ļ�д�����
	int newid;
	newid = x->d_ino = GetNextInodeNum();					//��ȡinode���
	strcpy(x->d_name, name);								//inode��Ŷ�Ӧ�ļ���
	inode[newid].CreateInode(newid, 1 * 100, superblock_mem.curuserid, NORMALFILE, "rwwrwwrww");    //������inode
	
	time_t ct;
	ct = time(NULL);
	inode[newid].file_ctime = ct;
	inode[newid].file_mtime = ct;
	inode[newid].i_count = 0;

	int newblock;
	newblock = inode[newid].i_zone[0] = GetNextBlockNum();	 //��ȡ������
	File xx = File();
	writeInBlock(newblock, (char*)&xx);
}

int getInodebyAbs(char* absPath){							//��������·�������ض�Ӧinode
	if (strcmp(absPath, "/") == 0) return 0;				//����·��Ϊ��Ŀ¼

	int cnt = 0;
	int start = 1;
	char str[100][15];     //���100�㣬�ļ����15


	if (absPath[0] != '/'){
		printf("Path is illegal!\n");
		return -1;
	}

	//�õ���һ��Ŀ¼��
	for (int i = 1; i < strlen(absPath); i++){
		if (absPath[i] == '/'){
			strncpy(str[cnt], absPath + start, i - start);
			str[cnt][i - start] = '\0';
			cnt++;
			int x = strlen(str[cnt - 1]);
			start = i + 1;
		}
	}
	strncpy(str[cnt], absPath + start, strlen(absPath) - start);   //���һ��Ŀ¼
	str[cnt][strlen(absPath) - start] = '\0';
	cnt++;

	//int x = strlen(str[cnt - 1]);

	//bool find = false;
	int curind = 0;    //��ǰĿ¼��Ӧ��inode
	for (int i = 0; i < cnt; i++){
		if (inode[curind].file_type != DIRFILE){   //����Ŀ¼�ļ�
			printf("Path is illegal!\n");
			return -1;
		}

		bool flag = false;
		//ֱ������
		for (int j = 0; j < 9; j++){
			int bl = inode[curind].i_zone[j];
			if (bl == -1) break;    //������Ŀ¼���ļ�
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
		//һ�μ������
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

char* strcut(char* dst, char *src, int n, int m){//nΪ���ȣ�mΪλ�� 
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


void AddInOpenFileList(char* name,int id){			//���Ѵ��ڣ�����ӣ�inode[id].i_count++(���ô���+1)
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		if (superblock_mem.OpenList[i].id == id){
			superblock_mem.OpenList[i].cnt++;
			return;
		}
	}
	superblock_mem.OpenList.push_back(OpenFileListItem(name, id, inode[id].file_ctime, inode[id].file_mtime));
	inode[id].i_count++;
}

void SubInOpenFileList(int id){							//���ļ��򿪱��и�����-1
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

void DeleteInOpenFileList(int id){	//������ɾ������
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

void ModifyTimeinMem(int id){		//���ڴ���ļ��򿪱��и������һ���޸�ʱ��
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
	printf(" %d\n", inode[file_i].file_size);         //����ļ���Ϣ
}


bool isOpen(int ind){
	for (int i = 0; i < superblock_mem.OpenList.size(); i++){
		if (superblock_mem.OpenList[i].id == ind){
			return true;
		}
	}
	return false;
}