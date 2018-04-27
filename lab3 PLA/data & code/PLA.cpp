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

//��¼��ͨѵ�������ݼ���ǩ 
struct Item{
	double data[100];
	bool value;
	Item(){
		data[0] = 1;
	}
};
Item Train_set[5000];

//��¼�������άѵ�������ݼ���ǩ 
struct Item_high_dim{
	double data[5000];
	bool value;
	Item_high_dim(){
		data[0] = 1;
	}
};
Item Train_set_high_dim[5000];

int cnt_col = 1;//��¼������ 
int cnt_row = 0;//��¼������ 
int cnt_col_high_dim = 0;

//�����˷� 
double vector_mul(double a[], double b[], int dim){
	double result = 0;
	for(int i = 0; i < dim; i++){
		result += a[i] * b[i];
	}
	return result;
}

//���ź��� 
bool sign(double a){
	if(a > 0) return true;
	else return false;
}

//��ͨѵ������������ָ����� 
double train_pingce_col(double w[]){
	int tp = 0, fn = 0, tn = 0, fp = 0;
	for(int i = 0; i < cnt_row; i++){
		if(sign(vector_mul(w, Train_set[i].data, cnt_col)) == Train_set[i].value){
			if(Train_set[i].value == true) tp++;
			else tn++;
		}else{
			if(Train_set[i].value == true) fn++;
			else fp++;
		}
	}
	double accuracy = double(tp+tn) / double(tp+fp+tn+fn);
	double recall = double(tp) / double(tp+fn);
	double precision = double(tp) / double(tp+fp);
	double f1 = double(2*precision*recall) / double(precision+recall);
	return f1;
}
	
//�������άѵ������������ָ����� 
double train_pingce_col_high_dim(double w[]){	
	int tp = 0, fn = 0, tn = 0, fp = 0;
	for(int i = 0; i < cnt_row; i++){
		if(sign(vector_mul(w, Train_set_high_dim[i].data, cnt_col_high_dim)) == Train_set_high_dim[i].value){
			if(Train_set_high_dim[i].value == true) tp++;
			else tn++;
		}else{
			if(Train_set_high_dim[i].value == true) fn++;
			else fp++;
		}
	}
	double accuracy = double(tp+tn) / double(tp+fp+tn+fn);
	double recall = double(tp) / double(tp+fn);
	double precision = double(tp) / double(tp+fp);
	double f1 = double(2*precision*recall) / double(precision+recall);
	return f1;
}

//���ź���boolתdouble 
double value_bool_to_double(bool value){
	if(value == true) return 1.0;
	else if(value == false) return -1.0;
} 

//���ź���boolתint 
int value_bool_to_int(bool value){
	if(value == true) return 1;
	else if(value == false) return -1;
} 

