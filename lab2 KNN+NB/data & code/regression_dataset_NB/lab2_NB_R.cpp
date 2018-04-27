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

//���� Pro_emotion ��¼ÿ���ı���������и��� 
struct Pro_emotion{
	int num;//��ǰ���к� 
	double emotion[7];//������и��� 
	Pro_emotion(){
		num = 0;
		for(int i = 1; i < 7; i++)
			emotion[i] = 0;
	}
};
Pro_emotion words_pro_emotion[5000];

//�ȽϺ��������ڽ� words_pro_emotion ��ԭ�ı��к�˳���������� 
bool cmp_num(Pro_emotion & a, Pro_emotion & b){
	if(a.num < b.num) return true;
	else if(a.num == b.num){
		if(a.num < b.num) return true;
		else return false;
	}else return false;
}

//��Ԫ˳������ 
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

//��Ԫ˳������ıȽϺ��� 
bool cmp(Triple & a, Triple & b){
	if(a.row < b.row) return true;
	else if(a.row == b.row){
		if(a.col < b.col) return true;
		else return false;
	}else return false;
}

//��Ԫ˳��� 
struct TMatrix{
	int rn;
	int cn;
	int tn;
	vector<Triple> data;//��¼��Ԫ˳������ 
	vector<int> row_word_cnt;//��¼ȫ�����ݼ�ÿ�еĵ������� 
	vector<int> col_word_cnt;//��¼ÿ�����������ֵ��������� 
	TMatrix(){
		rn = 0;
		cn = 0;
		tn = 0;
	}
	//����Ԫ˳���������� 
	void update(){
		sort(data.begin(), data.end(), cmp);
	}
	//��� One_hot ���� 
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
	//��� TF ���� 
	void show_TF_matrix(){
		int cnt = 0;	
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//����Ĵ�����ֵĴ�����һ�����Ƶ��
					double TF_tem = double(data[cnt].value) / double(row_word_cnt[i]);
					TF_matrix[i][j] = TF_tem;
					cnt++;
				}else{
					TF_matrix[i][j] = 0;
				}	
			}
		}
	}
	//��� TFIDE ���� 
	void show_TFIDE_matrix(){
		int cnt = 0;	
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//���������ļ�Ƶ���ټ��� TFIDE ���� 
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
	clock_t start_time = clock();//��¼��������ʱ�� 
	
	map<string, int> word_num;//��¼ȫ�����ݼ�ÿ�����ʵ��к� 
	int cnt_col = 0;//��¼������ 
	int cnt_row = 0;//��¼������ 
	
	//ѵ��������
	ifstream mydata_file("train_set.csv");
	TMatrix Data_matrix;
    if(mydata_file){
    	string mydata_str;
    	getline(mydata_file, mydata_str);//ȥ����һ����Ч�ַ� 
	    while(getline(mydata_file, mydata_str)){
	    	map<string, int> word_pos;//��¼��ǰ��ÿ�����ʵ��к� 
	    	Data_matrix.row_word_cnt.push_back(0);//��ǰ�еĵ�������0 

	    	int pos = mydata_str.find(',');//����Ѱ�ҵ�һ��Tab���ֵ�λ�� 
	    	
	    	//��¼ÿ���ı�����и��� 
	    	string emotion_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//��ȡ�����ַ��� 
	    	char* mydata_c_emotion = (char*)emotion_str.c_str();
			const char* d_emotion = ","; //��,����ȡ�� 
	        char* word_emotion;  
	        word_emotion = strtok(mydata_c_emotion, d_emotion); //��ȡ��Ч���� 
	        int i_emotion = 1;
	    	while(word_emotion){  
	            string word_tem(word_emotion);
	            words_pro_emotion[cnt_row].emotion[i_emotion] = atof(word_tem.c_str());	
	            i_emotion++;
	            word_emotion=strtok(NULL, d_emotion);
	        }
	        
	        //��¼��ǰ���к� 
	    	words_pro_emotion[cnt_row].num = cnt_row;
	    	
			mydata_str = mydata_str.substr(0, pos);//��ȡ��Ч�ַ���
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = " "; //�Կո�����ȡ�� 
	        char* word;  
	        word = strtok(mydata_c, d); //��ȡ��Ч���� 
	        while(word){  
	            string word_tem(word);
	            Data_matrix.row_word_cnt[Data_matrix.row_word_cnt.size()-1]++;//��ǰ�еĵ�����++ 
	            
	            //�жϵ�ǰ�����Ƿ���ȫ�����ݼ���һ�γ��� 
	            map<string, int>::iterator it;
	            it = word_num.find(word_tem);
				if(it == word_num.end()){
				    word_num[word_tem] = cnt_col;//��¼�µ��ʵ��к� 
				    Data_matrix.col_word_cnt.push_back(0);//��ǰ�еĵ��ʳ��ֵ���������0 
				    Data_matrix.cn++;
	            	cnt_col++;
				}
				
				//�жϵ�ǰ�����Ƿ��ڵ�ǰ�����ݼ���һ�γ��� 
	            it = word_pos.find(word_tem);
				if(it == word_pos.end()){
				    word_pos[word_tem] = Data_matrix.data.size();//��¼�µ��ʵ��к�
				    Data_matrix.data.push_back(Triple(cnt_row,word_num[word_tem], 1));//���µ���λ�÷�����Ԫ˳��� 
				    Data_matrix.col_word_cnt[word_num[word_tem]]++;//��ǰ�еĵ��ʳ��ֵ�������++ 
				    Data_matrix.tn++;
				}else{
					Data_matrix.data[word_pos[word_tem]].value++;//�µ���������Ԫ˳���valueֵ++ 
				}
	            word=strtok(NULL, d);
	        }
			Data_matrix.rn++;
	        cnt_row++;
	    }
	    mydata_file.close();
	    Data_matrix.update();//����Ԫ˳����������
		
		//�����Ԫ˳������������� 
	    Data_matrix.show_One_hot_matrix();
	    Data_matrix.show_TF_matrix();
	    Data_matrix.show_TFIDE_matrix(); 
	}else{
		cout << "open fail" << endl;
	}
	
    //���Լ�����
	int method = 4;
	int k = 11;
	int p = 2;
	int train_cnt_col = cnt_col;
//	mydata_file.open("validation_set.csv");
	mydata_file.open("test_set.csv");
	ofstream outf;
	outf.open("test_emotion.csv");
	
    if(mydata_file && outf){
    	string mydata_str;
    	getline(mydata_file, mydata_str);//ȥ����һ����Ч�ַ� 
    	
	    while(getline(mydata_file, mydata_str)){
	    	
	    	int test_One_hot[5000] = {0,};//���Լ�����One_hot����
			double test_TF[5000] = {0,};//���Լ�����TF����
	    	int row_word_cnt= 0;//��ǰ�еĵ�������0 
	    	
	    	map<string, int> word_pos;//��¼��ǰ��ÿ�����ʵ��к�
	    	int pos = mydata_str.find(',');//����Ѱ�ҵ�һ��Tab���ֵ�λ�� 
	    		    	
			mydata_str = mydata_str.substr(0, pos);//��ȡ��Ч�ַ���
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = " "; //�Կո�����ȡ�� 
	        char* word;  
	        word = strtok(mydata_c, d); //��ȡ��Ч���� 
	        while(word){  
	            string word_tem(word);
	            row_word_cnt++;//��ǰ�еĵ�����++ 
	            
	            //�жϵ�ǰ�����Ƿ���ȫ�����ݼ���һ�γ��� 
	            map<string, int>::iterator it;
	            it = word_num.find(word_tem);
				if(it == word_num.end()){
				    word_num[word_tem] = cnt_col;//��¼�µ��ʵ��к� 
				    Data_matrix.col_word_cnt.push_back(0);//��ǰ�еĵ��ʳ��ֵ���������0
				    Data_matrix.cn++;
	            	cnt_col++;
				}
				
				//�жϵ�ǰ�����Ƿ��ڵ�ǰ�����ݼ���һ�γ��� 
	            it = word_pos.find(word_tem);
				if(it == word_pos.end()){
				    word_pos[word_tem] = Data_matrix.data.size();//��¼�µ��ʵ��к�
				    //���µ���λ�÷��������
				    test_One_hot[word_num[word_tem]] = 1;
				    test_TF[word_num[word_tem]] = 1;
				}else{
					test_TF[word_num[word_tem]]++;//�µ�������TF����ֵ++ 
				}
	            word=strtok(NULL, d);
	        }
	    
			//����TFIDF����     
	        for(int i = 0; i < cnt_col; i++){
	        	if(Data_matrix.col_word_cnt[i] == 0) test_TF[i] = 0;
	        	else test_TF[i] = test_TF[i] / row_word_cnt * (log(double(cnt_row) / (1.0 + double(Data_matrix.col_word_cnt[i]))));
			}
			
			//����NB�������� 
			double NB_emotion[7] = {0,};
			double sum = 0;
			//����������� 
			for(int i = 1; i <= 6; i++){
				//��������ѵ�����ı���ĳ��и��� 
				for(int j =0; j < cnt_row; j++){
					double tem_emotion = 1;
					bool tem=false;//�����жϵ�ǰ������ȫ�������Ƿ�û���ڵ�ǰѵ�����ı����г��� 
					for(int m = 0; m < cnt_col; m++){
			        	if((test_One_hot[m] == 1) && (TF_matrix[j][m]!=0)){
							tem = true;
							continue; 
						}
					}
					//������ֹ���ʼ����NB��и��� 
					if(tem == true){
						for(int m = 0; m < train_cnt_col; m++){
				        	if(test_One_hot[m] == 1){
				       			tem_emotion *= ((TF_matrix[j][m] + 0.01) / (1.0 + 0.01 * train_cnt_col));
							}
						}
						NB_emotion[i] += tem_emotion * words_pro_emotion[j].emotion[i]; 
					}
				}
				sum += NB_emotion[i];
			}
			
			//������������е��ʴ�δ������ƽ����ֵ��и��� 
			if(sum == 0){
				double tem = 0.166667;
				for(int i = 1; i <= 6; i++){
					outf << tem << ",";
				}	
			}else{
				//����и��ʽ��й�һ������� 
				for(int i = 1; i <= 6; i++){
					outf << NB_emotion[i] / sum << ",";
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
	clock_t finish_time = clock();//��¼��������ʱ�� 
	double totaltime = double(finish_time - start_time) / CLOCKS_PER_SEC;
	cout << "\n�˳��������ʱ��Ϊ" << totaltime << "�룡" << endl;
	return 0;  
} 
