// LR.cpp: 定义控制台应用程序的入口点。
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
	double data[50];
	int value;
	Item() {
		data[0] = 1.0;
	}
};
Item Train_set[10000];
double w[50];
int Train_col;//记录总列数 
int Train_row;//记录总行数 

//向量乘法 
double vector_mul(double a[], double b[], int dim) {
	double result = 0;
	for (int i = 0; i < dim; i++) {
		result += a[i] * b[i];
	}
	return result;
}

//标签预测
int Test_label_Cal(double data[])
{
	if (1 / (1 + exp(-vector_mul(data, w, Train_col))) > 0.5) return 1;
	else return 0;
}

/*
训练集数据输入处理
输入：交叉验证法分割分数、训练数据文件
*/
void Train_file_input(int file_split) {
	//初始化各项数据
	Train_row = 0;//记录总行数
	Train_col = 0;//记录总列数

	//读取训练集数据文件
	//ifstream train_file("1.csv");
	ifstream train_file("train.csv");
	int tem_row_split = 0;
	if (train_file) {
		string train_str;
		while (getline(train_file, train_str)) {
			//根据交叉验证的分割份数读取文件
			if (!((file_split * 1000) <= tem_row_split && tem_row_split < ((file_split + 1) * 1000))) {
			//if (true) {
				//记录每行数据的标签值 
				int pos = train_str.rfind(',');//反向寻找第一个Tab出现的位置 
				string value_str = train_str.substr(pos + 1, train_str.length() - pos - 1);//截取值字符串 
				if (value_str == "1") Train_set[Train_row].value = 1;
				else if (value_str == "0") Train_set[Train_row].value = 0;
				else cout << "Error" << endl;

				//记录每行数据的特征值与标签值
				train_str = train_str.substr(0, pos);//截取有效字符串
				char* train_c_str = (char*)train_str.c_str();
				const char* d = ",";//以空格作截取词 
				char* tem_c = strtok(train_c_str, d);//截取有效单词 
				int tem_col = 1;
				while (tem_c) {
					//存储数据特征值,判断是否进行减少分叉操作
					Train_set[Train_row].data[tem_col]= atof(tem_c);
					tem_c = strtok(NULL, d);
					tem_col++;
				}
				Train_col = tem_col;
				Train_row++;
			}
			tem_row_split++;
		}
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
		cout << endl;
	}
	cout << endl;*/
}

