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

//伯努利模型 
struct Ber_emotion{
	int num;//记录某情感的句子数 
	int words[5000];//记录某情感中出现的某单词的句子数 
	Ber_emotion(){
		num = 0;
		for(int i = 0; i < 5000; i++)
			words[i] = 0;
	}
};
Ber_emotion bre_words_emotion[7];

//多项式模型 
struct Mul_emotion{
	int num;//记录某情感出现的总单词数 
	int words[5000];//记录某情感中出现的某单词数 
	Mul_emotion(){
		num = 0;
		for(int i = 0; i < 5000; i++)
			words[i] = 0;
	}
};
Mul_emotion mul_words_emotion[7];



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
	    	
			bre_words_emotion[emotion_num(emotion_str)].num++;//某情感句子数加一 
			
			mydata_str = mydata_str.substr(0, pos);//截取有效字符串
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = " "; //以空格作截取词 
	        char* word;  
	        word = strtok(mydata_c, d); //截取有效单词 
	        while(word){  
	            string word_tem(word);
	            Data_matrix.row_word_cnt[Data_matrix.row_word_cnt.size()-1]++;//当前行的单词数++ 
	            
	            mul_words_emotion[emotion_num(emotion_str)].num++;//某情感出现的总单词数加一 
	            
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
				    bre_words_emotion[emotion_num(emotion_str)].words[word_num[word_tem]]++;//某情感中出现的某单词的句子数加一 
				    Data_matrix.tn++;
				}else{
					Data_matrix.data[word_pos[word_tem]].value++;//新单词所在三元顺序表value值++ 
				}
				
				mul_words_emotion[emotion_num(emotion_str)].words[word_num[word_tem]]++;//某情感中出现的某单词数加一 
				
	            word=strtok(NULL, d);
	        }
			Data_matrix.rn++;
	        cnt_row++;
	    }
	    mydata_file.close();
	    Data_matrix.update();//对三元顺序表进行排序
	}else{
		cout << "open fail" << endl;
	}
	
	int train_cnt_col = cnt_col;
	
	//测试集数据
	int method = 3;
	for(double a = 0.9; a <= 0.9; a=a+0.01)
	{		
//		mydata_file.open("validation_set.csv");
		mydata_file.open("test_set.csv");
		ofstream outf;
		outf.open("test_emotion.csv");
		
	    if(mydata_file && outf){
	    	string mydata_str;
	    	getline(mydata_file, mydata_str);//去除第一行无效字符 
	    	
	    	int true_emotion_num = 0; 
		    while(getline(mydata_file, mydata_str)){
		    	
				double pro_emotion[7];
		    	if(method == 1){
		    		for(int i = 1;i <=6; i++)
						pro_emotion[i] = 0;
		    	}else{
			    	for(int i = 1;i <=6; i++){
						pro_emotion[i] = double(bre_words_emotion[i].num) / double(cnt_row);
					} 		    		
				}

					
		    	int test_One_hot[5000] = {0,};//测试集数据One_hot矩阵
				double test_TF[5000] = {0,};//测试集数据TF矩阵
		    	int row_word_cnt= 0;//当前行的单词数置0 
		    	
		    	map<string, int> word_pos;//记录当前行每个单词的列号
		    	int pos = mydata_str.find(',');//反向寻找第一个Tab出现的位置 
		    	
		    	string emotion_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//截取情绪字符串 
		    	
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
					    
					    //混合模型
					    if(method == 3){
							for(int i = 1;i <=6; i++)
								pro_emotion[i] *= double(mul_words_emotion[i].words[word_num[word_tem]] + a*1.0) / double(mul_words_emotion[i].num + a*train_cnt_col);
						}
					}else{
						test_TF[word_num[word_tem]]++;//新单词所在TF矩阵值++ 
					}
					
					//伯努利模型
					if(method == 1){
						for(int i = 1;i <= 6; i++)
							pro_emotion[i] +=  double(bre_words_emotion[i].words[word_num[word_tem]] + 1.0) / double(bre_words_emotion[i].num + 2.0);
					//多项式模型
					}else if(method == 2){
						for(int i = 1;i <=6; i++)
							pro_emotion[i] *= double(mul_words_emotion[i].words[word_num[word_tem]] + a*1.0) / double(mul_words_emotion[i].num + a*train_cnt_col);
					}
		            word=strtok(NULL, d);
		        }
		        
		        //伯努利模型
		        if(method == 1){
			        for(int i = 1;i <=6; i++){
						pro_emotion[i] = double(bre_words_emotion[i].num) / double(cnt_row);
					} 
				}

				//选取概率最大的情感作为 max_emotion
				int max_emotion = 1;
				double max_pro = pro_emotion[1];
				for(int i = 1;i <= 6; i++){
					if(max_pro < pro_emotion[i]){
						max_emotion = i;
						max_pro = pro_emotion[i];
						
					}
				}
				
				outf << emotion_string(max_emotion) << endl;
				if(emotion_string(max_emotion) == emotion_str) true_emotion_num++;
				
		    }
			cout << "method = " << a << " 准确率： " << double(true_emotion_num / 312.0) << endl;
		    mydata_file.close();
		    outf.close();
		}else{
			cout << "open fail" << endl;
		}
	}
	clock_t finish_time=clock();//记录程序运行时间 
	double totaltime=double(finish_time-start_time)/CLOCKS_PER_SEC;
	cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<endl;
	return 0;  
} 
