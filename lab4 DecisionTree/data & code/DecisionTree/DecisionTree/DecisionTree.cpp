// DecisionTree.cpp: 定义控制台应用程序的入口点。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>  
#include <map>
#include <algorithm>  
#include <list> 
#include <math.h>   
#include <vector>
#include <time.h>
#include <stdio.h>  
#include <stdlib.h>
using namespace std;

/*训练集数据*/
vector< vector<int> > Train_data;//记录训练数据集
vector< int > Train_property;//记录训练特征集
map< int, vector<int> > Train_property_value;//记录训练集每个特征的特征值的所有可能取值
int Train_col;//记录总列数 
int Train_row;//记录总行数 

/*决策树节点定义*/
struct DecisionTree_node {
	int property;//记录特征
	int property_value;//记录特征值
	int label_true_num;//记录当前结点下标签为1的数据集数量
	int label_false_num;//记录当前结点下标签为-1的数据集数量
	int error_num = 0;//记录叶子结点多数投票情况下的错误数量
	vector<DecisionTree_node*> child;//存储子节点
	/*默认构造函数定义*/
	DecisionTree_node() {
		property = -1;
		property_value = -1;
		label_true_num = 0;
		label_false_num = 0;
		error_num = 0;
	}
};

/*
最佳决策特征选择函数
输入：数据集、特征集、增益模型选择项
输出：最佳决策特征项
*/
int choose_property(vector< vector<int> > current_data, vector<int> current_property, int model) {

	int current_data_cnt[100][10] = { 0, };		//记录数据每个特征值的数量 
	int current_data_cnt_true[100][10] = { 0, };	//记录数据每个特征值相对于标签为true的数量
	int current_row = current_data.size();	//记录数据集的行数

	/*模型数据*/
	double H_D_A[9] = { 0, };
	double Gain_D_A[9] = { 0, };
	double SplitInfo[9] = { 0, };
	double GainRatio[9] = { 0, };
	double Gini[9] = { 0, };

	/*对数据集进行数据统计处理*/
	int cnt_all_true = 0;
	for (int i = 0; i < current_row; i++) {
		if (current_data[i][Train_col - 1] == 1) cnt_all_true++;
		for (int j = 0; j < Train_col - 1; j++) {
			current_data_cnt[current_data[i][j]][j]++;
			if (current_data[i][Train_col - 1] == 1) {
				current_data_cnt_true[current_data[i][j]][j]++;
			}
		}
	}

	/*-----------------------------------------------------ID3模型-----------------------------------------------------*/
	if (model == 1 || model == 2) {
		int max_Gain_index = current_property[0];
		double p_d = double(cnt_all_true) / double(current_row);
		double H_D = -p_d * (log(p_d) / log(2)) - (1 - p_d) * (log(1 - p_d) / log(2));

		for (vector <int>::iterator i = current_property.begin(); i < current_property.end(); i++) {
			for (int j = 0; j < 100; j++) {
				if (current_data_cnt[j][*i] != 0 && current_data_cnt_true[j][*i] != current_data_cnt[j][*i] 
					&& current_data_cnt_true[j][*i] != 0) {
					double p_a = double(current_data_cnt[j][*i]) / double(current_row);
					double p_a_d = double(current_data_cnt_true[j][*i]) / double(current_data_cnt[j][*i]);
					H_D_A[*i] += p_a * (-p_a_d * (log(p_a_d) / log(2)) - (1 - p_a_d) * (log(1 - p_a_d) / log(2)));
				}
			}
			Gain_D_A[*i] = H_D - H_D_A[*i];
			if (Gain_D_A[*i] > Gain_D_A[max_Gain_index]) max_Gain_index = *i;
		}

		if (model == 1) {
			/*for (vector <int>::iterator i = current_property.begin(); i < current_property.end(); i++) {
				std::cout << "H_D_A[" << *i << "]: " << H_D_A[*i] << "  ";
				std::cout << "Gain_D_A[" << *i << "]: " << Gain_D_A[*i] << endl;
			}
			std::cout << "root index: " << max_Gain_index << endl;
			std::cout << endl;*/
			return max_Gain_index;
		}
	}


	/*-----------------------------------------------------C4.5模型-----------------------------------------------------*/
	if (model == 2) {
		int max_GainRatio_index = current_property[0];
		int tem_col = 0;
		for (vector <int>::iterator i = current_property.begin(); i < current_property.end(); i++) {
			for (int j = 0; j < 100; j++) {
				if (current_data_cnt[j][*i] != 0) {
					double p_a = double(current_data_cnt[j][*i]) / double(current_row);
					SplitInfo[*i] += -p_a * (log(p_a) / log(2));
				}
			}
			GainRatio[*i] = Gain_D_A[*i] / SplitInfo[*i];
			if (GainRatio[*i] > GainRatio[max_GainRatio_index]) max_GainRatio_index = *i;
		}

		/*for (vector <int>::iterator i = current_property.begin(); i < current_property.end(); i++) {
			std::cout << "SplitInfo[" << *i << "]: " << SplitInfo[*i] << "  ";
			std::cout << "GainRatio[" << *i << "]: " << GainRatio[*i] << endl;
		}
		std::cout << "root index: " << max_GainRatio_index << endl;
		std::cout << endl;*/
		return 	max_GainRatio_index;
	}

	/*-----------------------------------------------------CART模型-----------------------------------------------------*/
	if (model == 3) {
		int max_Gini_index = current_property[0];
		double Gini_j[100][9] = { 0, };
		for (vector <int>::iterator i = current_property.begin(); i < current_property.end(); i++) {
			for (int j = 0; j < 100; j++) {
				if (current_data_cnt[j][*i] != 0) {
					double p_a_d = double(current_data_cnt_true[j][*i]) / double(current_data_cnt[j][*i]);
					Gini_j[j][*i] = 1 - pow(p_a_d, 2) - pow((1 - p_a_d), 2);
					double p_a = double(current_data_cnt[j][*i]) / double(current_row);
					Gini[*i] += p_a * Gini_j[j][*i];
				}
			}
			if (Gini[*i] < Gini[max_Gini_index]) max_Gini_index = *i;
		}
		/*for (vector <int>::iterator i = current_property.begin(); i < current_property.end(); i++) {
			std::cout << "Gini[" << *i << "]: " << Gini[*i] << endl;
		}
		std::cout << "root index: " << max_Gini_index << endl;
		std::cout << endl;*/
		return max_Gini_index;
	}
}

