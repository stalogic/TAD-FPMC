/*************************************************************************
	> File Name: tensor.h
	> Author:XQ 
	> Mail:houlaizhexq@163.com 
	> Created Time: 2015��01��15�� ������ 21ʱ24��20��
	> Function: 4ά�����Ļ������������ļ������ҵ�
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

//�÷ֶ�
class DeltaTime : public map<double , double>{};

//������һά�ȣ�Ҫ�Ƽ������(locCategory)=(deltaT,score)�÷ּ�ֵ�ԣ�ѵ�����÷�Ϊ���ֵĴ���
class LocCategory : public map<int , DeltaTime >{};

//�����ڶ�ά�ȣ�ǰ�ص��(preCategory,locCategory)=(deltaT,score)��ֵ��
class PreCategory : public map<int , LocCategory>{};

//��������ά�ȣ�ʱ��(time,preCategory,locCategory)=(deltaT,score)
class TimeCategory : public map<int , PreCategory>{};

//��������ά�ȣ��û�(user,timeCategory,preCategory,locCategory)=(deltaT,score)
class User : public map<int , TimeCategory>{
    public:
    //���ݵĴ�ѵ���ı���ȡ
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
		
        return 1;//�ɹ�
    }

	//ͳ�����е�����
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
		

		//ͳ�������Ϣ
		//user�����������û�����Ϣ
		for(iuser=(*this).begin();iuser!=(*this).end();iuser++){
			user.insert(iuser->first);
			//��ǰ�û�ĳ��ʱ��ε���Ϣ
			for(itimecategory=iuser->second.begin();itimecategory!=iuser->second.end();itimecategory++){
				timecate.insert(itimecategory->first);
				//��ǰ�û���ǰʱ��ε�ĳ��ǰ�ص���Ϣ
				for(ipreCategory=itimecategory->second.begin();ipreCategory!=itimecategory->second.end();ipreCategory++){
					precate.insert(ipreCategory->first);
					//ȷ���û���ȷ��ʱ��Σ�ȷ��ǰ�͵㣬�в�ͬʱ���������Ϣ
					for(ilocCategory=ipreCategory->second.begin();ilocCategory!=ipreCategory->second.end();ilocCategory++){
						loccacate.insert(ilocCategory->first);
						//�û���ʱ��Σ�ǰ�ص㣬ʱ��Σ���ĵص�
						for(ideltaTime=ilocCategory->second.begin();ideltaTime!=ilocCategory->second.end();ideltaTime++){
							deltaTime.insert(ideltaTime->first);
							//cout << iuser->first<<"\t"<<itimecategory->first<<"\t"<<ipreCategory->first<<"\t"<<ideltaTime->first<<"\t"<<ilocCategory->first<<":"<<ilocCategory->second<<endl;
						}
					}
				}
			}
		}
		cout<<"�û�������"<<user.size()<<endl;
		cout<<"ʱ��θ�����"<<timecate.size()<<endl;
		cout<<"ǰ�ص����������"<<precate.size()<<endl;
		cout<<"ʱ��������"<<deltaTime.size()<<endl;
		cout<<"��ص����������"<<loccacate.size()<<endl;
		for(int i=1;i<=762;i++){
			if(user.find(i)==user.end()){
				cout<<i<<" not found!"<<endl;
			}
		}
	}

    //���ݵĲ����ı�����,���������ݵ���֪������沢�÷�Ϊ1
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
    //���ݵĲ����ı�д��
    int toTestFile(string fileName){
        return 1;//good
    }
    
	//�����������ĸ�ά�ȵõ����Ӧ������
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