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

struct Dis_emotion{
	int num;
	double dis;
	int emotion;
	Dis_emotion(){
		num = 0;
		dis = 0.0;
		emotion = 0;
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
			double tem_Normalized = 0; //��¼������ģ 
			int tem_cnt = cnt;
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//���������ļ�Ƶ���ټ��� TFIDE ���� 
					double TF_tem = double(data[cnt].value) / double(row_word_cnt[i]);
					double TFIDE_tem = TF_tem * (log(double(rn) / (1.0 + double(col_word_cnt[j]))));
					tem_Normalized += pow(TFIDE_tem, 2);
					TFIDE_matrix[i][j] = TFIDE_tem;
					cnt++;
				}else{
					TFIDE_matrix[i][j] = 0;
				}	
			}
			tem_Normalized = sqrt(tem_Normalized);
			//�� TFIDF �������������ģ��һ������ 
			for(int j = 0; j < cn; j++){
				if(i == data[tem_cnt].row && j == data[tem_cnt].col){
					TFIDE_matrix[i][j] = TFIDE_matrix[i][j] / tem_Normalized;
					tem_cnt++;
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
	    	
	    	
	    	string emotion_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//��ȡ�����ַ��� 
	    	words_dis_emotion[cnt_row].emotion = emotion_num(emotion_str);
	    	words_dis_emotion[cnt_row].num = cnt_row;
	    	
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
	int p = 2;
	for(int k = 15; k <16; k++)
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
					}else{
						test_TF[word_num[word_tem]]++;//�µ�������TF����ֵ++ 
					}
		            word=strtok(NULL, d);
		        }
		        //����TFIDF���� 
		        double tem_Normalized = 0; //��¼������ģ 
		        for(int i = 0; i < cnt_col; i++){
		        	if(Data_matrix.col_word_cnt[i] == 0) test_TF[i] = 0;
		        	else{
		        		test_TF[i] = test_TF[i] / row_word_cnt * (log(double(cnt_row) / (1.0 + double(Data_matrix.col_word_cnt[i]))));
		        		tem_Normalized += pow(test_TF[i], 2);
					} 
				}
				tem_Normalized = sqrt(tem_Normalized);
				//�� TFIDF �������������ģ�Ĺ�һ������ 
				for(int i = 0; i < cnt_col; i++){
		        	test_TF[i] = test_TF[i] / tem_Normalized;
				}
				
				double cos_tem_test = 0;
				double cos_tem_train = 0;
				
				//��ʼ��ѵ�������о���Ϊ�㣬����ʼ��Ϊ��ԭʼ������ 
				for(int i = 0; i < 5000; i++)
					words_dis_emotion[i].dis = 0.0;
				sort(words_dis_emotion, words_dis_emotion + cnt_row, cmp_num);
				
				//���ݲ�ͬ���빫ʽ��������ͼ������ 
				for(int i = 0; i < cnt_row; i++){
					//ʹ��onehot����������p�ξ��� 
					if(method == 1){
						for(int j = 0; j < cnt_col; j++){
							words_dis_emotion[i].dis = words_dis_emotion[i].dis + pow(abs(test_One_hot[j] - One_hot_matrix[i][j]), p);
						}
						words_dis_emotion[i].dis = pow(words_dis_emotion[i].dis, double(1.0/p));
					}
					//ʹ��onehot���������Ҿ��� 
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
					//ʹ��TFIDF����������p�ξ��� 
					else if(method == 3){
						for(int j = 0; j < cnt_col; j++){
							words_dis_emotion[i].dis = words_dis_emotion[i].dis + pow(abs(test_TF[j] - TFIDE_matrix[i][j]), p);
						}
						words_dis_emotion[i].dis = pow(words_dis_emotion[i].dis, double(1.0/p));
					}
					//ʹ��TFIDF���������Ҿ��� 
					else if(method == 4){
						for(int j = 0; j < cnt_col; j++){
							words_dis_emotion[i].dis = words_dis_emotion[i].dis + (test_TF[j] * TFIDE_matrix[i][j]);
							cos_tem_test = cos_tem_test + pow(test_TF[j], 2);
							cos_tem_train = cos_tem_train + pow(TFIDE_matrix[i][j], 2);
						}
						cos_tem_test = sqrt(cos_tem_test);
						cos_tem_train = sqrt(cos_tem_train);
						words_dis_emotion[i].dis = words_dis_emotion[i].dis / (cos_tem_test * cos_tem_train);
					}
				}
				
				//�����о�����д�С�������� 
				sort(words_dis_emotion, words_dis_emotion + cnt_row, cmp_dis);
				
				int knn_tem_num[7] = {0,};
				double knn_tem_sum_dis[7] ={0,};
				int max_emotion = 1;
				
				if(method == 1 || method == 3){
					//��ʹ��p�ξ��������ԽСԽ�� 
					for(int i = 0; i < k; i++){
						knn_tem_num[words_dis_emotion[i].emotion]++;//��¼ÿ����еĸ��� 
						knn_tem_sum_dis[words_dis_emotion[i].emotion] +=  words_dis_emotion[i].dis;//��¼ÿ����еľ����ܺ� 
						if(max_emotion != words_dis_emotion[i].emotion){
							//ȡ�������������Ϊ max_emotion
							if(knn_tem_num[max_emotion] < knn_tem_num[words_dis_emotion[i].emotion]) max_emotion = words_dis_emotion[i].emotion;
							//�����ֶ����������У����ж������ͣ�����Ϊmax_emotion 
							else if (knn_tem_num[max_emotion] == knn_tem_num[words_dis_emotion[i].emotion]){
								if(knn_tem_sum_dis[max_emotion] > knn_tem_sum_dis[words_dis_emotion[i].emotion])
									max_emotion = words_dis_emotion[i].emotion;
							} 
						}	
					}	
				}else{
					//��ʹ�����Ҿ���������ֵԽ��Խ�ӽ� 
					for(int i = cnt_row - 1; i > cnt_row - k - 1; i--){
						knn_tem_num[words_dis_emotion[i].emotion]++;//��¼ÿ����еĸ��� 
						knn_tem_sum_dis[words_dis_emotion[i].emotion] +=  words_dis_emotion[i].dis;//��¼ÿ����еľ����ܺ� 
						//ȡ�����ܺ�����������Ϊ max_emotion
						if(knn_tem_sum_dis[max_emotion] < knn_tem_sum_dis[words_dis_emotion[i].emotion])
							max_emotion = words_dis_emotion[i].emotion;
					}
//					//��ʹ�����Ҿ���������ֵԽ��Խ�ӽ� 
//					for(int i = cnt_row - 1; i > cnt_row - k - 1; i--){
//						knn_tem_num[words_dis_emotion[i].emotion]++;//��¼ÿ����еĸ��� 
//						knn_tem_sum_dis[words_dis_emotion[i].emotion] +=  words_dis_emotion[i].dis;//��¼ÿ����еľ����ܺ� 
//                      //ȡ�������������Ϊ max_emotion
//						if(max_emotion != words_dis_emotion[i].emotion){
//							if(knn_tem_num[max_emotion] < knn_tem_num[words_dis_emotion[i].emotion]) max_emotion = words_dis_emotion[i].emotion;
//							//�����ֶ����������У����ж������ͣ�����Ϊmax_emotion
//							else if (knn_tem_num[max_emotion] == knn_tem_num[words_dis_emotion[i].emotion]){
//								if(knn_tem_sum_dis[max_emotion] < knn_tem_sum_dis[words_dis_emotion[i].emotion])
//									max_emotion = words_dis_emotion[i].emotion;
//							} 
//						}	
//					}
				}
				
				//��ǰk��������ĳ��и�������һ��ʱֱ����Ϊmax_emotion 
				for(int i=1;i<=6;i++){
					if(knn_tem_num[i] > k/2) max_emotion = i;
				} 
				
				//�����У�����¼׼ȷ�� 
				outf << emotion_string(max_emotion) << endl;
				if(emotion_string(max_emotion) == emotion_str) true_emotion_num++;
				
		    }
			cout << "method: "<<method << ", k: " << k << ", ׼ȷ��: " << double(true_emotion_num / 311.0) << endl;
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
