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
#include <cstdlib>
#include <stdlib.h>
using namespace std;

int One_hot_matrix[5000][5000] = {0,};
double TF_matrix[5000][5000] = {0,};
double  TFIDE_matrix[5000][5000] = {0,};


#define anger 1
#define disgust 2
#define fear 3
#define joy 4
#define sad 5
#define surprise 6

int emotion_num(string s){
	if(s == "anger") return 1;
	if(s == "disgust") return 2;
	if(s == "fear") return 3;
	if(s == "joy") return 4;
	if(s == "sad") return 5;
	if(s == "surprise") return 6;
}
string emotion_string(int i){
	if(i == 1) return "anger";
	if(i == 2) return "disgust";
	if(i == 3) return "fear";
	if(i == 4) return "joy";
	if(i == 5) return "sad";
	if(i == 6) return "surprise";
}

struct Dis_emotion{
	int num;
	double dis;
	double emotion[7];
	Dis_emotion(){
		num = 0;
		dis = 0.0;
		for(int i = 1; i < 7; i++)
			emotion[i] = 0;
	}
};
Dis_emotion words_dis_emotion[5000];


bool cmp_dis(Dis_emotion & a, Dis_emotion & b){
	if(a.dis < b.dis) return true;
	else if(a.dis == b.dis){
		if(a.dis < b.dis) return true;
		else return false;
	}else return false;
}

bool cmp_num(Dis_emotion & a, Dis_emotion & b){
	if(a.num < b.num) return true;
	else if(a.num == b.num){
		if(a.num < b.num) return true;
		else return false;
	}else return false;
}

//三元顺序表表项 
struct Triple{
	int row;
	int col;
	int value;
	Triple(){
		row = 0;
		col = 0;
		value = 0;
	}
	Triple(int row, int col, int value){
		this->row = row;
		this->col = col;
		this->value = value;
	}
};

//三元顺序表表项的比较函数 
bool cmp(Triple & a, Triple & b){
	if(a.row < b.row) return true;
	else if(a.row == b.row){
		if(a.col < b.col) return true;
		else return false;
	}else return false;
}

//三元顺序表 
struct TMatrix{
	int rn;
	int cn;
	int tn;
	vector<Triple> data;//记录三元顺序表表项 
	vector<int> row_word_cnt;//记录全部数据集每行的单词总数 
	vector<int> col_word_cnt;//记录每个单词所出现的文章总数 
	TMatrix(){
		rn = 0;
		cn = 0;
		tn = 0;
	}
	//对三元顺序表进行排序 
	void update(){
		sort(data.begin(), data.end(), cmp);
	}
	//输出 One_hot 矩阵 
	void show_One_hot_matrix(){
		int cnt = 0;
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					One_hot_matrix[i][j] = 1;
					cnt++;
				}else{
					One_hot_matrix[i][j] = 0;
				}	
			}
		}
	}
	//输出 TF 矩阵 
	void show_TF_matrix(){
		int cnt = 0;	
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//计算的词语出现的次数归一化后的频率
					double TF_tem = double(data[cnt].value) / double(row_word_cnt[i]);
					TF_matrix[i][j] = TF_tem;
					cnt++;
				}else{
					TF_matrix[i][j] = 0;
				}	
			}
		}
	}
	//输出 TFIDE 矩阵 
	void show_TFIDE_matrix(){
		int cnt = 0;	
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//计算逆向文件频率再计算 TFIDE 矩阵 
					double TF_tem = double(data[cnt].value) / double(row_word_cnt[i]);
					double TFIDE_tem = TF_tem * (log(double(rn) / (1.0 + double(col_word_cnt[j]))));				
					TFIDE_matrix[i][j] = TFIDE_tem;
					cnt++;
				}else{
					TFIDE_matrix[i][j] = 0;
				}	
			}
		}
	}
};