/*
数据集标签最多项统计函数
输入：数据集
输出：占比数据集最多的标签值及数量
*/
int MostLabel(vector< vector<int> > current_data) {
	int label_true = 0;//记录数据集中标签为1的数量
	int label_false = 0;//记录数据集中标签为-1的数量
	for (int i = 0; i < current_data.size(); i++) {
		if (current_data[i][Train_col - 1] == 1) label_true++;
		else label_false++;
	}
	if (label_true > label_false) return (label_true + 1);
	else return (-1 - label_false);
}

/*
节点数据集标签数量统计函数
输入：当前节点、当前节点的数据集
输出：定义了标签数量的当前节点
*/
DecisionTree_node* Label_num_cnt(DecisionTree_node* current_node, vector< vector<int> > current_data) {
	int label_true = 0;//记录数据集中标签为1的数量
	int label_false = 0;//记录数据集中标签为-1的数量
	for (int i = 0; i < current_data.size(); i++) {
		if (current_data[i][Train_col - 1] == 1) label_true++;
		else label_false++;
	}
	current_node->label_true_num = label_true;
	current_node->label_false_num = label_false;
	return current_node;
}

/*
叶子节点数量统计函数
输入：当前节点
输出：当前节点下的叶子节点个数
*/
int Leaves_num_cnt(DecisionTree_node* current_node) {
	if (current_node->property == -1) return 1;//如果是叶子结点则返回1
	else {//如果不是叶子节点则递归求和子节点的叶子结点个数
		int tem_leaves_num = 0;
		for (int i = 0; i < current_node->child.size(); i++) {
			tem_leaves_num += Leaves_num_cnt(current_node->child[i]);
		}
		return tem_leaves_num;
	}
}

/*
错误数量统计函数
输入：当前节点
输出：当前节点下的错误数量
*/
int Error_num_cnt(DecisionTree_node* current_node) {
	if (current_node->property == -1) return current_node->error_num;//如果是叶子结点则返回叶子结点多数投票情况下的错误数量
	else {//如果不是叶子节点则递归求和子节点的错误个数
		int tem_error_num = 0;
		for (int i = 0; i < current_node->child.size(); i++) {
			tem_error_num += Error_num_cnt(current_node->child[i]);
		}
		return tem_error_num;
	}
}

