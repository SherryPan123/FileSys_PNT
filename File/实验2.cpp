#include <iostream>
#include <ctime>
#include <cstdio>
using namespace std;
//Ŀ¼��
struct DirectoryItem{
	int  d_ino;     	         //��Ŀ¼���Ӧ�������ڵ��
	char d_name[16];	         //��Ŀ¼���Ӧ���ļ�������
	DirectoryItem(){
		d_ino = -1;
	}
};

//Ŀ¼
struct Directory{			 //���������Ŀ¼��ÿ��Ĵ�СΪ100
	DirectoryItem it[5];
	Directory(){
		for (int i = 0; i < 5; i++)
			it[i] = DirectoryItem();
	}
};
time_t GetCurrentTime();									//��ȡ��ǰʱ��

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
