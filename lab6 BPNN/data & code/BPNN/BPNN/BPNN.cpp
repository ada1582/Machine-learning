// BPNN.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
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
#include <string>
#include <vector>  
using namespace std;

//记录普通训练集数据及标签 
struct Item {
	double data[100];
	int value;
	Item() {
		data[0] = 1.0;
	}
};
Item Train_set[10000];//记录训练数据
int Train_col;//记录总列数 
int Train_row;//记录总行数

double w_i2h[100][100];//记录输入层到隐藏层的权重向量
double w_h2o[100];//记录隐藏层到输出层的权重向量

double hidden_set[100];//记录隐藏层数据
int hidden_set_num = 30;//记录隐藏层个数
int input_set_num;//记录输入层个数

/*
训练集数据输入处理
输入：交叉验证法分割分数、训练数据文件
*/
void Train_file_input(int file_split) {
	//初始化各项数据
	Train_row = 0;//记录总行数
	Train_col = 0;//记录总列数

	//读取训练集数据文件
	ifstream train_file("train_special.csv");
	int tem_row_split = 0;
	if (train_file) {
		string train_str;
		getline(train_file, train_str);
		while (getline(train_file, train_str)) {
			//根据交叉验证的分割份数读取文件
			//if (!((file_split * 1000) <= tem_row_split && tem_row_split < ((file_split + 1) * 1000))) {
			if (true) {
				//记录每行数据的标签值 
				int pos = train_str.rfind(',');//反向寻找第一个Tab出现的位置 
				string value_str = train_str.substr(pos + 1, train_str.length() - pos - 1);//截取值字符串 
				char* value_c_str = (char*)value_str.c_str();
				Train_set[Train_row].value = atof(value_c_str);

				//记录每行数据的特征值与标签值
				train_str = train_str.substr(0, pos);//截取有效字符串
				char* train_c_str = (char*)train_str.c_str();
				const char* d = ",";//以空格作截取词 
				char* tem_c = strtok(train_c_str, d);//截取有效单词 
				int tem_col = 1;
				Train_set[Train_row].data[0] = 1;
				while (tem_c) {
					//存储数据特征值,判断是否进行减少分叉操作
					Train_set[Train_row].data[tem_col] = atof(tem_c);
					tem_c = strtok(NULL, d);
					tem_col++;
				}
				Train_col = tem_col;
				Train_row++;
			}
			tem_row_split++;
		}
		input_set_num = Train_col - 1;
		train_file.close();
	}
	else {
		std::cout << "open fail" << endl;
	}
	/*cout << "训练集矩阵为：" << endl;
	for (int i = 0; i < Train_row; i++) {
		for (int j = 0; j < Train_col; j++) {
			cout << Train_set[i].data[j] << " ";
		}
		cout << Train_set[i].value << " ";
		cout << endl;
	}
	cout << endl;*/
}

//初始化权重向量
void initialize_w() {
	for (int j = 0; j <= hidden_set_num; j++) {
		w_h2o[j] = (rand() % 100) / 10000.0;
		if (j != 0) {
			for (int i = 0; i <= input_set_num; i++) {
				w_i2h[i][j] = (rand() % 100) / 10000.0;
				//(rand() % 100) / 10000.0
			}
		}
	}
}

//前向传播：从输入层到隐藏层
void forward_pass_i2h(double data[]) {
	hidden_set[0] = 1.0;
	for (int j = 1; j <= hidden_set_num; j++) {
		hidden_set[j] = 0.0;
		for (int i = 0; i <= input_set_num; i++) {
			hidden_set[j] += data[i] * w_i2h[i][j];
		}
		//激活函数Relu
		//if (hidden_set[j] > 0) hidden_set[j] = hidden_set[j];
		//else hidden_set[j] = hidden_set[j] * 0.5;
		//激活函数sigmod
		hidden_set[j] = 1 / (1 + exp(-hidden_set[j]));
	}
}

//前向传播：从隐藏层到输出层
double forward_pass_h2o() {
	double output = 0;
	for (int j = 0; j <= hidden_set_num; j++) {
		output += hidden_set[j] * w_h2o[j];
	}
	return output;
}

//反向传播：更新权重向量
void backward_pass(double output, Item set, int iter) {
	//动态步长
	double alpha1 = (double)iter / 8000.0;
	double alpha2 = (double)iter / 8000.0;
	if (iter < 800) {
		alpha1 = 0.1;
		alpha2 = 0.1;
	}
	//静态步长
	//double alpha1 = 0.01;
	//double alpha2 = 0.01;

	//计算误差梯度更新从输入层到隐藏层的权重向量
	for (int j = 1; j <= hidden_set_num; j++) {
		//if (hidden_set[j] > 0) tem_grad2 = tem_grad1 * w_h2o[j];
		//else tem_grad2 = tem_grad2 * 0.5 * w_h2o[j];
		for (int i = 0; i <= input_set_num; i++) {
			double hidden_errors_grad = (output - (double)set.value) * hidden_set[j] * (1.0 - hidden_set[j]) * w_h2o[j] * set.data[i];
			w_i2h[i][j] = w_i2h[i][j] - hidden_errors_grad * alpha1  / (double)Train_row;
		}
	}
	//计算误差梯度更新从从隐藏层到输出层的权重向量
	for (int j = 0; j <= hidden_set_num; j++) {
		double output_errors_grad = (output - (double)set.value) * hidden_set[j];
		w_h2o[j] = w_h2o[j] - output_errors_grad * alpha2 / (double)Train_row;
	}
}