/*
边界判断函数：是否全部数据集只有一种标签
输入：数据集
输出：否或标签值
*/
int meet_with_bound_OnlyOneLabel(vector< vector<int> > current_data) {
	int label = current_data[0][Train_col - 1];
	for (int i = 1; i < current_data.size(); i++) {
		if (label != current_data[i][Train_col - 1])
			return 0;//如果全部数据集不只有一种标签则返回0
	}
	return label;
}

/*
边界判断函数：特征集是否为空
输入：数据集、特征集
输出：否或占比数据集最多的标签值及数量
*/
int meet_with_bound_EmptyProperty(vector< vector<int> > current_data, vector<int> current_property) {
	if (current_property.size() != 0) return 0;
	else return MostLabel(current_data);
}

/*
边界判断函数：数据集是否为空
输入：数据集
输出：否或占比数据集最多的标签值及数量
*/
int meet_with_bound_EmptyData(vector< vector<int> > current_data) {
	if (current_data.size() != 0) return 0;
	else return MostLabel(current_data);
}

/*
决策树生成函数
输入：当前节点、数据集、特征集、增益模型选择项
输出：当前节点决策树
*/
DecisionTree_node* BuildDecisionTree(DecisionTree_node* current_node, vector< vector<int> > current_data, 
	vector<int> current_property, int model,int depth,bool BeforePruning) {
	//如果当前节点为空则为其分配内存空间
	if (current_node == NULL) {
		current_node = new DecisionTree_node();
	}
	
	current_node = Label_num_cnt(current_node, current_data);//节点数据集标签数量统计
	
	//边界判断是否全部数据集只有一种标签
	int bound1 = meet_with_bound_OnlyOneLabel(current_data);
	if (bound1 != 0) {
		//生成叶子节点
		DecisionTree_node* sub_node = new DecisionTree_node();
		sub_node->property = -1;//定义-1为叶子节点
		sub_node->property_value = bound1;//根据全部数据集只有一种的标签值进行定义
		current_node->child.push_back(sub_node);
		return current_node;
	}

	//边界判断特征集是否为空
	int bound2 = meet_with_bound_EmptyProperty(current_data, current_property);
	if (bound2 != 0) {
		//生成叶子节点
		DecisionTree_node* sub_node = new DecisionTree_node();
		sub_node->property = -1;//定义-1为叶子节点
		if (bound2 > 0)sub_node->property_value = 1;//根据多数投票的结果标签值进行定义
		else sub_node->property_value = -1;
		sub_node->error_num = current_data.size() - abs(bound2 - 1);//记录多数投票结果的错误数
		current_node->child.push_back(sub_node);
		return current_node;
	}


	//预剪枝判断是否达到了最大深度
	if (BeforePruning == true && depth == 3) {
		//生成叶子节点
		int bound4 = MostLabel(current_data);
		DecisionTree_node* sub_node = new DecisionTree_node();
		sub_node->property = -1;//定义-1为叶子节点
		if (bound4 > 0)sub_node->property_value = 1;//根据多数投票的结果标签值进行定义
		else sub_node->property_value = -1;
		sub_node->error_num = current_data.size() - abs(bound4 - 1);//记录多数投票结果的错误数
		current_node->child.push_back(sub_node);
		return current_node;
	}

	//选择最佳决策特征项
	int best_property = choose_property(current_data, current_property, model);

	//根据最佳决策特征项生成子特征集
	vector<int> sub_property;
	for (vector <int>::iterator i = current_property.begin(); i < current_property.end(); i++) {
		if (*i != best_property) sub_property.push_back(*i);
	}

	//得到最佳决策特征项的特征值种类
	vector<int> current_property_value = Train_property_value[best_property];

	//根据最佳决策特征项的特征值种类生成子节点
	for (vector <int>::iterator i = current_property_value.begin(); i < current_property_value.end(); i++) {
		vector< vector<int> > sub_data;
		DecisionTree_node* sub_node = new DecisionTree_node();
		//根据最佳决策特征项的特征值种类生成子数据集
		for (int j = 0; j < current_data.size(); j++) {
			if (current_data[j][best_property] == *i) sub_data.push_back(current_data[j]);
		}
		//定义子节点的特征与特征值
		sub_node->property = best_property;
		sub_node->property_value = *i;

		//边界判断数据集是否为空
		int bound3 = meet_with_bound_EmptyData(sub_data);
		if (bound3 != 0) {
			//生成子节点的叶子节点
			DecisionTree_node* sub_sub_node = new DecisionTree_node();
			sub_sub_node->property = -1;//定义-1为叶子节点
			if (bound3 > 0)sub_sub_node->property_value = 1;//根据多数投票的结果标签值进行定义
			else sub_sub_node->property_value = -1;
			sub_node->child.push_back(sub_sub_node);
		}
		//递归生成子节点的决策树
		else {
			sub_node = BuildDecisionTree(sub_node, sub_data, sub_property, model, depth + 1, BeforePruning);
		}
		current_node->child.push_back(sub_node);
	}
	return current_node;
}

