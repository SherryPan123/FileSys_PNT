#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
int main(){
	FILE* fd;
	fd = fopen("a.txt", "r");
	if (fd == NULL) printf("File not open!\n");
	getchar();
	return 0;
}