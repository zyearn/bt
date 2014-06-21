#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <node/v8.h>
#include <node/node.h>
#include "fft.h"
using namespace v8;  
using namespace std;

Handle<Value> Method(const Arguments& args) {
    HandleScope scope;

    if (system("cd addon && ./clean.sh"))
        return scope.Close(String::New("succ"));
    else
        return scope.Close(String::New("fail"));
}


void init(Handle<Object> target) {
  target->Set(String::NewSymbol("clean"),
      FunctionTemplate::New(Method)->GetFunction());
}

NODE_MODULE(clean, init)
