#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>  
#include <map>
#include <algorithm>  
#include <list> 
#include <math.h>   
#include <cmath> 
#include <time.h>
#include <stdio.h>  
#include <stdlib.h>
using namespace std;

//记录普通训练集数据及标签 
struct Item{
	double data[100];
	bool value;
	Item(){
		data[0] = 1;
	}
};
Item Train_set[5000];

//记录二次项高维训练集数据及标签 
struct Item_high_dim{
	double data[5000];
	bool value;
	Item_high_dim(){
		data[0] = 1;
	}
};
Item Train_set_high_dim[5000];

int cnt_col = 1;//记录总列数 
int cnt_row = 0;//记录总行数 
int cnt_col_high_dim = 0;

//向量乘法 
double vector_mul(double a[], double b[], int dim){
	double result = 0;
	for(int i = 0; i < dim; i++){
		result += a[i] * b[i];
	}
	return result;
}

//符号函数 
bool sign(double a){
	if(a > 0) return true;
	else return false;
}

//普通训练集数据评测指标计算 
double train_pingce_col(double w[]){
	int tp = 0, fn = 0, tn = 0, fp = 0;
	for(int i = 0; i < cnt_row; i++){
		if(sign(vector_mul(w, Train_set[i].data, cnt_col)) == Train_set[i].value){
			if(Train_set[i].value == true) tp++;
			else tn++;
		}else{
			if(Train_set[i].value == true) fn++;
			else fp++;
		}
	}
	double accuracy = double(tp+tn) / double(tp+fp+tn+fn);
	double recall = double(tp) / double(tp+fn);
	double precision = double(tp) / double(tp+fp);
	double f1 = double(2*precision*recall) / double(precision+recall);
	return f1;
}
	
//二次项高维训练集数据评测指标计算 
double train_pingce_col_high_dim(double w[]){	
	int tp = 0, fn = 0, tn = 0, fp = 0;
	for(int i = 0; i < cnt_row; i++){
		if(sign(vector_mul(w, Train_set_high_dim[i].data, cnt_col_high_dim)) == Train_set_high_dim[i].value){
			if(Train_set_high_dim[i].value == true) tp++;
			else tn++;
		}else{
			if(Train_set_high_dim[i].value == true) fn++;
			else fp++;
		}
	}
	double accuracy = double(tp+tn) / double(tp+fp+tn+fn);
	double recall = double(tp) / double(tp+fn);
	double precision = double(tp) / double(tp+fp);
	double f1 = double(2*precision*recall) / double(precision+recall);
	return f1;
}

//符号函数bool转double 
double value_bool_to_double(bool value){
	if(value == true) return 1.0;
	else if(value == false) return -1.0;
} 

//符号函数bool转int 
int value_bool_to_int(bool value){
	if(value == true) return 1;
	else if(value == false) return -1;
} 