/*
删除决策树
输入：当前节点
*/
void FreeDecisionTree(DecisionTree_node* current_node) {
	//
	if (current_node == NULL) return;
	//递归删除子节点的决策树
	for (vector<DecisionTree_node*>::iterator i = current_node->child.begin(); i != current_node->child.end(); i++) {
		FreeDecisionTree(*i);
	}
	//将当前节点的数据置为NULL再delete
	current_node = NULL;
	delete current_node;
	return;
}

/*
决策树剪枝
输入：当前节点、决策树叶子节点总数、决策树错误数、alpha系数
输出：剪枝后的当前节点
*/
DecisionTree_node* PruningDecisionTree(DecisionTree_node* current_node, int& root_leaves_num, int& root_error_num, double alpha) {
	//如果当前节点为叶子节点则返回
	if (current_node->property == -1) return current_node;

	//递归剪枝子节点的决策树
	for (int i = 0; i < current_node->child.size(); i++) {
		current_node->child[i] = PruningDecisionTree(current_node->child[i], root_leaves_num, root_error_num, alpha);
	}

	//计算剪枝后的融入模型复杂度的错误率
	int new_error_num = 0;//记录错误数
	int new_label = 0;//记录叶子节点数
	if (current_node->label_true_num > current_node->label_false_num) {
		new_error_num = current_node->label_false_num;
		new_label = 1;
	}
	else {
		new_error_num = current_node->label_true_num;
		new_label = -1;
	}
	int current_error_num = Error_num_cnt(current_node);
	int current_leaves_num = Leaves_num_cnt(current_node);
	double new_error = (double(root_error_num - current_error_num + new_error_num) 
		+ double(root_leaves_num - current_leaves_num + 1) * double(alpha)) / double(Train_row);

	//计算剪枝前的融入模型复杂度的错误率
	double current_error = (double(root_error_num) + double(root_leaves_num) * double(alpha)) / double(Train_row);

	//判断错误率是否下降
	if (new_error < current_error) {
		//生成叶子节点
		DecisionTree_node* sub_node = new DecisionTree_node();
		sub_node->property = -1;//定义-1为叶子节点
		sub_node->property_value = new_label;//根据多数投票的结果标签值进行定义
		sub_node->error_num = new_error_num;//记录多数投票结果的错误数
		//删除该节点原先的所有子节点决策树
		for (vector<DecisionTree_node*>::iterator i = current_node->child.begin(); i != current_node->child.end(); i++) {
			FreeDecisionTree(*i);
		}
		current_node->child.clear();
		current_node->child.push_back(sub_node);
		//更新决策树叶子节点总数、决策树错误数
		root_error_num = root_error_num - current_error_num + new_error_num;
		root_leaves_num = root_leaves_num - current_leaves_num + 1;
	}
	return current_node;
}

/*
打印决策树
输入：当前节点、深度
输出：决策树
*/
void PrintDecisionTree(DecisionTree_node* current_node, int depth) {
	for (int i = 0; i < depth; i++) std::cout << '\t';//根据深度输出缩进
	//深度为0则为根节点
	if (depth == 0) {
		std::cout << "决策树" << endl;
	}
	else {
		//如果是叶子结果则输出标签
		if (current_node->property == -1) {
			std::cout << "标签" << ": " << current_node->property_value << endl;
		}
		//如果是普通子节点则输出特征及特征值
		else {
			std::cout << "特征" << current_node->property << ":" << current_node->property_value << endl;
		}

	}
	//递归打印子节点的决策树
	for (int i = 0; i < current_node->child.size(); i++) {
		PrintDecisionTree(current_node->child[i], depth + 1);
	}
}

