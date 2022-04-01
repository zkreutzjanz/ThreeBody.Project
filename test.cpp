#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>

using namespace std;

class chunk{
    private:
    int size = 0;
    vector<unsigned char> type ={0,0,0,0};
    vector<unsigned char> CRC ={0,0,0,0};

    public:
    chunk(){};
    chunk(unsigned char type){

    }

    void setData(unsigned char data[]){
        
    }

    unsigned char* prepare(){
        
    }

};

string meXOR(string a,string b){
    string val;
    for(int i =0;i<a.length();i++){
        val.append(to_string(((a.at(i)-0)^(b.at(i)-0))-0));
    }
    return val;
}
string lefttrim(string a){
   for(int i =0;i<a.length();i++){
        if(a.at(i)=='1'){
            return a.substr(i,a.length()-i);
        }
    }
    return "0";
}
string CRC(string data,string divisor){
    int l = data.length();
    cout<<"CRC:"<<data<<":"<<divisor<<"\n";
    while(true){
        string c = lefttrim(data);
        if(c.length()>=divisor.length()){
            string comp = divisor;
            data = meXOR(comp.append(c.length()-divisor.length(),'0'),c);
            string addo = "";
            addo.append(l-c.length(),' ');
            cout<<"Data:"<<addo<<c<<"\n";
            cout<<"COMP:"<<addo<<comp<<"\n";
        }else{
            return c;
        }
    }
}

int main(){


    string data = "0100100101000100010000010101010000011000010101110110001111111000100011110001101111010000010101111110111011111111011111110000000011000111101111101000011001111010";   
    data.append("11000000011000001010010110111101");//11101011010100010011000001000010
               //11000000011000001010010110111101                                                                                                 
    string divisor = "100000100110000010001110110110111";
    string crc = CRC(data,divisor);

   
    cout<<"CRC("<<data<<","<<divisor<<")="<<crc<<"\n";
    
}