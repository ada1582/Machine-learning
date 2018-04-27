#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>  
#include <map>
#include <algorithm>  
#include <list> 
#include <math.h>   
#include <time.h>
#include <stdio.h>  
#include <stdlib.h>
using namespace std;

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
	//��� One_hot ��Ԫ˳��� 
	void show_One_hot_table(){
		ofstream outf; 
		outf.open("smatrix.txt");	
		outf << rn << endl << cn << endl  << tn << endl; 
		for (int i = 0; i < data.size(); i++){       
	        outf << data[i].row << " " << data[i].col << " 1" <<endl;   
	    }
		outf.close();
	}
	//��� One_hot ���� 
	void show_One_hot_matrix(){
		ofstream outf; 
		outf.open("onehot.txt");
		int cnt = 0;
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//���� One_hot ����Ϊ 0-1 ��������ֱ�����1 
					outf << "1 ";
					cnt++;
				}else{
					outf << "0 "; 
				}	
			}
			outf << endl;
		}
		outf.close();
	}
	//��� TF ���� 
	void show_TF_matrix(){
		ofstream outf; 
		outf.open("TF.txt");
		int cnt = 0;	
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//����Ĵ�����ֵĴ�����һ�����Ƶ��
					double TF_tem = double(data[cnt].value) / double(row_word_cnt[i]);
					outf << TF_tem << " ";
					cnt++;
				}else{
					outf<< "0 "; 
				}	
			}
			outf << endl;
		}
		outf.close();
	}
	//��� TFIDE ���� 
	void show_TFIDE_matrix(){
		ofstream outf; 
		outf.open("TFIDF.txt");
		int cnt = 0;	
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//���������ļ�Ƶ���ټ��� TFIDE ���� 
					double TF_tem = double(data[cnt].value) / double(row_word_cnt[i]);
					double TFIDE_tem = TF_tem * (log(double(rn) / (1.0 + double(col_word_cnt[j]))));
					outf << TFIDE_tem << " ";
					cnt++;
				}else{
					outf<< "0 "; 
				}	
			}
			outf<< endl;
		}
		outf.close();
	}
};
//��Ԫ˳�����Ӿ��� 
void AplusB(){ 
	TMatrix m1, m2;
	ifstream smatrix_A("A.txt"); 
	ifstream smatrix_B("B.txt"); 
	if(smatrix_A && smatrix_B){
		//��ȡA��Ԫ˳����ֵ 
    	smatrix_A >> m1.rn;
    	smatrix_A >> m1.cn;
    	smatrix_A >> m1.tn;
    	
    	for(int i = 0; i < m1.tn; i++){
    		Triple m_node;
    		smatrix_A >> m_node.row;
    		smatrix_A >> m_node.col;
    		smatrix_A >> m_node.value;
    		m1.data.push_back(m_node);
    		
		}
		//��ȡB��Ԫ˳����ֵ 
		smatrix_B >> m2.rn;
    	smatrix_B >> m2.cn;
    	smatrix_B >> m2.tn;
		for(int i = 0; i < m2.tn; i++){
    		Triple m_node;
    		smatrix_B >> m_node.row;
    		smatrix_B >> m_node.col;
    		smatrix_B >> m_node.value;
    		m2.data.push_back(m_node);
		}
    	if(m1.rn == m2.rn && m1.cn == m2.cn){
    		ofstream outf; 
			outf.open("smatrix_AplusB.txt");
			//д�������Ԫ˳��������ֵ 
			TMatrix m;
			m.rn = m1.rn;
			outf << m.rn << endl;
			m.cn = m2.cn;
			outf << m.cn << endl;
			
			int cnt = 0;//��¼�����Ԫ˳�������� 
			int i = 0;//��¼A��Ԫ˳���������±� 
			int j = 0;//��¼B��Ԫ˳���������±� 
			
			//ͬʱ��������Ԫ˳��������Ӳ��� 
			while(i <= m1.tn && j <= m2.tn){
				if(i == m1.tn && j == m2.tn) break;//�����������˳�ѭ�� 
				else if(i == m1.tn && j < m2.tn){
					m.data.push_back(m2.data[j]);
					j++;
				}//ֻʣB��Ԫ˳�����ֱ�ӷ��� 
				else if(i < m1.tn && j == m2.tn){
					m.data.push_back(m1.data[i]);
					i++;
				}//ֻʣA��Ԫ˳�����ֱ�ӷ��� 
				else{
					//�ж�����Ԫ˳���������к�˳����� 
					if(m1.data[i].row < m2.data[j].row){
						m.data.push_back(m1.data[i]);
						i++;
					}else if(m1.data[i].row == m2.data[j].row){
						if(m1.data[i].col < m2.data[j].col){
							m.data.push_back(m1.data[i]);
							i++;
						}else if(m1.data[i].col == m2.data[j].col){
							//������Ԫ˳��������к���ͬ�����ֵ�ٷ��� 
							Triple m_node(m1.data[i].row, m1.data[i].col, m1.data[i].value + m2.data[j].value);
							m.data.push_back(m_node);
							i++;
							j++;
						}else{
							m.data.push_back(m2.data[j]);
							j++;
						}
						
					}else{
						m.data.push_back(m2.data[j]);
						j++;
					}
				}
				cnt++;
			}
			//д�������Ԫ˳��������
			m.tn = cnt;
			outf << cnt << endl;
			//д�������Ԫ˳������	
			for(int k = 0; k < cnt; k++){
				outf << m.data[k].row << " " << m.data[k].col << " " << m.data[k].value << endl;
			}
			outf.close();
		}else{
			cout<< "add error" << endl;
		}
	}else{
		cout << "open fail" << endl;
	}
}
int main(){
	clock_t start_time = clock();//��¼��������ʱ�� 
	ifstream mydata_file("semeval.txt");
	map<string, int> word_num;//��¼ȫ�����ݼ�ÿ�����ʵ��к� 
	int cnt_col = 0;//��¼������ 
	int cnt_row = 0;//��¼������ 
	TMatrix Data_matrix;
    if(mydata_file){
    	string mydata_str;
	    while(getline(mydata_file, mydata_str)){
	    	map<string, int> word_pos;//��¼��ǰ��ÿ�����ʵ��к� 
	    	Data_matrix.row_word_cnt.push_back(0);//��ǰ�еĵ�������0 
	    	 
	    	int pos = mydata_str.rfind('\t');//����Ѱ�ҵ�һ��Tab���ֵ�λ�� 
			mydata_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//��ȡ��Ч�ַ���
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
	    Data_matrix.show_One_hot_table();
	    Data_matrix.show_One_hot_matrix();
	    Data_matrix.show_TF_matrix();
	    Data_matrix.show_TFIDE_matrix();
	    
		//��Ԫ˳������ 
		AplusB();
	}else{
		cout << "open fail" << endl;
	}
	clock_t finish_time=clock();//��¼��������ʱ�� 
	double totaltime=double(finish_time-start_time)/CLOCKS_PER_SEC;
	cout<<"\n�˳��������ʱ��Ϊ"<<totaltime<<"�룡"<<endl;
	return 0;  
} 