/*
测试集单决策树决策
输入：测试集数据、决策树节点
输出：标签值
*/
int Test_label_Cal(vector<int> test_data, DecisionTree_node* current_node) {
	//如果是叶子节点则返回标签值
	if (current_node->child[0]->property == -1) {
		return current_node->child[0]->property_value;
	}
	//如果是普通子节点则递归决策树决策
	else {
		bool exist = true;//判断当前特征值是否存在在决策树中
		for (int i = 0; i < current_node->child.size(); i++) {
			//若找到匹配特征值则继续递归决策树决策
			if (test_data[current_node->child[i]->property] == current_node->child[i]->property_value) {
				return Test_label_Cal(test_data, current_node->child[i]);
			}
			else if (i == current_node->child.size() - 1) {
				exist = false;//若遍历完所有特征值仍不存在则记录
			}
		}
		//如果决策树中没有当前特征值则根据当前节点数据集多数投票结果返回标签
		if (exist == false) {
			int label_true = 0;//记录数据集标签为1的数量
			int label_false = 0;//记录数据集标签为-1的数量
			for (int i = 0; i < current_node->child.size(); i++) {
				if(Test_label_Cal(test_data, current_node->child[i]) == 1) label_true++;
				else label_false++;
			}
			if (label_true > label_false) return 1;
			else return -1;
		}
	}
}

/*
特征值映射函数
输入：特征、特征值
输出：映射后的特征值
*/
int MapClass(int property, int current_value) {
	int branch = 6;//将分支个数限定为branch
	if (property == 0 || property == 3) {
		return int(current_value % branch);
	}
	else return current_value;
}

/*
训练集数据输入处理
输入：交叉验证法分割分数、训练数据文件
*/
void Train_file_input(int file_split, bool ReduceClass) {
	//初始化各项数据

	Train_data.clear();//记录训练数据集
	Train_property.clear();//记录训练特征集
	Train_property_value.clear();//记录训练集每个特征的特征值的所有可能取值
	Train_col = 0;//记录总列数
	Train_row = 0;//记录总行数

	//读取训练集数据文件
	ifstream train_file("train1.csv");
	int tem_row = 0;
	if (train_file) {
		string train_str;
		while (getline(train_file, train_str)) {
			//根据交叉验证的分割份数读取文件
			if (!((file_split * 100) <= tem_row && tem_row < ((file_split + 1) * 100))) {
			//if (true) {
				//记录每行数据的特征值与标签值
				char* train_c_str = (char*)train_str.c_str();
				const char* d = ",";//以空格作截取词 
				char* tem_c = strtok(train_c_str, d);//截取有效单词 
				vector<int> tem_row_data;
				int tem_col = 0;
				while (tem_c) {
					//存储数据特征值,判断是否进行减少分叉操作
					if(ReduceClass == true) tem_row_data.push_back(MapClass(tem_col,atoi(tem_c)));
					else tem_row_data.push_back(atoi(tem_c));
					tem_c = strtok(NULL, d);
					tem_col++;
				}
				Train_data.push_back(tem_row_data);
				Train_row++;
			}
			tem_row++;
		}
		Train_col = Train_data[0].size();
		train_file.close();
	}
	else {
		std::cout << "open fail" << endl;
	}

	//记录训练特征集
	//记录训练集每个特征的特征值的所有可能取值
	for (int i = 0; i < Train_col - 1; i++) {
		int exist[100] = { 0, };
		vector<int> tem_property_value;
		for (int j = 0; j < Train_row; j++) {
			if (exist[Train_data[j][i]] == 0) {
				tem_property_value.push_back(Train_data[j][i]);
				exist[Train_data[j][i]] = 1;
			}
		}
		Train_property.push_back(i);
		Train_property_value[i] = tem_property_value;
	}
}

