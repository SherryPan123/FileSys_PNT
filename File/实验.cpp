#include <iostream>
#include <cstdio>
#include <ctime> 
#include <cstring> 
using namespace std;
struct DirectoryItem
{
	int  d_ino;     	         //该目录项对应的索引节点号
	char   d_name[16];	         //该目录项对应的文件的名称
};
struct Directory{
	DirectoryItem it[5];
};
char data[10000];
void LS(char abs);
void readInBlock(int blockNum, char *a) {
	memcpy(a, data+blockNum*100, 100);
	cout << ((Directory *)a)->it[1].d_ino << endl;
	cout << ((Directory *)a)->it[1].d_name << endl;
}

void writeInBlock(int blockNum, char *a) {
	cout << ((Directory *)a)->it[1].d_ino << endl;
	cout << ((Directory *)a)->it[1].d_name << endl;
	memcpy(data+blockNum*100, a, 100);	
}

int main(){
	/*Directory b;
	
	b.it[1].d_ino = 1055;
	strcpy(b.it[1].d_name, "PNT");
	cout << b.it[1].d_ino << endl;
	cout << b.it[1].d_name << endl;
	writeInBlock(5, (char *)&b); 
	
	Directory a;
	readInBlock(5, (char *)&a);
	cout << a.it[1].d_ino << endl;
	cout << a.it[1].d_name << endl;*/
	time_t lt;
lt = time(NULL);
printf(ctime(&lt)); 

	return 0;
} 