int main(){
	clock_t start_time = clock();//记录程序运行时间 
	
	map<string, int> word_num;//记录全部数据集每个单词的列号 
	int cnt_col = 0;//记录总列数 
	int cnt_row = 0;//记录总行数 
	
	//训练集数据
	ifstream mydata_file("train_set.csv");
	TMatrix Data_matrix;
    if(mydata_file){
    	string mydata_str;
    	getline(mydata_file, mydata_str);//去除第一行无效字符 
	    while(getline(mydata_file, mydata_str)){
	    	map<string, int> word_pos;//记录当前行每个单词的列号 
	    	Data_matrix.row_word_cnt.push_back(0);//当前行的单词数置0 

	    	int pos = mydata_str.find(',');//反向寻找第一个Tab出现的位置 
	    		
	    	string emotion_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//截取情绪字符串 
	    	char* mydata_c_emotion = (char*)emotion_str.c_str();
			const char* d_emotion = ","; //以,作截取词 
	        char* word_emotion;  
	        word_emotion = strtok(mydata_c_emotion, d_emotion); //截取有效单词 
	        
	        //截取情感单词并记录 
	        int i_emotion = 1;
	    	while(word_emotion){  
	            string word_tem(word_emotion);
	            words_dis_emotion[cnt_row].emotion[i_emotion] = atof(word_tem.c_str());	
	            i_emotion++;
	            word_emotion=strtok(NULL, d_emotion);
	        }
	    	words_dis_emotion[cnt_row].num = cnt_row;
	    	
			mydata_str = mydata_str.substr(0, pos);//截取有效字符串
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = " "; //以空格作截取词 
	        char* word;  
	        word = strtok(mydata_c, d); //截取有效单词 
	        while(word){  
	            string word_tem(word);
	            Data_matrix.row_word_cnt[Data_matrix.row_word_cnt.size()-1]++;//当前行的单词数++ 
	            
	            //判断当前单词是否在全部数据集第一次出现 
	            map<string, int>::iterator it;
	            it = word_num.find(word_tem);
				if(it == word_num.end()){
				    word_num[word_tem] = cnt_col;//记录新单词的列号 
				    Data_matrix.col_word_cnt.push_back(0);//当前列的单词出现的文章数置0 
				    Data_matrix.cn++;
	            	cnt_col++;
				}
				
				//判断当前单词是否在当前行数据集第一次出现 
	            it = word_pos.find(word_tem);
				if(it == word_pos.end()){
				    word_pos[word_tem] = Data_matrix.data.size();//记录新单词的列号
				    Data_matrix.data.push_back(Triple(cnt_row,word_num[word_tem], 1));//将新单词位置放入三元顺序表 
				    Data_matrix.col_word_cnt[word_num[word_tem]]++;//当前列的单词出现的文章数++ 
				    Data_matrix.tn++;
				}else{
					Data_matrix.data[word_pos[word_tem]].value++;//新单词所在三元顺序表value值++ 
				}
	            word=strtok(NULL, d);
	        }
			Data_matrix.rn++;
	        cnt_row++;
	    }
	    mydata_file.close();
	    Data_matrix.update();//对三元顺序表进行排序
		
		//输出三元顺序表与三个矩阵 
	    Data_matrix.show_One_hot_matrix();
	    Data_matrix.show_TF_matrix();
	    Data_matrix.show_TFIDE_matrix(); 
	}else{
		cout << "open fail" << endl;
	}
	

    //测试集数据
	int method = 4;
	int k = 11;
	int p = 2;
	
//	mydata_file.open("validation_set.csv");
	mydata_file.open("test_set.csv");
	ofstream outf;
	outf.open("test_emotion.csv");
	
    if(mydata_file && outf){
    	string mydata_str;
    	getline(mydata_file, mydata_str);//去除第一行无效字符 
    	
	    while(getline(mydata_file, mydata_str)){
	    	
	    	int test_One_hot[5000] = {0,};//测试集数据One_hot矩阵
			double test_TF[5000] = {0,};//测试集数据TF矩阵
	    	int row_word_cnt= 0;//当前行的单词数置0 
	    	
	    	map<string, int> word_pos;//记录当前行每个单词的列号
	    	int pos = mydata_str.find(',');//反向寻找第一个Tab出现的位置 
	    	
	    	
			mydata_str = mydata_str.substr(0, pos);//截取有效字符串
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = " "; //以空格作截取词 
	        char* word;  
	        word = strtok(mydata_c, d); //截取有效单词 
	        while(word){  
	            string word_tem(word);
	            row_word_cnt++;//当前行的单词数++ 
	            
	            //判断当前单词是否在全部数据集第一次出现 
	            map<string, int>::iterator it;
	            it = word_num.find(word_tem);
				if(it == word_num.end()){
				    word_num[word_tem] = cnt_col;//记录新单词的列号 
				    Data_matrix.col_word_cnt.push_back(0);//当前列的单词出现的文章数置0
				    Data_matrix.cn++;
	            	cnt_col++;
				}
				
				//判断当前单词是否在当前行数据集第一次出现 
	            it = word_pos.find(word_tem);
				if(it == word_pos.end()){
				    word_pos[word_tem] = Data_matrix.data.size();//记录新单词的列号
				    //将新单词位置放入矩阵中 
				    test_One_hot[word_num[word_tem]] = 1;
				    test_TF[word_num[word_tem]] = 1;
				}else{
					test_TF[word_num[word_tem]]++;//新单词所在TF矩阵值++ 
				}
	            word=strtok(NULL, d);
	        }
	        //定义TF矩阵 
	        for(int i = 0; i < cnt_col; i++){
	        	if(Data_matrix.col_word_cnt[i] == 0) test_TF[i] = 0;
	        	else test_TF[i] = test_TF[i] / row_word_cnt * (log(double(cnt_row) / (1.0 + double(Data_matrix.col_word_cnt[i]))));
			}
			
			double cos_tem_test = 0;
			double cos_tem_train = 0;
			
			//初始化训练集所有距离为零，并初始化为最原始的排序 
			for(int i = 0; i < 5000; i++)
				words_dis_emotion[i].dis = 0.0;
			sort(words_dis_emotion, words_dis_emotion + cnt_row, cmp_num);
			
			//根据不同距离公式与矩阵类型计算距离 
			for(int i = 0; i < cnt_row; i++){
				//使用onehot矩阵与任意p次距离 
				if(method == 1){
					for(int j = 0; j < cnt_col; j++){
						words_dis_emotion[i].dis = words_dis_emotion[i].dis + pow(abs(test_One_hot[j] - One_hot_matrix[i][j]), p);
					}
					words_dis_emotion[i].dis = pow(words_dis_emotion[i].dis, double(1.0/p));
				}
				//使用onehot矩阵与余弦距离 
				else if(method == 2){
					for(int j = 0; j < cnt_col; j++){
						words_dis_emotion[i].dis = words_dis_emotion[i].dis + (test_One_hot[j] * One_hot_matrix[i][j]);
						cos_tem_test = cos_tem_test + pow(test_One_hot[j], 2);
						cos_tem_train = cos_tem_train + pow(One_hot_matrix[i][j], 2);
					}
					cos_tem_test = sqrt(cos_tem_test);
					cos_tem_train = sqrt(cos_tem_train);
					words_dis_emotion[i].dis = words_dis_emotion[i].dis / (cos_tem_test * cos_tem_train);
				}
				//使用TFIDF矩阵与任意p次距离 
				else if(method == 3){
					for(int j = 0; j < cnt_col; j++){
						words_dis_emotion[i].dis = words_dis_emotion[i].dis + pow(abs(test_TF[j] - TFIDE_matrix[i][j]), p);
					}
					words_dis_emotion[i].dis = pow(words_dis_emotion[i].dis, double(1.0/p));
				}
				//使用TFIDF矩阵与余弦距离
				else if(method == 4){
					for(int j = 0; j < cnt_col; j++){
						words_dis_emotion[i].dis = words_dis_emotion[i].dis + (test_TF[j] * TFIDE_matrix[i][j]);
						cos_tem_test = cos_tem_test + pow(test_TF[j], 2);
						cos_tem_train = cos_tem_train + pow(TFIDE_matrix[i][j], 2);
					}
					cos_tem_test = sqrt(cos_tem_test);
					cos_tem_train = sqrt(cos_tem_train);
					if(words_dis_emotion[i].dis == 0) words_dis_emotion[i].dis = 0;
					else words_dis_emotion[i].dis = words_dis_emotion[i].dis / (cos_tem_test * cos_tem_train);
				}
			}
			
			//对所有距离进行从小到大排序 
			sort(words_dis_emotion, words_dis_emotion + cnt_row, cmp_dis);

			double knn_tem_pro[7] = {0,};
			double sum = 0;
			
			//若使用p次距离则距离越小越近 
			if(method == 1 || method == 3){
				//如果距离为零则句子完全相同，直接将情感附值 
				if(words_dis_emotion[0].dis == 0){
					for(int i = 1; i <= 6; i++){
						knn_tem_pro[i] = words_dis_emotion[0].emotion[i];
						sum += 	knn_tem_pro[i];
					}	
				}else{
					//计算各距离的倒数乘以情感概率 
					for(int i = 1; i <= 6; i++){
						//记录最大距离与最小距离以使用标准化 
						double max_dis = words_dis_emotion[0].dis;
						double min_dis = words_dis_emotion[0].dis; 
						for(int j = 0; j < k; j++){
							if(words_dis_emotion[j].dis > max_dis) max_dis = words_dis_emotion[j].dis;
							if(words_dis_emotion[j].dis < min_dis) min_dis = words_dis_emotion[j].dis;
						}
						for(int j = 0; j < k; j++){
							knn_tem_pro[i] += words_dis_emotion[j].emotion[i] / words_dis_emotion[j].dis;
							//对距离权重进行标准化 
//							knn_tem_pro[i] += words_dis_emotion[j].emotion[i] / ((words_dis_emotion[j].dis - min_dis + 1) / (max_dis - min_dis + 1));
						}
						sum += 	knn_tem_pro[i];
					}	
				}	
			//若使用余弦距离则余弦值越大越接近
			}else{
				//如果距离为一则句子完全相同，直接将情感附值 
				if(words_dis_emotion[cnt_row - 1].dis == 1){
					for(int i = 1; i <= 6; i++){
						knn_tem_pro[i] = words_dis_emotion[cnt_row - 1].emotion[i];
						sum += 	knn_tem_pro[i];
					}	
				}else{
					//计算各距离乘以情感概率 
					for(int i = 1; i <= 6; i++){
						//记录最大距离与最小距离、方差与平均值以使用标准化 
						double max_dis = words_dis_emotion[cnt_row - 1].dis;
						double min_dis = words_dis_emotion[cnt_row - 1].dis; 
						double mean_dis = 0;
						double standard_deviation_dis = 0;
						for(int j = cnt_row - 1; j > cnt_row - k - 1; j--){
							if(words_dis_emotion[j].dis > max_dis) max_dis = words_dis_emotion[j].dis;
							if(words_dis_emotion[j].dis < min_dis) min_dis = words_dis_emotion[j].dis;
							mean_dis += words_dis_emotion[j].dis;
						}
						//计算平均值 
						mean_dis = mean_dis / k;
						//计算方差 
						for(int j = cnt_row - 1; j > cnt_row - k - 1; j--){
							standard_deviation_dis += pow((words_dis_emotion[j].dis - mean_dis), 2);
						}
						standard_deviation_dis = sqrt(standard_deviation_dis / k);
						
						for(int j = cnt_row - 1; j > cnt_row - k - 1; j--){
							knn_tem_pro[i] += words_dis_emotion[j].emotion[i] * words_dis_emotion[j].dis;
							//对距离权重进行两种不同的标准化 
//							knn_tem_pro[i] += words_dis_emotion[j].emotion[i] * (words_dis_emotion[j].dis - min_dis) / (max_dis - min_dis);
//							knn_tem_pro[i] += words_dis_emotion[j].emotion[i] * (words_dis_emotion[j].dis - mean_dis + 1) / standard_deviation_dis;
						}
						sum += 	knn_tem_pro[i];
					}
				}
				
			}
			
			//如果句子中所有单词从未出现则平均附值情感概率 
			if(sum == 0){
				double tem = 0.166667;
				for(int i = 1; i <= 6; i++){
					outf << tem << ",";
				}	
			}else{
				//对情感概率进行归一化后输出 
				for(int i = 1; i <= 6; i++){
					outf << knn_tem_pro[i] / sum << ",";
				}
			}		
			outf << endl;
			
	    }
	    mydata_file.close();
		outf.close();
	}else{
		cout << "open fail" << endl;
	}

	outf.close();
	clock_t finish_time = clock();//记录程序运行时间 
	double totaltime = double(finish_time - start_time) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	return 0;  
} 
