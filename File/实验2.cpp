#include <iostream>
#include <ctime>
#include <cstdio>
using namespace std;
//目录项
struct DirectoryItem{
	int  d_ino;     	         //该目录项对应的索引节点号
	char d_name[16];	         //该目录项对应的文件的名称
	DirectoryItem(){
		d_ino = -1;
	}
};

//目录
struct Directory{			 //物理块存的是目录，每块的大小为100
	DirectoryItem it[5];
	Directory(){
		for (int i = 0; i < 5; i++)
			it[i] = DirectoryItem();
	}
};
time_t GetCurrentTime();									//获取当前时间

time_t GetCurrentTime(){
	time_t tl;
	tl = time(NULL);
	return tl;
}

int main(){
	//Directory a = Directory();
	//cout<<a.it[1].d_ino<<endl; 
	
	char ss[1000];
	scanf("%s",ss); 
	for(int i=0;i<10000;i++){
	}
	t = GetCurrentTime();
	printf(ctime(&t));
	return 0;
} 