/*
权重更新函数
输入：更新方法、训练集数据
*/
void Train_update_w(int method) {
	for (int i = 0; i < 50; i++) w[i] = 1.0;//初始化w值 
	srand(time(0));
	//for(int i = 0; i < 50; i++) w[i] = double(rand()%1);//初始化w值
	double min_gradient_avg = 10000;

	//普通逻辑回归
	if (method == 1) {
		int iter = 5000;//设置迭代次数 
		double alpha = 0.0001;
		while (iter--) {
			//动态步长
			alpha = double(iter) * 0.0000002;

			//计算梯度
			double wx[10000];
			for (int i = 0; i < Train_row; i++) {
				wx[i] = vector_mul(w, Train_set[i].data, Train_col);
			}
			double w_new[50];
			double gradient_avg = 0;
			for (int j = 0; j < Train_col; j++) {
				double gradient = 0;
				for (int i = 0; i < Train_row; i++) {
					double logistic = exp(wx[i]) / (1 + exp(wx[i])) - Train_set[i].value;
					gradient = gradient + logistic * Train_set[i].data[j];
				}
				w_new[j] = w[j] - alpha * gradient;
				gradient_avg += abs(gradient);
			}

			//计算梯度向量平均值
			gradient_avg = gradient_avg / Train_col;
			if (min_gradient_avg > gradient_avg) min_gradient_avg = gradient_avg;
			
			//如果梯度向量平均值小于某阈值则不再迭代
			if (gradient_avg > 800) {
				for (int i = 0; i < Train_col; i++) {
					w[i] = w_new[i];
				}
			}
			else {
				break;
			}
		}
		cout << min_gradient_avg << endl;
	}
	//随机梯度下降
	else if (method == 2) {
		int iter = 350000;//设置迭代次数 
		double alpha = 0.0001;
		int tem_row = 0;
		while (iter--) {
			//动态步长
			alpha = double(iter) * 0.000000002;

			//计算随机梯度
			double wx;
			tem_row = rand() % Train_row;
			wx = vector_mul(w, Train_set[tem_row].data, Train_col);
			double w_new[50];
			for (int j = 0; j < Train_col; j++) {
				double gradient = 0;
				double logistic = exp(wx) / (1 + exp(wx)) - Train_set[tem_row].value;
				gradient = gradient + logistic * Train_set[tem_row].data[j];
				w_new[j] = w[j] - alpha * gradient;
			}
			
			//更新权重
			for (int i = 0; i < Train_col; i++) {
				w[i] = w_new[i];
			}
		}
	}
	//正则化
	else if (method == 3) {
		int iter = 5000;//设置迭代次数 
		double alpha = 0.0001;
		double beta = 0.5;
		while (iter--) {
			//动态步长
			alpha = double(iter) * 0.0000002;

			//计算正则化梯度
			double wx[10000];
			for (int i = 0; i < Train_row; i++) {
				wx[i] = vector_mul(w, Train_set[i].data, Train_col);
			}
			double w_new[50];
			double gradient_avg = 0;
			for (int j = 0; j < Train_col; j++) {
				double gradient = 0;
				for (int i = 0; i < Train_row; i++) {
					double logistic = exp(wx[i]) / (1 + exp(wx[i])) - Train_set[i].value;
					gradient = gradient + logistic * Train_set[i].data[j];
				}
				w_new[j] = w[j] - alpha * (gradient + beta * w[j]);
				gradient_avg += abs(alpha * gradient);
			}

			//计算梯度向量平均值
			gradient_avg = gradient_avg / Train_col;
			if (min_gradient_avg > gradient_avg) min_gradient_avg = gradient_avg;

			//如果梯度向量平均值小于某阈值则不再迭代
			if (gradient_avg > 0.5) {
				for (int i = 0; i < Train_col; i++) {
					w[i] = w_new[i];
				}
			}
			else {
				break;
			}
		}
		cout << min_gradient_avg << endl;
	}
}

/*
测试集数据输入处理
输入：交叉验证法分割分数、测试数据文件
输出：准确率
*/
double Test_file_input(int file_split) {
	ifstream test_file("train.csv");
	//ifstream test_file("2.csv");
	//ifstream test_file("test.csv");
	//ofstream result_file("15352237_liushunyu.txt");
	if (test_file) {
		int corret_num = 0;
		int test_row = 0;
		string test_str;
		
		//读取测试集数据文件
		int tem_row_split = 0;
		while (getline(test_file, test_str)) {
			//根据交叉验证的分割份数读取文件
			if ((file_split * 1000) <= tem_row_split && tem_row_split < ((file_split + 1) * 1000)) {
			//if (true) {
				Item Test_set;

				//记录每行数据的value值 
				int pos = test_str.rfind(',');//反向寻找第一个Tab出现的位置 
				string value_str = test_str.substr(pos + 1, test_str.length() - pos - 1);//截取值字符串 
				if (value_str == "1") Test_set.value = 1;
				else if (value_str == "0") Test_set.value = 0;
				else cout << "Error" << endl;


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
				//result_file << test_label << endl;
				//cout << "预测标签为："<<test_label << endl;
				if (test_label == Test_set.value) corret_num++;//记录正确个数
				test_row++;
			}
			tem_row_split++;
		}
		double corret_rate = double(corret_num) / double(test_row) * 100;//计算准确率

		std::cout << "corret rate: " << corret_rate << "%" << endl;
		//result_file.close();
		test_file.close();
		return corret_rate;
		//return 0;
	}
	else {
		std::cout << "open fail" << endl;
		return 0;
	}
}

int main() {
	clock_t start_time = clock();//记录程序运行时间 
	int method = 3;
	double avg_corret_rate = 0;
	for (int i = 0; i < 8; i++) {//交叉验证法
		Train_file_input(i);//训练集数据处理
		Train_update_w(method);
		avg_corret_rate += Test_file_input(i);//测试决策树
	}
	avg_corret_rate = avg_corret_rate / 8;
	std::cout << "avg_corret_rate: " << avg_corret_rate << "%" << endl;


	clock_t finish_time = clock();//记录程序运行时间 
	double totaltime = double(finish_time - start_time) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
    return 0;
}