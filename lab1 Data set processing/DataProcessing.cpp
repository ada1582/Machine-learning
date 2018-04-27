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
	//输出 One_hot 三元顺序表 
	void show_One_hot_table(){
		ofstream outf; 
		outf.open("smatrix.txt");	
		outf << rn << endl << cn << endl  << tn << endl; 
		for (int i = 0; i < data.size(); i++){       
	        outf << data[i].row << " " << data[i].col << " 1" <<endl;   
	    }
		outf.close();
	}
	//输出 One_hot 矩阵 
	void show_One_hot_matrix(){
		ofstream outf; 
		outf.open("onehot.txt");
		int cnt = 0;
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//由于 One_hot 矩阵为 0-1 矩阵所以直接输出1 
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
	//输出 TF 矩阵 
	void show_TF_matrix(){
		ofstream outf; 
		outf.open("TF.txt");
		int cnt = 0;	
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//计算的词语出现的次数归一化后的频率
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
	//输出 TFIDE 矩阵 
	void show_TFIDE_matrix(){
		ofstream outf; 
		outf.open("TFIDF.txt");
		int cnt = 0;	
		for(int i = 0; i < rn; i++){
			for(int j = 0; j < cn; j++){
				if(i == data[cnt].row && j == data[cnt].col){
					//计算逆向文件频率再计算 TFIDE 矩阵 
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
//三元顺序表相加矩阵 
void AplusB(){ 
	TMatrix m1, m2;
	ifstream smatrix_A("A.txt"); 
	ifstream smatrix_B("B.txt"); 
	if(smatrix_A && smatrix_B){
		//读取A三元顺序表的值 
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
		//读取B三元顺序表的值 
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
			//写入输出三元顺序表的行列值 
			TMatrix m;
			m.rn = m1.rn;
			outf << m.rn << endl;
			m.cn = m2.cn;
			outf << m.cn << endl;
			
			int cnt = 0;//记录输出三元顺序表的项数 
			int i = 0;//记录A三元顺序表遍历项下标 
			int j = 0;//记录B三元顺序表遍历项下标 
			
			//同时遍历两三元顺序表进行相加操作 
			while(i <= m1.tn && j <= m2.tn){
				if(i == m1.tn && j == m2.tn) break;//遍历结束则退出循环 
				else if(i == m1.tn && j < m2.tn){
					m.data.push_back(m2.data[j]);
					j++;
				}//只剩B三元顺序表项直接放入 
				else if(i < m1.tn && j == m2.tn){
					m.data.push_back(m1.data[i]);
					i++;
				}//只剩A三元顺序表项直接放入 
				else{
					//判断两三元顺序表项的行列号顺序放入 
					if(m1.data[i].row < m2.data[j].row){
						m.data.push_back(m1.data[i]);
						i++;
					}else if(m1.data[i].row == m2.data[j].row){
						if(m1.data[i].col < m2.data[j].col){
							m.data.push_back(m1.data[i]);
							i++;
						}else if(m1.data[i].col == m2.data[j].col){
							//若两三元顺序表项行列好相同则相加值再放入 
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
			//写入输出三元顺序表的项数
			m.tn = cnt;
			outf << cnt << endl;
			//写入输出三元顺序表表项	
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
	clock_t start_time = clock();//记录程序运行时间 
	ifstream mydata_file("semeval.txt");
	map<string, int> word_num;//记录全部数据集每个单词的列号 
	int cnt_col = 0;//记录总列数 
	int cnt_row = 0;//记录总行数 
	TMatrix Data_matrix;
    if(mydata_file){
    	string mydata_str;
	    while(getline(mydata_file, mydata_str)){
	    	map<string, int> word_pos;//记录当前行每个单词的列号 
	    	Data_matrix.row_word_cnt.push_back(0);//当前行的单词数置0 
	    	 
	    	int pos = mydata_str.rfind('\t');//反向寻找第一个Tab出现的位置 
			mydata_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//截取有效字符串
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
	    Data_matrix.show_One_hot_table();
	    Data_matrix.show_One_hot_matrix();
	    Data_matrix.show_TF_matrix();
	    Data_matrix.show_TFIDE_matrix();
	    
		//三元顺序表相加 
		AplusB();
	}else{
		cout << "open fail" << endl;
	}
	clock_t finish_time=clock();//记录程序运行时间 
	double totaltime=double(finish_time-start_time)/CLOCKS_PER_SEC;
	cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<endl;
	return 0;  
} 
