/*************************************************************************
	> File Name: pitf.h
	> Author:XQ 
	> Mail:houlaizhexq@163.com 
	> Created Time: 2015年1月19日 星期一
	> Function: 张量的分解方法pitf
 ************************************************************************/
#ifndef _pitf_h_
#define _pitf_h_ 

#include<map>
#include<fstream>
#include<iostream>
#include<string.h>
#include"matrix.h"
#include"tensor.h"

int LOSS_FUNCTION_SIGMOID = 0;
int LOSS_FUNCTION_LN_SIGMOID = 1;

class LocRecPITF{
public:
	LocRecPITF(){}

	//定义每行的数据结构，u-t-p-l-deltaT
	struct DataCase{
		int userIdcase;
		int timeCateCase;
		int preCase;
		int locCase;
		double deltaCase;
	};

	User *datesets;//训练集

	DataCase *dataCase;//
	int numDataCase;//总的u-t-p-l对数

	DataCase *testdataCase;//测试集u-t-p-l
	int numtestdataCase;//总的u-t-p-l对数

	map<int,double> locacate;//存储所有地点信息

	DMatrixDouble u_l, l_u, t_l, l_t, p_l, l_p;//各大参数矩阵
	std::map<int, double> locCateScore;//存储地点的得分

	int maxUserId, maxTimeId, maxPreCateId, maxLocCateId;//各种的最大id，为建立矩阵做准备！！！！！

	int num_neg_samples;//负样本数量，默认100
	int num_iterations ;//迭代次数，默认100
	double learn_rate;//学习率
	double init_mean;//均值
	double init_stdev;// 方差
	int num_feature ;//特征个数，默认64
	double regular ;//正则化参数，默认0

	//求导lnsigmoid的外层导数
	double partial_loss(int loss_function, double x) {
		if (loss_function == LOSS_FUNCTION_SIGMOID) {
           	double sigmoid_tp_tn = (double) 1/(1+exp(-x));
       		return sigmoid_tp_tn*(1-sigmoid_tp_tn);//sigmoid函数的导数
		} else if (loss_function == LOSS_FUNCTION_LN_SIGMOID) {//使用lnsigmoid函数进行训练
			double exp_x = exp(-x);
     		return exp_x / (1 + exp_x);
     	} else {
			assert((loss_function == LOSS_FUNCTION_LN_SIGMOID) || (loss_function == LOSS_FUNCTION_SIGMOID));	
		}             			
		return 0;
	}

