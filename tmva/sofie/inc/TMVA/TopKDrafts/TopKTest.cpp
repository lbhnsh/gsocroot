#include <bits/stdc++.h>

using namespace std;

int main()
{
int size=3;
vector<int>fShape = {2, 3, 4};
int axis=1;
int k= 2;
string fType = "int";
bool sorted = 1;
bool max  = 1;
int fNx[2][3][4] = {{{1, 2, 7, 8 },{5, 6, 3, 4},{9, 10, 0, 12}},{{13, 14, 15, 16},{17, 18, 19, 20},{22, 21, 23, 24}}};
int values[2][2][4];
int indexes[2][2][4];
for(int i=0;i<fShape[0];i++){
    for(int j=0;j<fShape[2];j++){
        vector<pair<int,int>>elements;
        for(int k=0;k<fShape[1];k++){
            elements.push_back({fNx[i][k][j],k});
        }
        sort(elements.begin(),elements.end(),[](pair<int,int>a,pair<int,int>b){return a.first>b.first;});
        for(int k=0;k<2;k++){
            values[i][k][j]=elements[k].first;
            indexes[i][k][j]=elements[k].second;
        }
    }
}

    // for (int i=0;i<2;i++){
    //     cout<<indexes[i]<<" ";
    // }
    // cout<<endl;
    // for (int i=0;i<2;i++){
    //     cout<<values[i]<<" ";
    // }

    // for (int i = 0; i < 2; ++i) {
    //     for (int j = 0; j < 3; ++j) {
    //         // for (int k = 0; k < 2; ++k) {
    //             cout << values[i][j] << " ";
    //         // }
    //         // cout << endl;
    //     }
    //     cout << endl;
    // }

    // the above script is auto-generated
    // below just printing the indexes array and values array
    cout<<"values:\n";
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 4; ++k) {
                cout << values[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    cout<<"indexes:\n";
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 4; ++k) {
                cout << indexes[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}