int main(){
	clock_t start_time = clock();//��¼��������ʱ�� 
	
	//ѵ�������ݶ�ȡ 
	ifstream train_file("train.csv");
    if(train_file){
    	string mydata_str;//��¼��ȡ�ַ��� 
    	bool first_traversal = true;//���ڼ�¼����ʱ����ʱ�жϱ��� 
	    while(getline(train_file, mydata_str)){
	    	
	    	//��¼ÿ�����ݵ�valueֵ 
	    	int pos = mydata_str.rfind(',');//����Ѱ�ҵ�һ��Tab���ֵ�λ�� 
	    	string value_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//��ȡֵ�ַ��� 
	    	
			if(value_str == "1") Train_set[cnt_row].value = true;
	    	else if(value_str == "-1") Train_set[cnt_row].value = false;
	    	else cout << "Error" << endl;
			
			//��¼ÿ�����ݵ�dataֵ 
			mydata_str = mydata_str.substr(0, pos);//��ȡ��Ч�ַ���
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = ","; //�Կո�����ȡ�� 
	        char* data_tem_c;  
	        data_tem_c = strtok(mydata_c, d); //��ȡ��Ч���� 
	        int col_tem = 1;//��¼��������ǰ�еĵڼ��� 
	        while(data_tem_c){  
	            Train_set[cnt_row].data[col_tem] = atof(data_tem_c);//���ַ���ת��Ϊdouble���ʹ���data 
	            col_tem++;//��ǰ������++ 
	            if(first_traversal == true) cnt_col++;//����Ǳ�����һ����������++		
	            data_tem_c = strtok(NULL, d);
	        }
	        first_traversal = false;
	        cnt_row++;
	    }
	    train_file.close();
	}else{
		cout << "open fail" << endl;
	}
	
	//ѵ��������ѵ��
	int method = 2;//ѡ��ѵ��ģʽ 
	srand(time(0));//��������
//	cout<<"���ӣ� "<< time(0) <<endl;
	double w0[5000];
	for(int i = 0; i < 5000; i++) w0[i] = 1.0;//��ʼ��wֵ 
//	for(int i = 0; i < 5000; i++) w0[i] = double(rand()%100);//��ʼ��wֵ 
	// 1508935028 30 
	double w0_accuracy = 0;
	double w1[5000];
	for(int i = 0; i < 5000; i++) w1[i] = w0[i];
	double w1_accuracy = 0;
	

	
	//ԭʼPLA�㷨 
	if(method == 1){
		int iter = 5000;//���õ������� 
		while(iter--){
			//����ȫ��ѵ�������� 
			for(int i = 0; i < cnt_row; i++){
				//�ж�Ԥ��ֵ�Ƿ�׼ȷ 
				if(sign(vector_mul(w0, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//����Ȩ������ 
					for(int j = 0; j < cnt_col; j++){
						w0[j] = w0[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
				}
			}
		}
	//�ڴ��㷨 
	}else if(method == 2){
		int iter = 2000;//���õ������� 
		double old_train_accuracy_col = train_pingce_col(w0);
		while(iter--){
			cout<<iter<<endl;
			//����ȫ��ѵ�������� 
			for(int i = 0; i < cnt_row; i++){
				//�ж�Ԥ��ֵ�Ƿ�׼ȷ 
				if(sign(vector_mul(w1, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//����Ȩ������ 
					for(int j = 0; j < cnt_col; j++){
						w1[j] = w1[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
					//�����ǰȨ������Ч����������Ȩ���������������Ȩ������ 
					double new_train_accuracy_col= train_pingce_col(w1);
					if(new_train_accuracy_col > old_train_accuracy_col){	
						for(int j = 0; j < cnt_col; j++){
							w0[j] = w1[j];
						}
						old_train_accuracy_col = new_train_accuracy_col;
					}
				}
			}
		}
	//�ڴ��㷨�����ɰ棩 
	}else if(method == 3){
		int iter = 2000;//���õ������� 
		double old_train_accuracy_col = train_pingce_col(w0);
		while(iter--){
			//����ȫ��ѵ�������� 
			for(int i = 0; i < cnt_row; i++){
				//�ж�Ԥ��ֵ�Ƿ�׼ȷ 
				if(sign(vector_mul(w1, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//����Ȩ������ 
					for(int j = 0; j < cnt_col; j++){
						w1[j] = w1[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
				}
			}
			//�����ǰȨ������Ч����������Ȩ���������������Ȩ������ 
			double new_train_accuracy_col= train_pingce_col(w1);
			if(new_train_accuracy_col > old_train_accuracy_col){	
				for(int j = 0; j < cnt_col; j++){
					w0[j] = w1[j];
				}
				old_train_accuracy_col = new_train_accuracy_col;
			}
		}
	//�ڴ��㷨���ս��棩 
	}else if(method == 4){
		int iter = 5;//���õ������� 
		double old_train_accuracy_col = train_pingce_col(w0);
		while(iter--){
			//����ȫ��ѵ�������� 
			for(int i = 0; i < cnt_row; i++){
				//�ж�Ԥ��ֵ�Ƿ�׼ȷ 
				if(sign(vector_mul(w0, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//��¼���º��Ȩ������ 
					for(int j = 0; j < cnt_col; j++){
						w1[j] = w0[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
					//������º��Ȩ������Ч�����ڵ�ǰȨ����������µ�ǰȨ������ 
					double new_train_accuracy_col= train_pingce_col(w1);
					if(new_train_accuracy_col > old_train_accuracy_col){	
						for(int j = 0; j < cnt_col; j++){
							w0[j] = w1[j];
						}
						old_train_accuracy_col = new_train_accuracy_col;
					}
					
				}
			}
		}
	//ģ���˻�ڴ��㷨���ս��棩		
	}else if(method == 5){
		double T=100000;								//ϵͳ���¶ȣ�ϵͳ��ʼӦ��Ҫ����һ�����µ�״̬ 
		double T_min=1;									//�¶ȵ����ޣ����¶�T�ﵽT_min����ֹͣ����
		double dE;
		double r=0.999;								//���ڿ��ƽ��µĿ���
//		srand(time(0));
		srand(1508848158);//r=0.999 0.508453	
		cout<<"���ӣ� "<< time(0) <<endl;
		double old_train_accuracy_col = train_pingce_col(w0);
		while(T > T_min){
			//����ȫ��ѵ�������� 
			for(int i = 0; i < cnt_row; i++){
				//�ж�Ԥ��ֵ�Ƿ�׼ȷ 
				if(sign(vector_mul(w0, Train_set[i].data, cnt_col)) != Train_set[i].value){
					//��¼���º��Ȩ������ 
					for(int j = 0; j < cnt_col; j++){
						w1[j] = w0[j] + Train_set[i].data[j] * value_bool_to_double(Train_set[i].value);
					}
					double new_train_accuracy_col = train_pingce_col(w1);
					
					dE = (new_train_accuracy_col - old_train_accuracy_col);//��¼�¾�Ȩ�غ���ֵ�Ĳ� 
					//����ƶ���õ����Ž⣬�����ǽ����ƶ�
					if(dE > 0){	
						for(int j = 0; j < cnt_col; j++){
							w0[j] = w1[j];
						}
						old_train_accuracy_col = new_train_accuracy_col;
					//����ƶ���һ�������Ž⣬��ͨ���������һ�������ж��Ƿ��ƶ� 
					}else if(exp(dE/T)>((rand()%11)/10.000)){
					//����exp(dE/T)��ȡֵ��Χ��(0,1)��dE/TԽ����exp(dE/T)ҲԽ��	
						for(int j = 0; j < cnt_col; j++){
							w0[j] = w1[j];
						}
					}
					T=r*T;//�����˻� ��0<r<1 ��rԽ�󣬽���Խ����rԽС������Խ��
					/*
		 			��r������������ȫ�����Ž�Ŀ��ܻ�ϸߣ��������Ĺ���Ҳ�ͽϳ���
					��r��С���������Ĺ��̻�ܿ죬�����տ��ܻ�ﵽһ���ֲ�����ֵ
		����		*/ 
				}
			}	
		} 
	//�������ά�㷨 
	}else if(method == 6){
		//����ȫ��ѵ�������ݽ���ת��Ϊ�������ά���� n+C(n��2) 
		for(int i = 0; i < cnt_row; i++){
			int dim = 0;
			Train_set_high_dim[i].value=Train_set[i].value;
			Train_set_high_dim[i].data[0] = 1;//������չ 
			dim++;
			//n��Ϊһ���� 
			for(int j = 1; j < cnt_col; j++){
				Train_set_high_dim[i].data[dim] = Train_set[i].data[j];
				dim++;
			}
			//C(n��2)��Ϊ������ 
			for(int j = 1; j < cnt_col; j++){
				for(int k = j; k < cnt_col; k++){
					Train_set_high_dim[i].data[dim] = Train_set[i].data[j] * Train_set[i].data[k];
					dim++;
				}
			}
			cnt_col_high_dim = dim;
		}
		int iter = 5;//���õ������� 
		double old_train_accuracy_col_high_dim=train_pingce_col_high_dim(w0);
		while(iter--){
			//����ȫ��ѵ��������
			for(int i = 0; i < cnt_row; i++){
				//�ж�Ԥ��ֵ�Ƿ�׼ȷ 
				if(sign(vector_mul(w1, Train_set_high_dim[i].data, cnt_col_high_dim)) != Train_set_high_dim[i].value){
					//����Ȩ������ 
					for(int j = 0; j < cnt_col_high_dim; j++){
						w1[j] = w1[j] + Train_set_high_dim[i].data[j] * value_bool_to_double(Train_set_high_dim[i].value);
					}
					//�����ǰȨ������Ч����������Ȩ���������������Ȩ������ 
					double new_train_accuracy_col_high_dim=train_pingce_col_high_dim(w1);
					if(new_train_accuracy_col_high_dim > old_train_accuracy_col_high_dim){	
						for(int j = 0; j < cnt_col_high_dim; j++){
							w0[j] = w1[j];
						}
						old_train_accuracy_col_high_dim = new_train_accuracy_col_high_dim;
					}
				}
			}
		}
	}
	
	//����ѵ��������ָ�� 
	int tp = 0, fn = 0, tn = 0, fp = 0;
	for(int i = 0; i < cnt_row; i++){
		if(sign(vector_mul(w0, Train_set[i].data, cnt_col)) == Train_set[i].value){
			if(Train_set[i].value == true) tp++;
			else tn++;
		}else{
			if(Train_set[i].value == true) fn++;
			else fp++;
		}
	}
	double accuracy = double(tp+tn) / double(tp+fp+tn+fn);
	double recall = double(tp) / double(tp+fn);
	double precision = double(tp) / double(tp+fp);
	double f1 = double(2*precision*recall) / double(precision+recall);

	cout << "train: " <<endl; 
	cout << "accuracy: " << accuracy << endl;
	cout << "recall: " << recall << endl;
	cout << "precision: " << precision << endl;
	cout << "f1: " << f1 << endl;
	cout << endl;
	
	
	//���Լ�����
	ifstream test_file;
//	test_file.open("val.csv");
	test_file.open("test.csv");

	ofstream result_file;
	result_file.open("test_result.csv");
	
    if(test_file && result_file){
    	Item Test_set;
    	Item Test_set_high_dim;
    	bool isVal = false;
		tp = 0;fn = 0;tn = 0;fp = 0;
    	
    	string mydata_str;//��¼��ȡ�ַ��� 
	    while(getline(test_file, mydata_str)){
	    	
	    	//��¼ÿ�����ݵ�valueֵ 
	    	int pos = mydata_str.rfind(',');//����Ѱ�ҵ�һ��Tab���ֵ�λ�� 
	    	string value_str = mydata_str.substr(pos + 1, mydata_str.length() - pos - 1);//��ȡֵ�ַ��� 
	    	
	    	if(isVal == true){
	    		if(value_str == "1") Test_set.value = true;
		    	else if(value_str == "-1") Test_set.value = false;
		    	else cout << "Error" << endl;
			}
			
			//��¼ÿ�����ݵ�dataֵ 
			mydata_str = mydata_str.substr(0, pos);//��ȡ��Ч�ַ���
			char* mydata_c = (char*)mydata_str.c_str();
			const char* d = ","; //�Կո�����ȡ�� 
	        char* data_tem_c;  
	        data_tem_c = strtok(mydata_c, d); //��ȡ��Ч���� 
	        int col_tem = 1;//��¼��������ǰ�еĵڼ��� 
	        while(data_tem_c){

	            Test_set.data[col_tem] = atof(data_tem_c);//���ַ���ת��Ϊdouble���ʹ���data 

	            col_tem++;//��ǰ������++ 
	            	
	            data_tem_c = strtok(NULL, d);
	        }
			
			bool result_value;
			//�ж��Ƿ�Ϊ�������ά�㷨 
			if(method == 6){
				//����ȫ����֤�������ݽ���ת��Ϊ�������ά���� n+C(n��2) 
				int dim = 0;
				Test_set_high_dim.value=Test_set.value;
				Test_set_high_dim.data[0] = 1;//������չ 
				dim++;
				//n��Ϊһ���� 
				for(int j = 1; j < cnt_col; j++){
					Test_set_high_dim.data[dim] = Test_set.data[j];
					dim++;
				}
				//C(n��2)��Ϊ������ 
				for(int j = 1; j < cnt_col; j++){
					for(int k = j; k < cnt_col; k++){
						Test_set_high_dim.data[dim] = Test_set.data[j] * Test_set.data[k];
						dim++;
					}
				}
				//����Ԥ�Ⲣ��¼��������ָ�� 
				result_value = sign(vector_mul(w0, Test_set_high_dim.data, cnt_col_high_dim));
				if(isVal == true){
					if(result_value == Test_set_high_dim.value){
						if(Test_set_high_dim.value == true) tp++;
						else tn++;
					}else{
						if(Test_set_high_dim.value == true) fn++;
						else fp++;
					}
				}
			}else{
				//����Ԥ�Ⲣ��¼��������ָ�� 
				result_value = sign(vector_mul(w0, Test_set.data, cnt_col));
				if(isVal == true){
					if(result_value == Test_set.value){
						if(Test_set.value == true) tp++;
						else tn++;
					}else{
						if(Test_set.value == true) fn++;
						else fp++;
					}
				}
			}
			//���Ԥ���� 
			result_file << value_bool_to_int(result_value) << endl;
	    }
	    
		if(isVal == true){
			//������֤������ָ�� 
			accuracy = double(tp+tn) / double(tp+fp+tn+fn);
			recall = double(tp) / double(tp+fn);
			precision = double(tp) / double(tp+fp);
			f1 = double(2*precision*recall) / double(precision+recall);
		
			cout << "val: " << endl; 
			cout << "accuracy: " << accuracy << endl;
			cout << "recall: " << recall << endl;
			cout << "precision: " << precision << endl;
			cout << "f1: " << f1 << endl;
			cout << endl;
		}
		
		result_file.close();
	    test_file.close();
	}else{
		cout << "open fail" << endl;
	}
	clock_t finish_time=clock();//��¼��������ʱ�� 
	double totaltime=double(finish_time-start_time)/CLOCKS_PER_SEC;
	cout<<"\n�˳��������ʱ��Ϊ"<<totaltime<<"�룡"<<endl;
	return 0;  
} 