//标签预测
int Test_label_Cal(double data[])
{
	forward_pass_i2h(data);
	return (int)forward_pass_h2o();
}

/*
测试集数据输入处理
输入：交叉验证法分割分数、测试数据文件
输出：准确率
*/
double Test_file_input(int file_split) {
	ifstream test_file("test_special.csv");
	//ifstream test_file("test.csv");
	ofstream result_file("15352237_liushunyu.txt");
	if (test_file) {
		double msn = 0;
		int test_row = 0;
		string test_str;

		//读取测试集数据文件
		int tem_row_split = 0;
		getline(test_file, test_str);
		while (getline(test_file, test_str)) {
			//根据交叉验证的分割份数读取文件
			//if (((file_split) * 1000) <= tem_row_split && tem_row_split < ((file_split + 1) * 1000)) {
			if (true) {
				Item Test_set;
				Test_set.data[0] = 1;
				//记录每行数据的value值 
				int pos = test_str.rfind(',');//反向寻找第一个Tab出现的位置 
				//string value_str = test_str.substr(pos + 1, test_str.length() - pos - 1);//截取值字符串 
				//char* value_c_str = (char*)value_str.c_str();
				//Test_set.value = atof(value_c_str);


				/*记录每行数据的特征值与标签值*/
				test_str = test_str.substr(0, pos);//截取值字符串 
				char* test_c_str = (char*)test_str.c_str();
				const char* d = ",";//以空格作截取词 
				char* tem_c = strtok(test_c_str, d);//截取有效单词 
				int tem_col = 1;
				while (tem_c) {
					//存储数据特征值,判断是否进行减少分叉操作
					Test_set.data[tem_col] = atof(tem_c);
					tem_c = strtok(NULL, d);
					tem_col++;
				}
				int test_label = Test_label_Cal(Test_set.data); //测试集预测
				//result_file << test_label << ","  << Test_set.value << endl;
				result_file << test_label << endl;
				//cout << "预测标签为："< <test_label << endl;
				//msn += pow((double)((double)test_label - (double)Test_set.value), 2);
				test_row++;
			}
			tem_row_split++;
		}
		//msn = msn / test_row;
		//std::cout << "msn: " << msn << endl;
		result_file.close();
		test_file.close();
		return msn;
		//return 0;
	}
	else {
		std::cout << "open fail" << endl;
		return 0;
	}
}

void update_w(int file_split) {
	int method = 2;
	int iter = 1000;
	//ofstream result_file("msn_train.csv");
	srand(time(0));
	initialize_w();

	while (iter--) {
		cout << "iter" << iter << endl;
		double msn_train = 0;

		//正常神经网络
		if (method == 1) {
			for (int i = 0; i < Train_row; i++) {
				forward_pass_i2h(Train_set[i].data);//前向传播：从输入层到隐藏层
				double output = forward_pass_h2o();//前向传播：从隐藏层到输出层
				msn_train += pow(((double)Train_set[i].value - output), 2);//计算msn
				backward_pass(output, Train_set[i], iter);//反向传播：更新权重向量
			}
			msn_train = msn_train / (double)Train_row;
		}
		//Mini-batch随机批处理神经网络
		else if (method == 2) {
			int Train_rand_num = 1000;//定义批梯度下降训练集个数
			int exit[10000] = { 0, };
			for (int i = 0; i < Train_rand_num;) {
				int rand_i = rand() % Train_row;//随机抽取训练集数据
				if (exit[rand_i] == 0) {
					exit[rand_i] = 1;
					forward_pass_i2h(Train_set[rand_i].data);//前向传播：从输入层到隐藏层
					double output = forward_pass_h2o();//前向传播：从隐藏层到输出层
					msn_train += pow(((double)Train_set[rand_i].value - output), 2);//计算msn
					backward_pass(output, Train_set[rand_i], iter);//反向传播：更新权重向量
					i++;
				}
			}
			msn_train = msn_train / (double)Train_rand_num;
		}

		cout << msn_train << endl;
		//double msn_test = Test_file_input(file_split);
		//result_file << msn_train << "," << msn_test << endl;
	}
	//result_file.close();
}

int main() {
	clock_t start_time = clock();//记录程序运行时间 

	double avg_msn = 0;
	for (int i = 1; i < 2; i++) {//交叉验证法
		Train_file_input(i);//训练集数据处理
		update_w(i);//训练
		avg_msn += Test_file_input(i);
	}
	avg_msn = avg_msn / 9;
	std::cout << "avg_msn: " << avg_msn << endl;
	clock_t finish_time = clock();//记录程序运行时间 
	double totaltime = double(finish_time - start_time) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	return 0;
}
