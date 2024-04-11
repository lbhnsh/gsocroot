#include<bits/stdc++.h>
#define N 5
#define sp "    "
using namespace std;
string vectorToString(const vector<int>& vec) {
    string result = "{";
    for (int i = 0; i < vec.size(); ++i) {
        result += to_string(vec[i]);
        if (i != vec.size() - 1) {
            result += ", ";
        }
    }
    result += "}";
    return result;
}
void print(string fNX, int size, vector<int>fShape,int axis,int k, string fType,bool sorted, bool max){
    string initialize="int size="+to_string(size)+";\nvector<int>fShape = "+vectorToString(fShape)+";\n";
    initialize+="int axis="+to_string(axis)+";\nint k= "+to_string(k)+";\nstring fType = \""+ fType+"\";\n";
    initialize+="bool sorted = "+to_string(sorted)+";\nbool max  = "+to_string(max)+";";
    cout<<initialize<<endl;
    char id[N]={'i','j','k','l','m'};//can be extended
    int dim=0;
    string inputTensor= fType+" fNX";
    string value="int values";
    string index="int indexes";
    string outputTensorShape="";
    for(int b=0;b<size;b++) inputTensor=inputTensor+"["+to_string(fShape[b])+"]";
    for(int b=0;b<size;b++) outputTensorShape=outputTensorShape+"["+((b==axis)?to_string(k):to_string(fShape[b]))+"]";
    inputTensor+=" = "+(fNX);
    value+=outputTensorShape;
    index+=outputTensorShape;
    cout<<inputTensor<<";\n";
    cout<<value<<";"<<endl<<index<<";"<<endl;
    for(int a=0;a<size;a++){
        if(a==size-1){
            cout<<"vector<pair<"<<fType<<","<<fType<<">>elements;"<<endl;
            for(int i=0;i<a;i++)cout<<sp;

        }
        cout<<"for(int "<<id[a]<<"=0;"<<id[a]<<"<fShape["<<((a<size-1)?((a==axis)?++dim:dim):axis)<<"];"<<id[a]<<"++){"<<endl;
        for(int i=0;i<=a;i++){
            cout<<sp;
        }
        dim++;
    }
    dim=0;
    string element="fNX";
    string indexes="";
    for(int i=0;i<size;i++){
        indexes+="[";
        if(i==axis){
            indexes+=id[size-1];
            dim--;
        }
        else {
            indexes+=id[dim];
        }
        indexes+="]";
        dim++;
    }
    element+=indexes;
    cout<<"elements.push_back({"<<element<<","<<id[size-1]<<"});";
    cout<<endl;
    for(int i=0;i<size-1;i++)cout<<sp;
    cout<<"}"<<endl;
    for(int i=0;i<size-1;i++)cout<<sp;
    if(sorted){
        if(max){
            cout<<"sort(elements.begin(),elements.end(),[](pair<int,int>a,pair<int,int>b){return a.first>b.first;})";
        }
        else cout<<"sort(elements.begin(),elements.end(),[](pair<int,int>a,pair<int,int>b){return a.first<b.first;})";
    }
    else cout<<"sort(elements.begin(),elements.end())";
    cout<<";"<<endl;
    for(int i=0;i<size-1;i++)cout<<sp;
    char itr=(id[size-1]);
    cout<<"for(int "<<itr<<"=0;"<<itr<<"<"<<k<<";"<<itr<<"++){"<<endl;
    for(int i=0;i<size;i++) cout<<sp;
    cout<<"values"<<indexes<<"=elements["<<itr<<"].first;\n";
    for(int i=0;i<size;i++)cout<<sp;
    cout<<"indexes"<<indexes<<"=elements["<<itr<<"].second;\n";
    for(int i=0;i<size-1;i++)cout<<sp;
    cout<<"}"<<endl;    
    for(int c=size-1;c>0;c--){
        for(int i=c-2;i>=0;i--)cout<<sp;
            cout<<"}"<<endl; 
    }

}   
int main(){
    vector<int>fShape({2,3,4});
    // vector<int>fShape({2,4});
    string fNX="{{{1, 2, 7, 8 },{5, 6, 3, 4},{9, 10, 0, 12}},{{13, 14, 15, 16},{17, 18, 19, 20},{22, 21, 23, 24}}}";
    // string fNX="{{22, 21, 23, 24},{2, 1, 3, 4}}";
    print(fNX,fShape.size(),fShape,1,2,"int",1,1);
}


// trying to reshape the topK elements
// // #include<iostream>

// // using namespace std;
// // int main(){
// //     int array[6] = { 1, 2, 3, 4, 5, 6 };
// //     // Reinterpret the array with different indices
// //     int(*array_pointer)[3][2] = reinterpret_cast<int(*)[3][2]>(array);

// //     for (int x = 0; x < 3; ++x) {
// //         for (int y = 0; y < 2; ++y)
// //             std::cout << (*array_pointer)[x][y] << " ";
// //         std::cout << std::endl;
// //     }
// // }

// // printing the values
//     for(int a=0;a<size;a++){
//         cout<<"for(int "<<id[a]<<"=0;"<<id[a]<<"<"<<outputTensorShape[a]<<";"<<id[a]<<"++){"<<endl;
//         for(int i=0;i<=a;i++){
//             cout<<sp;
//         }
//         cout<<"cout<<values"<<;
//         for(int c=size-1;c>0;c--){
//            for(int i=c-2;i>=0;i--)cout<<sp;
//                 cout<<"}"<<endl; 
//         }
//     }