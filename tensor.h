/*************************************************************************
	> File Name: tensor.h
	> Author:XQ 
	> Mail:houlaizhexq@163.com 
	> Created Time: 2015年01月15日 星期四 21时24分20秒
	> Function: 4维张量的基本操作，读文件，查找等
 ************************************************************************/

#ifndef _tensor_h_ 
#define _tensor_h_ 

#include<fstream>
#include<string>
#include<cstring>
#include<stdio.h>
#include<map>
#include<set>
#include<iostream>

using namespace std;

//得分对
class DeltaTime : public map<double , double>{};

//张量第一维度：要推荐种类的(locCategory)=(deltaT,score)得分键值对，训练集得分为出现的次数
class LocCategory : public map<int , DeltaTime >{};

//张量第二维度：前地点点(preCategory,locCategory)=(deltaT,score)键值对
class PreCategory : public map<int , LocCategory>{};

//张量第三维度：时间(time,preCategory,locCategory)=(deltaT,score)
class TimeCategory : public map<int , PreCategory>{};

//张量第四维度：用户(user,timeCategory,preCategory,locCategory)=(deltaT,score)
class User : public map<int , TimeCategory>{
    public:
    //数据的从训练文本获取
    int fromTrainFile(string fileName){
        ifstream instream(fileName.c_str());
        if(instream){
            int userTemp,timeCategoryTemp,preTemp,locTemp;//temp variables for user,time#,pre,loc,
            double deltaTTemp;//temp variable for deltaT
            string line="";
            while(getline(instream,line)){
                sscanf(line.c_str(),"%d\t%d\t%d\t%lf\t%d",&userTemp,&timeCategoryTemp,&preTemp,&deltaTTemp,&locTemp);
				if((*this)[userTemp][timeCategoryTemp][preTemp][locTemp][deltaTTemp]){
					(*this)[userTemp][timeCategoryTemp][preTemp][locTemp][deltaTTemp]+=1;
				}else{
					(*this)[userTemp][timeCategoryTemp][preTemp][locTemp][deltaTTemp]=1;
				}
            }
			instream.close();
        }else{
            cout<<"can not read train file!"<<endl;
            return 0;
	    }
		
        return 1;//成功
    }

	//统计所有的数据
	void show(){
		User::iterator iuser;// user iterator
		TimeCategory::iterator itimecategory;//time category iterator
		PreCategory::iterator ipreCategory;//pre location iterator
		LocCategory::iterator ilocCategory;//location iterator
		DeltaTime::iterator ideltaTime;//deltaT iterator

		set<int> user;
		set<int> timecate;
		set<int> precate;
		set<int> loccacate;
		set<double> deltaTime;
		

		//统计相关信息
		//user迭代器插入用户的信息
		for(iuser=(*this).begin();iuser!=(*this).end();iuser++){
			user.insert(iuser->first);
			//当前用户某个时间段的信息
			for(itimecategory=iuser->second.begin();itimecategory!=iuser->second.end();itimecategory++){
				timecate.insert(itimecategory->first);
				//当前用户当前时间段的某个前地点信息
				for(ipreCategory=itimecategory->second.begin();ipreCategory!=itimecategory->second.end();ipreCategory++){
					precate.insert(ipreCategory->first);
					//确定用户，确定时间段，确定前低点，中不同时间间隔后的信息
					for(ilocCategory=ipreCategory->second.begin();ilocCategory!=ipreCategory->second.end();ilocCategory++){
						loccacate.insert(ilocCategory->first);
						//用户，时间段，前地点，时间段，后的地点
						for(ideltaTime=ilocCategory->second.begin();ideltaTime!=ilocCategory->second.end();ideltaTime++){
							deltaTime.insert(ideltaTime->first);
							//cout << iuser->first<<"\t"<<itimecategory->first<<"\t"<<ipreCategory->first<<"\t"<<ideltaTime->first<<"\t"<<ilocCategory->first<<":"<<ilocCategory->second<<endl;
						}
					}
				}
			}
		}
		cout<<"用户个数："<<user.size()<<endl;
		cout<<"时间段个数："<<timecate.size()<<endl;
		cout<<"前地点种类个数："<<precate.size()<<endl;
		cout<<"时间差个数："<<deltaTime.size()<<endl;
		cout<<"后地点种类个数："<<loccacate.size()<<endl;
		for(int i=1;i<=762;i++){
			if(user.find(i)==user.end()){
				cout<<i<<" not found!"<<endl;
			}
		}
	}

    //数据的测试文本读入,将测试数据的已知结果保存并得分为1
    int fromTestFile(string fileName){
		ifstream instream(fileName.c_str());
        if(instream){
            int userTemp,timeCategoryTemp,preTemp,locTemp;//temp variables for user,time#,pre,loc,
            double deltaTTemp;//temp variable for deltaT
            string line="";
            while(getline(instream,line)){
                sscanf(line.c_str(),"%d\t%d\t%d\t%lf\t%d",&userTemp,&timeCategoryTemp,&preTemp,&deltaTTemp,&locTemp);
				(*this)[userTemp][timeCategoryTemp][preTemp][locTemp][deltaTTemp]=1;
            }
			instream.close();
        }else{
            cout<<"can not read train file!"<<endl;
            return 0;
	    }
		
        return 1;//good
    }
    //数据的测试文本写入
    int toTestFile(string fileName){
        return 1;//good
    }
    
	//根据张量的四个维度得到相对应的数据
    int getScore(int guser,int gtimecate, int gpreloccate, int gloccate,double *deltaTime,double *score){
		
		User::iterator iuser;// user iterator
		TimeCategory::iterator itimecategory;//time category iterator
		PreCategory::iterator ipreCategory;//pre location iterator
		LocCategory::iterator ilocCategory;//location iterator
		DeltaTime::iterator ideltaTime;//deltaT iterator

		if((iuser=(*this).find(guser))!=(*this).end()){
			cout<<guser<<" find!"<<endl;			
			if((itimecategory=iuser->second.find(gtimecate))!=iuser->second.end()){
				cout<<gtimecate<<" find!"<<endl;
				if((ipreCategory=itimecategory->second.find(gpreloccate))!=itimecategory->second.end()){
					cout<<gpreloccate<<" find!"<<endl;
					if((ilocCategory=ipreCategory->second.find(gloccate))!=ipreCategory->second.end()){
						cout<<gloccate<<" find!"<<endl;
						for(ideltaTime=ilocCategory->second.begin();ideltaTime!=ilocCategory->second.end();ideltaTime++){
							*deltaTime=ideltaTime->first;
							*score=ideltaTime->second;
							return 1;
						}
					}else{
						cout<<gloccate<<"not find!"<<endl;
						return 0;
					}
				}else{
					cout<<gpreloccate<<"not find!"<<endl;
					return 0;
				}
			}else{
				cout<<gtimecate<<"not find!"<<endl;
				return 0;
			}
		}else{
			cout<<guser<<"not find!"<<endl;
			return 0;
		}
    }
};

#endif 