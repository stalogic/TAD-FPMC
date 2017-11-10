#include<iostream>
#include<fstream>
#include<time.h>
#include"tensor.h"
#include"matrix.h"
#include"pitf.h"

using namespace std;

int main(){
	srand (time(NULL));//设置随机数种子
	User userTrain;
	User userTest;
	struct tm *ptr;
	time_t it=time(NULL);
	ptr=localtime(&it);
	printf("start time:%4d-%02d-%02d %d:%d:%d\n",ptr->tm_year+1900,ptr->tm_mon+1,ptr->tm_mday,ptr->tm_hour,ptr->tm_min,ptr->tm_sec);
	
	cout<<"start reading datesets..."<<endl;
	if(userTrain.fromTrainFile("train.txt")==1){
		cout<<"read file OK!"<<endl;
		LocRecPITF *pitf=new LocRecPITF();
		pitf->init(&userTrain);
		cout<<"init ok!"<<endl;
		pitf->train();
		cout<<"train ok!"<<endl;
	}else{
		cout<<"failed to read file!"<<endl;
	}

	it=time(NULL);
	ptr=localtime(&it);
	printf("end time:%4d-%02d-%02d %d:%d:%d\n",ptr->tm_year+1900,ptr->tm_mon+1,ptr->tm_mday,ptr->tm_hour,ptr->tm_min,ptr->tm_sec);
}