int main(){
	clock_t start_time = clock();//记录程序运行时间 
	
	//训练集数据读取 
	ifstream train_file("train.csv");
    if(train_file){
    	string mydata_str;//记录读取字符串 
    	bool first_traversal = true;//用于记录列数时作临时判断变量 
	    while(getline(train_file, mydata_str)){
	    	
	    	//记录每行数据的value值 
	    	int pos = mydata_str.rfind(',');//反向寻找第一个Tab出现的位置 
	    	string value_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//截取值字符串 
	    	
			if(value_str == "1") Train_set[cnt_row].value = true;
	    	else if(value_str == "-1") Train_set[cnt_row].value = false;
	    	else cout << "Error" << endl;
			
			//记录每行数据的data值 
			mydata_str = mydata_str.substr(0, pos);//截取有效字符串
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = ","; //以空格作截取词 
	        char* data_tem_c;  
	        data_tem_c = strtok(mydata_c, d); //截取有效单词 
	        int col_tem = 1;//记录遍历到当前行的第几列 
	        while(data_tem_c){  
	            Train_set[cnt_row].data[col_tem] = atof(data_tem_c);//将字符串转换为double类型存入data 
	            col_tem++;//当前行列数++ 
	            if(first_traversal == true) cnt_col++;//如果是遍历第一行则总列数++		
	            data_tem_c = strtok(NULL, d);
	        }
	        first_traversal = false;
	        cnt_row++;
	    }
	    train_file.close();
	}else{
		cout << "open fail" << endl;
	}
	
	//训练集数据训练
	int method = 2;//选择训练模式 
	srand(time(0));//设置种子
//	cout<<"种子： "<< time(0) <<endl;
	double w0[5000];
	for(int i = 0; i < 5000; i++) w0[i] = 1.0;//初始化w值 
//	for(int i = 0; i < 5000; i++) w0[i] = double(rand()%100);//初始化w值 
	// 1508935028 30 
	double w0_accuracy = 0;
	double w1[5000];
	for(int i = 0; i < 5000; i++) w1[i] = w0[i];
	double w1_accuracy = 0;
	

	
	//原始PLA算法 
	if(method == 1){
		int iter = 5000;//设置迭代次数 
		while(iter--){
			//遍历全部训练集数据 
			for(int i = 0; i < cnt_row; i++){
				//判断预测值是否准确 
				if(sign(vector_mul(w0, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//更新权重向量 
					for(int j = 0; j < cnt_col; j++){
						w0[j] = w0[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
				}
			}
		}
	//口袋算法 
	}else if(method == 2){
		int iter = 2000;//设置迭代次数 
		double old_train_accuracy_col = train_pingce_col(w0);
		while(iter--){
			cout<<iter<<endl;
			//遍历全部训练集数据 
			for(int i = 0; i < cnt_row; i++){
				//判断预测值是否准确 
				if(sign(vector_mul(w1, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//更新权重向量 
					for(int j = 0; j < cnt_col; j++){
						w1[j] = w1[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
					//如果当前权重向量效果优于最优权重向量则更新最优权重向量 
					double new_train_accuracy_col= train_pingce_col(w1);
					if(new_train_accuracy_col > old_train_accuracy_col){	
						for(int j = 0; j < cnt_col; j++){
							w0[j] = w1[j];
						}
						old_train_accuracy_col = new_train_accuracy_col;
					}
				}
			}
		}
	//口袋算法（放松版） 
	}else if(method == 3){
		int iter = 2000;//设置迭代次数 
		double old_train_accuracy_col = train_pingce_col(w0);
		while(iter--){
			//遍历全部训练集数据 
			for(int i = 0; i < cnt_row; i++){
				//判断预测值是否准确 
				if(sign(vector_mul(w1, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//更新权重向量 
					for(int j = 0; j < cnt_col; j++){
						w1[j] = w1[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
				}
			}
			//如果当前权重向量效果优于最优权重向量则更新最优权重向量 
			double new_train_accuracy_col= train_pingce_col(w1);
			if(new_train_accuracy_col > old_train_accuracy_col){	
				for(int j = 0; j < cnt_col; j++){
					w0[j] = w1[j];
				}
				old_train_accuracy_col = new_train_accuracy_col;
			}
		}
	//口袋算法（收紧版） 
	}else if(method == 4){
		int iter = 5;//设置迭代次数 
		double old_train_accuracy_col = train_pingce_col(w0);
		while(iter--){
			//遍历全部训练集数据 
			for(int i = 0; i < cnt_row; i++){
				//判断预测值是否准确 
				if(sign(vector_mul(w0, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//记录更新后的权重向量 
					for(int j = 0; j < cnt_col; j++){
						w1[j] = w0[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
					//如果更新后的权重向量效果优于当前权重向量则更新当前权重向量 
					double new_train_accuracy_col= train_pingce_col(w1);
					if(new_train_accuracy_col > old_train_accuracy_col){	
						for(int j = 0; j < cnt_col; j++){
							w0[j] = w1[j];
						}
						old_train_accuracy_col = new_train_accuracy_col;
					}
					
				}
			}
		}
	//模拟退火口袋算法（收紧版）		
	}else if(method == 5){
		double T=100000;								//系统的温度，系统初始应该要处于一个高温的状态 
		double T_min=1;									//温度的下限，若温度T达到T_min，则停止搜索
		double dE;
		double r=0.999;								//用于控制降温的快慢
//		srand(time(0));
		srand(1508848158);//r=0.999 0.508453	
		cout<<"种子： "<< time(0) <<endl;
		double old_train_accuracy_col = train_pingce_col(w0);
		while(T > T_min){
			//遍历全部训练集数据 
			for(int i = 0; i < cnt_row; i++){
				//判断预测值是否准确 
				if(sign(vector_mul(w0, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//记录更新后的权重向量 
					for(int j = 0; j < cnt_col; j++){
						w1[j] = w0[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
					double new_train_accuracy_col = train_pingce_col(w1);
					
					dE = (new_train_accuracy_col - old_train_accuracy_col);//记录新旧权重函数值的差 
					//表达移动后得到更优解，则总是接受移动
					if(dE > 0){	
						for(int j = 0; j < cnt_col; j++){
							w0[j] = w1[j];
						}
						old_train_accuracy_col = new_train_accuracy_col;
					//表达移动不一定到更优解，则通过随机函数一定概率判断是否移动 
					}else if(exp(dE/T)>((rand()%11)/10.000)){
					//函数exp(dE/T)的取值范围是(0,1)，dE/T越大，则exp(dE/T)也越大	
						for(int j = 0; j < cnt_col; j++){
							w0[j] = w1[j];
						}
					}
					T=r*T;//降温退火 ，0<r<1 。r越大，降温越慢；r越小，降温越快
					/*
		 			若r过大，则搜索到全局最优解的可能会较高，但搜索的过程也就较长。
					若r过小，则搜索的过程会很快，但最终可能会达到一个局部最优值
		　　		*/ 
				}
			}	
		} 
	//二次项高维算法 
	}else if(method == 6){
		//遍历全部训练集数据将其转换为二次项高维数据 n+C(n，2) 
		for(int i = 0; i < cnt_row; i++){
			int dim = 0;
			Train_set_high_dim[i].value=Train_set[i].value;
			Train_set_high_dim[i].data[0] = 1;//向量扩展 
			dim++;
			//n个为一次项 
			for(int j = 1; j < cnt_col; j++){
				Train_set_high_dim[i].data[dim] = Train_set[i].data[j];
				dim++;
			}
			//C(n，2)个为二次项 
			for(int j = 1; j < cnt_col; j++){
				for(int k = j; k < cnt_col; k++){
					Train_set_high_dim[i].data[dim] = Train_set[i].data[j] * Train_set[i].data[k];
					dim++;
				}
			}
			cnt_col_high_dim = dim;
		}
		int iter = 5;//设置迭代次数 
		double old_train_accuracy_col_high_dim=train_pingce_col_high_dim(w0);
		while(iter--){
			//遍历全部训练集数据
			for(int i = 0; i < cnt_row; i++){
				//判断预测值是否准确 
				if(sign(vector_mul(w1, Train_set_high_dim[i].data, cnt_col_high_dim)) != Train_set_high_dim[i].value){
					//更新权重向量 
					for(int j = 0; j < cnt_col_high_dim; j++){
						w1[j] = w1[j] + Train_set_high_dim[i].data[j] * value_bool_to_double(Train_set_high_dim[i].value);
					}
					//如果当前权重向量效果优于最优权重向量则更新最优权重向量 
					double new_train_accuracy_col_high_dim=train_pingce_col_high_dim(w1);
					if(new_train_accuracy_col_high_dim > old_train_accuracy_col_high_dim){	
						for(int j = 0; j < cnt_col_high_dim; j++){
							w0[j] = w1[j];
						}
						old_train_accuracy_col_high_dim = new_train_accuracy_col_high_dim;
					}
				}
			}
		}
	}
	
	//计算训练集评测指标 
	int tp = 0, fn = 0, tn = 0, fp = 0;
	for(int i = 0; i < cnt_row; i++){
		if(sign(vector_mul(w0, Train_set[i].data, cnt_col)) == Train_set[i].value){
			if(Train_set[i].value == true) tp++;
			else tn++;
		}else{
			if(Train_set[i].value == true) fn++;
			else fp++;
		}
	}
	double accuracy = double(tp+tn) / double(tp+fp+tn+fn);
	double recall = double(tp) / double(tp+fn);
	double precision = double(tp) / double(tp+fp);
	double f1 = double(2*precision*recall) / double(precision+recall);

	cout << "train: " <<endl; 
	cout << "accuracy: " << accuracy << endl;
	cout << "recall: " << recall << endl;
	cout << "precision: " << precision << endl;
	cout << "f1: " << f1 << endl;
	cout << endl;
	
	
	//测试集数据
	ifstream test_file;
//	test_file.open("val.csv");
	test_file.open("test.csv");

	ofstream result_file;
	result_file.open("test_result.csv");
	
    if(test_file && result_file){
    	Item Test_set;
    	Item Test_set_high_dim;
    	bool isVal = false;
		tp = 0;fn = 0;tn = 0;fp = 0;
    	
    	string mydata_str;//记录读取字符串 
	    while(getline(test_file, mydata_str)){
	    	
	    	//记录每行数据的value值 
	    	int pos = mydata_str.rfind(',');//反向寻找第一个Tab出现的位置 
	    	string value_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//截取值字符串 
	    	
	    	if(isVal == true){
	    		if(value_str == "1") Test_set.value = true;
		    	else if(value_str == "-1") Test_set.value = false;
		    	else cout << "Error" << endl;
			}
			
			//记录每行数据的data值 
			mydata_str = mydata_str.substr(0, pos);//截取有效字符串
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = ","; //以空格作截取词 
	        char* data_tem_c;  
	        data_tem_c = strtok(mydata_c, d); //截取有效单词 
	        int col_tem = 1;//记录遍历到当前行的第几列 
	        while(data_tem_c){

	            Test_set.data[col_tem] = atof(data_tem_c);//将字符串转换为double类型存入data 

	            col_tem++;//当前行列数++ 
	            	
	            data_tem_c = strtok(NULL, d);
	        }
			
			bool result_value;
			//判断是否为二次项高维算法 
			if(method == 6){
				//遍历全部验证集集数据将其转换为二次项高维数据 n+C(n，2) 
				int dim = 0;
				Test_set_high_dim.value=Test_set.value;
				Test_set_high_dim.data[0] = 1;//向量扩展 
				dim++;
				//n个为一次项 
				for(int j = 1; j < cnt_col; j++){
					Test_set_high_dim.data[dim] = Test_set.data[j];
					dim++;
				}
				//C(n，2)个为二次项 
				for(int j = 1; j < cnt_col; j++){
					for(int k = j; k < cnt_col; k++){
						Test_set_high_dim.data[dim] = Test_set.data[j] * Test_set.data[k];
						dim++;
					}
				}
				//进行预测并记录各项评测指标 
				result_value = sign(vector_mul(w0, Test_set_high_dim.data, cnt_col_high_dim));
				if(isVal == true){
					if(result_value == Test_set_high_dim.value){
						if(Test_set_high_dim.value == true) tp++;
						else tn++;
					}else{
						if(Test_set_high_dim.value == true) fn++;
						else fp++;
					}
				}
			}else{
				//进行预测并记录各项评测指标 
				result_value = sign(vector_mul(w0, Test_set.data, cnt_col));
				if(isVal == true){
					if(result_value == Test_set.value){
						if(Test_set.value == true) tp++;
						else tn++;
					}else{
						if(Test_set.value == true) fn++;
						else fp++;
					}
				}
			}
			//输出预测结果 
			result_file << value_bool_to_int(result_value) << endl;
	    }
	    
		if(isVal == true){
			//计算验证集评测指标 
			accuracy = double(tp+tn) / double(tp+fp+tn+fn);
			recall = double(tp) / double(tp+fn);
			precision = double(tp) / double(tp+fp);
			f1 = double(2*precision*recall) / double(precision+recall);
		
			cout << "val: " << endl; 
			cout << "accuracy: " << accuracy << endl;
			cout << "recall: " << recall << endl;
			cout << "precision: " << precision << endl;
			cout << "f1: " << f1 << endl;
			cout << endl;
		}
		
		result_file.close();
	    test_file.close();
	}else{
		cout << "open fail" << endl;
	}
	clock_t finish_time=clock();//记录程序运行时间 
	double totaltime=double(finish_time-start_time)/CLOCKS_PER_SEC;
	cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<endl;
	return 0;  
} 