	//初始化，
	void init(User *userTrain) {		
		datesets = userTrain;//训练集
		numDataCase=0;//训练集总共的行数		

		//各种最大的序号，用来产生矩阵所用
		maxUserId=0;
		maxTimeId=0; 
		maxPreCateId=0;
		maxLocCateId=0;

		//给负样本，迭代次数，学习率，均值，方差，特征数目，正则化参数赋初值
		num_neg_samples = 100;
		num_iterations = 100;
		learn_rate = 0.01;
		init_mean=8;
		init_stdev=1;
		num_feature =64;
		regular =0;

		//定义user,time#,pre#,loc#,deltaT的迭代器
		User::iterator iuser;// user iterator
		TimeCategory::iterator itimecategory;//time category iterator
		PreCategory::iterator ipreCategory;//pre location iterator
		LocCategory::iterator ilocCategory;//location iterator
		DeltaTime::iterator ideltaTime;//deltaT iterator		

		//计算所有的numDataCase数，即所有的u-t-p-l-deltaT对
		for(iuser=datesets->begin();iuser!=datesets->end();iuser++){
			for(itimecategory=iuser->second.begin();itimecategory!=iuser->second.end();itimecategory++){
				for(ipreCategory=itimecategory->second.begin();ipreCategory!=itimecategory->second.end();ipreCategory++){
					for(ilocCategory=ipreCategory->second.begin();ilocCategory!=ipreCategory->second.end();ilocCategory++){
						numDataCase += ilocCategory->second.size();
					}
				}
			}
		}

		//根据所有的数量，定义训练数据对的大小
		dataCase = new DataCase[numDataCase];
		int cntr=0;//记录插入数据时候的行数

		//更新最大的user,time#,pre#,loc#的最大id，以便建立参数矩阵
		//将所有数据插入到定义的训练数据集的数据结构中
		for(iuser=datesets->begin();iuser!=datesets->end();iuser++){
			if(maxUserId < iuser->first){maxUserId = iuser->first;}//更新最大user id
			for(itimecategory=iuser->second.begin();itimecategory!=iuser->second.end();itimecategory++){
				if(maxTimeId < itimecategory->first){maxTimeId = itimecategory->first;}//更新time#id
				for(ipreCategory=itimecategory->second.begin();ipreCategory!=itimecategory->second.end();ipreCategory++){					
					if(maxPreCateId < ipreCategory->first){maxPreCateId = ipreCategory->first;}//更新pre#id
					for(ilocCategory=ipreCategory->second.begin();ilocCategory!=ipreCategory->second.end();ilocCategory++){						
						//将所有出现的地点存储到locacate中
						locacate[ilocCategory->first]=0.0;												
						if(maxLocCateId < ilocCategory->first){maxLocCateId = ilocCategory->first;}//更新loca#id
						for(ideltaTime=ilocCategory->second.begin();ideltaTime!=ilocCategory->second.end();ideltaTime++){
							if(cntr==numDataCase){cout<<"out bound!";}
							//将数据插入到训练数据所有列表中，用于随机产生样本点
							dataCase[cntr].userIdcase=iuser->first;
							dataCase[cntr].timeCateCase=itimecategory->first;
							dataCase[cntr].preCase=ipreCategory->first;
							dataCase[cntr].locCase=ilocCategory->first;
							dataCase[cntr].deltaCase=ideltaTime->first;
							cntr++;
						}
					}
				}
			}
		}

		//输出相关信息
		cout<<"max user id:"<<maxUserId<<endl;
		cout<<"max time category id:"<<maxTimeId<<endl;
		cout<<"max prelocation catgory id:"<<maxPreCateId<<endl;
		//cout<<"deltaT number:："<<deltaTime.size()<<endl;
		cout<<"max location category id:"<<maxLocCateId<<endl;

		//设置参数矩阵的大小
		this->u_l.setSize(maxUserId+1, num_feature);
		this->l_u.setSize(maxLocCateId+1, num_feature);
		this->t_l.setSize(maxTimeId+1, num_feature);
		this->l_t.setSize(maxLocCateId+1, num_feature);
		this->p_l.setSize(maxPreCateId+1, num_feature);
		this->l_p.setSize(maxLocCateId+1, num_feature);

		//初始化参数矩阵
		this->u_l.init(init_mean, init_stdev);
		this->l_u.init(init_mean, init_stdev);
		this->t_l.init(init_mean, init_stdev);
		this->l_t.init(init_mean, init_stdev);
		this->p_l.init(init_mean, init_stdev);
		this->l_p.init(init_mean, init_stdev);

		//记录初始产生测矩阵用于做对比
		ofstream f1("o_u_l.txt"); 
		ofstream f2("o_l_u.txt"); 
		ofstream f3("o_t_l.txt"); 
		ofstream f4("o_l_t.txt"); 
		ofstream f5("o_p_l.txt"); 
		ofstream f6("o_l_p.txt"); 
		
		for(int i=0;i<u_l.dim1;i++){
			for(int j=0;j<u_l.dim2;j++){
				f1<<u_l(i,j)<<"\t";
			}
			f1<<endl;
		}
		f1.close();

		for(int i=0;i<l_u.dim1;i++){
			for(int j=0;j<l_u.dim2;j++){
				f2<<l_u(i,j)<<"\t";
			}
			f2<<endl;
		}
		f2.close();

		for(int i=0;i<t_l.dim1;i++){
			for(int j=0;j<t_l.dim2;j++){
				f3<<t_l(i,j)<<"\t";
			}
			f3<<endl;
		}
		f3.close();

		for(int i=0;i<l_t.dim1;i++){
			for(int j=0;j<l_t.dim2;j++){
				f4<<l_t(i,j)<<"\t";
			}
			f4<<endl;
		}
		f4.close();

		for(int i=0;i<p_l.dim1;i++){
			for(int j=0;j<p_l.dim2;j++){
				f5<<p_l(i,j)<<"\t";
			}
			f5<<endl;
		}

		for(int i=0;i<l_p.dim1;i++){
			for(int j=0;j<l_p.dim2;j++){
				f6<<l_p(i,j)<<"\t";
			}
			f6<<endl;
		}
		f6.close();

	}

