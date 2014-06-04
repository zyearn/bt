#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <node/v8.h>
#include <node/node.h>
#include "global_params.h"
#include "fft.h"
using namespace v8;  
using namespace std;

#define CHECK_UP 1.7
#define CHECK_DOWN (1.0 / CHECK_UP)
#define CHUNK_SIZE 1024
#define FILE_LIB "./addon/filelib"
#define FILE_NUM "./addon/filenum"
#define EP 0.4

int fft(char *file_name, vector<double> &darr) {
	unsigned char* source = (unsigned char*)calloc(sizeof(unsigned char), MAXLEN+1);
	unsigned int length;
	ifstream ifs;

	cout<<"open file "<<file_name<<endl;
	ifs.open(file_name);
	if(ifs.is_open()) {
		ifs.read((char*)source,MAXLEN);
		length = ifs.gcount();
		ifs.close();
	}
	else {
		fprintf(stderr, "ERROR: Can not open the file %s\n", file_name);
		return -1;
	}
	cout << "len is " << strlen((const char*)source) << " & length is " << length << endl;
	
    complex *pSignal = new complex[MAXLEN];
	for(unsigned int i=0; i<length; i++) {
		pSignal[i] = (double)source[i];
	}
	//   Apply FFT
	CFFT::Forward(pSignal, length);
	 
    for (size_t i = 0; i<length; ++i)
    {
        darr.push_back(pSignal[i].norm());
        //printf("i = %zu, value = %lf\n", i, pSignal[i].norm());
    }

    delete[] pSignal;
	return THES_SUCC;
}

int calc_simi(vector<double> &da, vector<double> &db, double &result)
{
    if (da.size() != db.size())
    {
        fprintf(stderr, "two vector size must be equal.\n");
        return THES_FAIL;
    }

    size_t vsize = da.size();
    int count = 0;
    for(size_t i=0; i<vsize; ++i)
    {
        double qt = da[i] / db[i];

        if (qt > CHECK_UP || qt < CHECK_DOWN)
            continue;

        count++;
    }
    
    result = double(count) / vsize;
    return THES_SUCC;
}

Handle<Value> Method(const Arguments& args) {
    HandleScope scope;
    char *c_arg;

    if (args.Length() < 1) 
    {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }
    v8::String::Utf8Value str(args[0]->ToString());
    c_arg = *str;
    
    printf("file_name (from source c) = %s\n", c_arg);

	// do fft and get feature from file
    vector<double> narr_1;
	if(fft(c_arg, narr_1) < 0) 
		return scope.Close(Undefined());

    // get current file number
    ifstream fin(FILE_NUM); 
    int file_num;
	if(fin.is_open()) {
        fin >> file_num;
        fin.close();
	}
	else {
		fprintf(stderr, "ERROR: Can not open the file %s\n", FILE_NUM);
		return scope.Close(Undefined());
	}

    cout << "filenum is " << file_num << endl;
    
    // open lib file
    if (file_num > 0)
    {
        ifstream flib(FILE_LIB);
        double result = 0, tot_result = 0;
        long long value;
        for (int file = 0; file < file_num; ++file)
        {
            vector<double> lib_from_file;
            
            for (int col = 0; col < CHUNK_SIZE; ++col)
            {
                flib >> value;
                lib_from_file.push_back(value * 1.0);
            }
            calc_simi(narr_1, lib_from_file, result);
            cout << "result in file " << file << " = " << result << endl;

            tot_result = (result > tot_result) ? result : tot_result;
        }
        
        if (tot_result > EP)    //find duplicate
        {
            cout << "find duplicate! Server will not store and ignore the file\n";
        }
        else
        {
            cout << "it is a new file! Server will store it and update the lib\n";
            
            ofstream fout(FILE_NUM);
            fout << (file_num + 1);
            fout.close();
            
            ofstream flib(FILE_LIB, fstream::app);
            for (int col = 0; col < CHUNK_SIZE; ++col)
                flib << int(narr_1[col]) << endl;
            flib.close();
        }

        char reschar[100];
        sprintf(reschar, "%lf", tot_result);
        return scope.Close(String::New(reschar));
    } 
    else 
    {
        ofstream fout(FILE_NUM);
        fout << (file_num + 1);
        fout.close();

        ofstream flib(FILE_LIB);
        for (int col = 0; col < CHUNK_SIZE; ++col)
            flib << (long long)narr_1[col] << endl;
        flib.close();

        return scope.Close(String::New("0"));
    }
}


void init(Handle<Object> target) {
  target->Set(String::NewSymbol("hello"),
      FunctionTemplate::New(Method)->GetFunction());
}

NODE_MODULE(hello, init)
