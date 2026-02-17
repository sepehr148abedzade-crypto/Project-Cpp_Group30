#ifndef OPERATORS_FUNCTIONS_H
#define OPERATORS_FUNCTIONS_H
#include "cmath"
#include "string"
#include "random"

template <typename T , typename G>
auto addition(T a,G b){
    return a+b;
}

template <typename T , typename G>
auto difference(T a,G b){
    return a-b;
}

template <typename T , typename G>
auto multiply(T a,G b){
    return a*b;
}

template <typename T , typename G>
auto division(T a,G b){
    if(b==0) return false;
    else return a/b;
}

template <typename T,typename G>
auto pick_random(T min,G max){
    static std::random_device R;
    static std::mt19937 gen(R());
    auto s = min + max;

    if(s != (double)s){
        std::uniform_real_distribution<double>distribution(min,max);
        return distribution(gen);
    }
    else{
        std::uniform_int_distribution<long>distribution(min,max);
        return distribution(gen);
    }
}
template <typename T , typename G>
bool greater_than(T a,G b){
    if(a>b) return true;
    else return false;
}

template <typename T , typename G>
bool less_than(T a,G b){
    if(a<b) return true;
    else return false;
}

template <typename T , typename G>
bool is_equal(T a,G b){
    if(a==b) return true;
    else return false;
}

template <typename T>
auto round(T a){
    return std::round(a);
}
template <typename T>
auto roundUp(T a){
    return std::ceil(a);
}

template <typename T>
auto roundDown(T a){
    return std::floor(a);
}

template <typename T,typename G>
auto mod(T a, G b){
    return fmod(a,b);
}
bool And (bool p,bool q);
bool Or(bool p,bool q);
bool Not(bool p);
std::string join_a_b(std::string a,std::string b);
char letter_a_of_b(int n,std::string a);
int length_of_a(std::string a);
bool a_contains_b(std::string a,std::string b);

#endif //OPERATORS_FUNCTIONS_H