/*
测试集数据输入处理
输入：决策树节点、交叉验证法分割分数、训练数据文件
输出：准确率
*/
double Test_file_input(DecisionTree_node* current_node, int file_split, bool ReduceClass) {
	//ifstream test_file("train1.csv");
	ifstream test_file("test.csv");
	ofstream result_file("15352237_liushunyu.txt");
	if (test_file) {
		int corret_num = 0;
		int test_row = 0;
		string test_str;

		//读取测试集数据文件
		int tem_row = 0;
		while (getline(test_file, test_str)) {
			//根据交叉验证的分割份数读取文件
			//if ((file_split * 100) <= tem_row && tem_row < ((file_split + 1) * 100)) {
			if (true) {
				/*记录每行数据的特征值与标签值*/
				char* test_c_str = (char*)test_str.c_str();
				const char* d = ",";//以空格作截取词 
				char* tem_c = strtok(test_c_str, d);//截取有效单词 
				vector<int> tem_row_data;
				int tem_col = 0;
				while (tem_c) {
					//存储数据特征值,判断是否进行减少分叉操作
					if(ReduceClass == true)tem_row_data.push_back(MapClass(tem_col, atoi(tem_c)));
					else tem_row_data.push_back(atoi(tem_c));
					tem_c = strtok(NULL, d);
					tem_col++;
				}
				int test_label = Test_label_Cal(tem_row_data, current_node);//测试集单决策树决策
				result_file << test_label << endl;
				//if (test_label == tem_row_data[Train_col - 1]) corret_num++;//记录正确个数
				test_row++;
			}
			tem_row++;
		}
		//int corret_rate = double(corret_num) / double(test_row) * 100;//计算准确率
		//std::cout << "corret rate: " << corret_rate << "%" << endl;
		test_file.close();
		result_file.close();
		//return corret_rate;
		return 0;
	}
	else {
		std::cout << "open fail" << endl;
		return 0;
	}
}

/*
训练集数据随机抽取
输入：训练集数据
输出：随机抽取的训练集数据
*/
vector<vector<int>> Random_data(vector<vector<int>> current_data) {
	int exist[1000] = { 0, };
	vector<vector<int>> sub_data;
	//随机抽取训练集数据
	for (int i = 0; i < 200;) {
		int j = rand() % current_data.size();
		if (exist[j] == 0) {
			sub_data.push_back(current_data[j]);
			i++;
			exist[j] = 1;
		}
	}
	return sub_data;
}

/*
特征集数据随机抽取
输入：特征集
输出：随机抽取的特征集
*/
vector<int> Random_property(vector<int> current_property) {
	int exist[10] = { 0, };
	vector<int> sub_property;
	//随机抽取特征集
	for (int i = 0; i < 5;) {
		int j = rand() % current_property.size();
		if (exist[j] == 0) {
			sub_property.push_back(current_property[j]);
			i++;
			exist[j] = 1;
		}
	}
	return sub_property;
}

/*
训练集数据输入处理随机森林决策
输入：决策树根节点、交叉验证法分割分数、训练数据文件
输出：准确率
*/
double Test_file_input_RandomForest(vector<DecisionTree_node*>root_forest, int file_split, bool ReduceClass) {
	ifstream test_file("train1.csv");
	if (test_file) {
		int corret_num = 0;
		int test_row = 0;
		string test_str;

		int tem_row = 0;
		while (getline(test_file, test_str)) {
			if ((file_split * 100) <= tem_row && tem_row < ((file_split + 1) * 100)) {
				//记录每行数据的特征值与标签值
				char* test_c_str = (char*)test_str.c_str();
				const char* d = ",";//以空格作截取词 
				char* tem_c = strtok(test_c_str, d);//截取有效单词 
				vector<int> tem_row_data;
				int tem_col = 0;
				while (tem_c) {
					if (ReduceClass == true)tem_row_data.push_back(MapClass(tem_col, atoi(tem_c)));
					else tem_row_data.push_back(atoi(tem_c));//存储数据特征值
					tem_c = strtok(NULL, d);
					tem_col++;
				}
				int label_true_num = 0;//记录标签值为1的数量
				int label_false_num = 0;//记录标签值为-1的数量
				int test_label;
				//遍历决策森林
				for (int i = 0; i < root_forest.size(); i++) {
					//根据每颗决策树的结果标签记录数量
					if(Test_label_Cal(tem_row_data, root_forest[i]) == 1) label_true_num++;
					else label_false_num++;
				}
				//多数投票法选出最终标签结果
				if (label_true_num > label_false_num)test_label = 1;
				else test_label = -1;
				if (test_label == tem_row_data[Train_col - 1]) corret_num++;
				test_row++;
			}
			tem_row++;
		}
		int corret_rate = double(corret_num) / double(test_row) * 100;
		std::cout << "corret rate: " << corret_rate << "%" << endl;
		test_file.close();
		return corret_rate;
	}
	else {
		std::cout << "open fail" << endl;
		return 0;
	}
}