	//产生预测函数，也就是两个隐式矩阵进行点乘,用于训练只用
	double predict(int userId, int timeId, int preId, int locId, std::map<double, double> deltaT_score) {
		double result = 0;
		double u_dot_l = 0;
		double t_dot_l = 0;
		double p_dot_l = 0;
        
		//进行预测
		for (int f = 0; f < num_feature; f++) {
			double temp_p_dot_l=0;
			u_dot_l += u_l(userId,f) * l_u(locId, f);
			t_dot_l += t_l(timeId,f) * l_t(locId, f);
            for(std::map<double , double >::iterator i = deltaT_score.begin(); i != deltaT_score.end(); i++){
                double d = exp(-0.04 * i->first);
                temp_p_dot_l = d * p_l(preId,f)*l_p(locId,f);
            }
			//p_dot_l += p_l(preId,f) * l_p(locId, f);
			p_dot_l += temp_p_dot_l/deltaT_score.size();
			if(deltaT_score.size()<1) {
				cout<<deltaT_score.size()<<"\t"<<temp_p_dot_l<<endl;
			}
		}
		result += u_dot_l + t_dot_l + p_dot_l;	
		//cout<<u_dot_l<<"\t"<<t_dot_l<<"\t"<<p_dot_l<<"\t"<<result<<endl;	
		return result;
	}

	//产生预测函数，也就是两个隐式矩阵进行点乘，用于测试,因为测试的时候，只是知道u,t,p,l,deltaT
	double predict(int userId, int timeId, int preId, int locId, double deltaT) {
		double result = 0;
		double u_dot_l = 0;
		double t_dot_l = 0;
		double p_dot_l = 0;
        	double d = exp(-0.04 * deltaT);
        	for (int f = 0; f < num_feature; f++) {
			u_dot_l += u_l(userId,f) * l_u(locId, f);
			t_dot_l += t_l(timeId,f) * l_t(locId, f);
			p_dot_l += d * (p_l(preId,f) * l_p(locId, f));

		}
		result += u_dot_l + t_dot_l + p_dot_l;		
		return result;
	}

	int valid(int userId, int timeId, int preId, int locId, int negLocId, std::map<double, double> deltaT_score) {
		double x_utplp = predict(userId, timeId, preId, locId, deltaT_score);//产生正样本的得分
		double x_utpln = predict(userId, timeId, preId, negLocId, deltaT_score);//产生负样本的得分
		return x_utplp > x_utpln ? 1 : 0;
	}

	void check() {
		long long num_draws_per_iteration = (int)(numDataCase / 1);
		int all = 0;
		int right = 0;
		for(int draw = 0; draw < num_draws_per_iteration; draw++) {
			int r  = rand() % numDataCase;
			int u  = dataCase[r].userIdcase;
			int t  = dataCase[r].timeCateCase;
			int p = dataCase[r].preCase;
			int lp = dataCase[r].locCase;
			double d = dataCase[r].deltaCase;

			//得到所有p-l对，以便于提取负样本
			LocCategory loccategory = (*datesets)[u][t][p];
			int ln = drawlocNeg(lp, loccategory);

			//学习参数
			right += valid(u, t, p, lp, ln, (*datesets)[u][t][p][lp]);
			all++;
		}
		cout << "accuracy:" << right << "/" << all << "=" << (double)right / all << endl;
	}

	//参数学习的过程
	void learn(int userId, int timeId, int preId, int locId, int negLocId, std::map<double, double> deltaT_score) {
		double x_utplp = predict(userId, timeId, preId, locId, deltaT_score);//产生正样本的得分
        double x_utpln = predict(userId, timeId, preId, negLocId, deltaT_score);//产生负样本的得分
		//以下部分为：正样本分数-负样本分数最大化的过程

		//得到导数,ln(sigmoid)的前半部分
		double normalizer = partial_loss(1, x_utplp - x_utpln);
        //更新参数
        for (int f = 0; f < num_feature; f++) {
			//cout<<"learn"<<f<<endl;
			//各大特征的当前值
			double u_l_f = u_l(userId, f);
			double t_l_f = t_l(timeId, f);
			double p_l_f = p_l(preId, f);
			double l_u_f_p = l_u(locId, f);
			double l_t_f_p = l_t(locId, f);
			double l_p_f_p = l_p(locId, f);
			double l_u_f_n = l_u(negLocId, f);
			double l_t_f_n = l_t(negLocId, f);
			double l_p_f_n = l_p(negLocId, f);

            //update parameters with deltatime information
            u_l(userId, f) += learn_rate * (normalizer * (l_u_f_p - l_u_f_n) - regular * u_l(userId, f));
			t_l(timeId, f) += learn_rate * (normalizer * (l_t_f_p - l_t_f_n) - regular * t_l(timeId, f));
			l_u(locId, f) +=  learn_rate * (normalizer * (u_l_f) - regular * l_u(locId, f));
			l_t(locId, f) += learn_rate * (normalizer * (t_l_f) - regular * l_t(locId, f));
			l_u(negLocId, f) +=  learn_rate * (normalizer * (-u_l_f) - regular * l_u(negLocId, f));
			l_t(negLocId, f) +=  learn_rate * (normalizer * (-t_l_f) - regular * l_t(negLocId, f));
            double temp_p_l=0;
            double temp_l_p_p=0;
            double temp_l_p_n=0;
            for(std::map<double,double >::iterator i=deltaT_score.begin();i!=deltaT_score.end();i++){
                double d = exp(-0.4 * (i->first));
                temp_p_l += d * (l_p_f_p - l_p_f_n );
                temp_l_p_p += d * p_l_f;
                temp_l_p_n += d * (-p_l_f);
            }
            int dss = deltaT_score.size();
            p_l(preId, f) += learn_rate * (normalizer * temp_p_l/dss - regular * p_l(preId, f));
            l_p(locId,f) += learn_rate * (normalizer * temp_l_p_p/dss - regular * l_p(locId, f));
            l_p(negLocId,f) += learn_rate * (normalizer * temp_l_p_n/dss - regular * l_p(negLocId, f));
			if(isnan(u_l(userId,f))) cout<<"NAN"<<endl;
        }
   	}

