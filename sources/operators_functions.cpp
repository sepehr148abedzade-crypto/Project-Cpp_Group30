#include "operators_functions.h"
#include "iostream"

bool And (bool p,bool q){
    if(p && q ) return true;
    else return false;
}

bool Or(bool p,bool q){
    if(p || q) return true;
    else return false;
}

bool Not(bool p){
    return !p;
}

std::string join_a_b(std::string a,std::string b){
    return a+b;
}

char letter_a_of_b(int n,std::string a){
    if(n-1 >=0 && n-1<=a.length()) return a[n-1];
}

int length_of_a(std::string a){
    return a.length();
}

bool a_contains_b(std::string a,std::string b){
    if(a.find(b)!=std::string::npos) return true;
    else return false;
}