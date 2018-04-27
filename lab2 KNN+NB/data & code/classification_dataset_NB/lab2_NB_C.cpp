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

//��Ŭ��ģ�� 
struct Ber_emotion{
	int num;//��¼ĳ��еľ����� 
	int words[5000];//��¼ĳ����г��ֵ�ĳ���ʵľ����� 
	Ber_emotion(){
		num = 0;
		for(int i = 0; i < 5000; i++)
			words[i] = 0;
	}
};
Ber_emotion bre_words_emotion[7];

//����ʽģ�� 
struct Mul_emotion{
	int num;//��¼ĳ��г��ֵ��ܵ����� 
	int words[5000];//��¼ĳ����г��ֵ�ĳ������ 
	Mul_emotion(){
		num = 0;
		for(int i = 0; i < 5000; i++)
			words[i] = 0;
	}
};
Mul_emotion mul_words_emotion[7];



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
	    	
	    	string emotion_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//��ȡ�����ַ��� 
	    	
			bre_words_emotion[emotion_num(emotion_str)].num++;//ĳ��о�������һ 
			
			mydata_str = mydata_str.substr(0, pos);//��ȡ��Ч�ַ���
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = " "; //�Կո�����ȡ�� 
	        char* word;  
	        word = strtok(mydata_c, d); //��ȡ��Ч���� 
	        while(word){  
	            string word_tem(word);
	            Data_matrix.row_word_cnt[Data_matrix.row_word_cnt.size()-1]++;//��ǰ�еĵ�����++ 
	            
	            mul_words_emotion[emotion_num(emotion_str)].num++;//ĳ��г��ֵ��ܵ�������һ 
	            
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
				    bre_words_emotion[emotion_num(emotion_str)].words[word_num[word_tem]]++;//ĳ����г��ֵ�ĳ���ʵľ�������һ 
				    Data_matrix.tn++;
				}else{
					Data_matrix.data[word_pos[word_tem]].value++;//�µ���������Ԫ˳���valueֵ++ 
				}
				
				mul_words_emotion[emotion_num(emotion_str)].words[word_num[word_tem]]++;//ĳ����г��ֵ�ĳ��������һ 
				
	            word=strtok(NULL, d);
	        }
			Data_matrix.rn++;
	        cnt_row++;
	    }
	    mydata_file.close();
	    Data_matrix.update();//����Ԫ˳����������
	}else{
		cout << "open fail" << endl;
	}
	
	int train_cnt_col = cnt_col;
	
	//���Լ�����
	int method = 3;
	for(double a = 0.9; a <= 0.9; a=a+0.01)
	{		
//		mydata_file.open("validation_set.csv");
		mydata_file.open("test_set.csv");
		ofstream outf;
		outf.open("test_emotion.csv");
		
	    if(mydata_file && outf){
	    	string mydata_str;
	    	getline(mydata_file, mydata_str);//ȥ����һ����Ч�ַ� 
	    	
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

					
		    	int test_One_hot[5000] = {0,};//���Լ�����One_hot����
				double test_TF[5000] = {0,};//���Լ�����TF����
		    	int row_word_cnt= 0;//��ǰ�еĵ�������0 
		    	
		    	map<string, int> word_pos;//��¼��ǰ��ÿ�����ʵ��к�
		    	int pos = mydata_str.find(',');//����Ѱ�ҵ�һ��Tab���ֵ�λ�� 
		    	
		    	string emotion_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//��ȡ�����ַ��� 
		    	
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
					    
					    //���ģ��
					    if(method == 3){
							for(int i = 1;i <=6; i++)
								pro_emotion[i] *= double(mul_words_emotion[i].words[word_num[word_tem]] + a*1.0) / double(mul_words_emotion[i].num + a*train_cnt_col);
						}
					}else{
						test_TF[word_num[word_tem]]++;//�µ�������TF����ֵ++ 
					}
					
					//��Ŭ��ģ��
					if(method == 1){
						for(int i = 1;i <= 6; i++)
							pro_emotion[i] +=  double(bre_words_emotion[i].words[word_num[word_tem]] + 1.0) / double(bre_words_emotion[i].num + 2.0);
					//����ʽģ��
					}else if(method == 2){
						for(int i = 1;i <=6; i++)
							pro_emotion[i] *= double(mul_words_emotion[i].words[word_num[word_tem]] + a*1.0) / double(mul_words_emotion[i].num + a*train_cnt_col);
					}
		            word=strtok(NULL, d);
		        }
		        
		        //��Ŭ��ģ��
		        if(method == 1){
			        for(int i = 1;i <=6; i++){
						pro_emotion[i] = double(bre_words_emotion[i].num) / double(cnt_row);
					} 
				}

				//ѡȡ�������������Ϊ max_emotion
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
			cout << "method = " << a << " ׼ȷ�ʣ� " << double(true_emotion_num / 312.0) << endl;
		    mydata_file.close();
		    outf.close();
		}else{
			cout << "open fail" << endl;
		}
	}
	clock_t finish_time=clock();//��¼��������ʱ�� 
	double totaltime=double(finish_time-start_time)/CLOCKS_PER_SEC;
	cout<<"\n�˳��������ʱ��Ϊ"<<totaltime<<"�룡"<<endl;
	return 0;  
} 