	bool isnan(double d) {
		return d!=d;
	}
	//负样本的产生
	int drawlocNeg(int locId, LocCategory loccate) {
		int l_n;
		do {//循环条件：产生的负样本在当前P-L对中或者产生的负样本不在所有出现过的地点信息中或者产生的负样本等于正样本
			l_n = rand() % (maxLocCateId+1);
		} while ((loccate.find(l_n) != loccate.end()) || (locacate.find(l_n) == locacate.end())|| l_n == locId);
		return l_n;
	}

	//train训练
	void train() {
		long long num_draws_per_iteration = numDataCase * 1;//正样本数目*每个正样本数比较的负样本数（默认100）
		for (int iteration = 0; iteration < 100; iteration++) {//进行100次迭代
			cout<<"train outer"<<iteration<<endl;
			for (int draw = 0; draw < num_draws_per_iteration; draw++) {//迭代次数为更新一次全部数据所需要的比较次数
				//cout<<"train inner:"<<draw<<endl;
				// 产生样本点(u, t, p, l_p, l_n)
				// 随机产生需要的数据
				int r  = rand() % numDataCase;
				int u  = dataCase[r].userIdcase;
				int t  = dataCase[r].timeCateCase;
				int p = dataCase[r].preCase;
				int lp = dataCase[r].locCase;
				double d = dataCase[r].deltaCase;

				//得到所有p-l对，以便于提取负样本
				LocCategory loccategory= (*datesets)[u][t][p];
				int ln = drawlocNeg(lp,loccategory);

				//学习参数
				learn(u, t, p, lp, ln, (*datesets)[u][t][p][lp]);
			}
			check();
		}

		ofstream f1("u_l.txt"); 
		ofstream f2("l_u.txt"); 
		ofstream f3("t_l.txt"); 
		ofstream f4("l_t.txt"); 
		ofstream f5("p_l.txt"); 
		ofstream f6("l_p.txt"); 
		
		for(int i=0;i<u_l.dim1;i++){
			for(int j=0;j<u_l.dim2;j++){
				f1<<u_l(i,j)<<"\t";
			}
			f1<<endl;
		}
		f1.close();

		for(int i=0;i<l_u.dim1;i++){
			for(int j=0;j<l_u.dim2;j++){
				f2<<l_u(i,j)<<"\t";
			}
			f2<<endl;
		}
		f2.close();

		for(int i=0;i<t_l.dim1;i++){
			for(int j=0;j<t_l.dim2;j++){
				f3<<t_l(i,j)<<"\t";
			}
			f3<<endl;
		}
		f3.close();

		for(int i=0;i<l_t.dim1;i++){
			for(int j=0;j<l_t.dim2;j++){
				f4<<l_t(i,j)<<"\t";
			}
			f4<<endl;
		}
		f4.close();

		for(int i=0;i<p_l.dim1;i++){
			for(int j=0;j<p_l.dim2;j++){
				f5<<p_l(i,j)<<"\t";
			}
			f5<<endl;
		}

		for(int i=0;i<l_p.dim1;i++){
			for(int j=0;j<l_p.dim2;j++){
				f6<<l_p(i,j)<<"\t";
			}
			f6<<endl;
		}
		f6.close();
		cout<<"fuck!finish!"<<endl;
	}
};

#endif