int main() {
	//记录程序运行时间 
	clock_t start_time = clock();

	int method = 2;//决策模型选择
	bool BeforePruning = false;//前剪枝选择
	bool AfterPruning = false;//后剪枝选择
	bool ReduceClass = false;//叉数减少
	int model = 2;//增益模型选择

	/*普通决策树*/
	if (method == 1) {
		for (double alpha = 0.3; alpha < 0.4; alpha = alpha + 0.1) {
			double avg_corret_rate = 0;
			for (int i = 0; i < 8; i++) {//交叉验证法
				DecisionTree_node *root = new DecisionTree_node();
				Train_file_input(i, ReduceClass);//训练集数据处理
				root = BuildDecisionTree(root, Train_data, Train_property, model,0, BeforePruning);//建立决策树
				root->property = -2;
				int root_leaves_num = Leaves_num_cnt(root);
				int root_error_num = Error_num_cnt(root);
				if(AfterPruning == true) PruningDecisionTree(root, root_leaves_num, root_error_num, alpha);//剪枝决策树
				//PrintDecisionTree(root, 0);//打印决策树
				avg_corret_rate += Test_file_input(root, i, ReduceClass);//测试决策树
				FreeDecisionTree(root);//删除决策树
			}
			avg_corret_rate = avg_corret_rate / 8;
			std::cout << "alpha: "<< alpha << " avg_corret_rate: " << avg_corret_rate << "%" << endl;
		}
	}
	/*随机森林*/
	else if (method == 2) {
		double avg_corret_rate = 0;
		srand(time(0));
		for (int i = 0; i < 8; i++) {
			Train_file_input(i, ReduceClass);//训练集数据处理
			vector<DecisionTree_node*> root_forest;//存储决策森林
			for (int j = 0; j < 10; j++) {
				DecisionTree_node *root = new DecisionTree_node();
				vector<vector<int>> RandomTrain_data;
				vector<int>RandomTrain_property;
				RandomTrain_data = Random_data(Train_data);//生成随机训练数据集
				RandomTrain_property= Random_property(Train_property);//生成随机特征集
				root = BuildDecisionTree(root, RandomTrain_data, RandomTrain_property, model, 0, BeforePruning);//建立决策树
				root->property = -2;
				//PrintDecisionTree(root, 0);
				root_forest.push_back(root);//将决策树放入决策森林
			}
			avg_corret_rate += Test_file_input_RandomForest(root_forest, i, ReduceClass);//测试决策森林
			for (int i = 0; i < root_forest.size(); i++) {
				FreeDecisionTree(root_forest[i]);//删除决策树
			}
		}
		avg_corret_rate = avg_corret_rate / 8;
		std::cout << "avg_corret_rate: " << avg_corret_rate << "%" << endl;
	}
	/*普通决策树跑测试集*/
	else if (method == 3) {
		double alpha = 0.3;
		DecisionTree_node *root = new DecisionTree_node();
		Train_file_input(-1, ReduceClass);//训练集数据处理
		root = BuildDecisionTree(root, Train_data, Train_property, model, 0, BeforePruning);//建立决策树
		root->property = -2;
		int root_leaves_num = Leaves_num_cnt(root);
		int root_error_num = Error_num_cnt(root);
		if (AfterPruning == true) PruningDecisionTree(root, root_leaves_num, root_error_num, alpha);//剪枝决策树
		//PrintDecisionTree(root, 0);//打印决策树
		Test_file_input(root, -1, ReduceClass);//测试决策树
		FreeDecisionTree(root);//删除决策树	
	}

	//记录程序运行时间 
	clock_t finish_time = clock();
	double totaltime = double(finish_time - start_time) / CLOCKS_PER_SEC;
	std::cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;

	return 0;
}