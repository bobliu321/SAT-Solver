/*
ENSC 351 Assignment 5
Nov 8th, 2018
Keith Leung - 301221899
Bob Liu - 301236133
*/

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include <stack>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <chrono>



using namespace std;


vector <vector<int>> clause;
vector <vector<int>> tt;
vector<bool> answerVector;


ofstream myfile;

//int count = 0;
int num;
int numClause;
int numVar;
bool cond = false;
int countTrue;



// DIMACS file tokenizer
void tokenizer(char *filename) {
    string token;
    string line;

    ifstream from_file;
    from_file.open(filename);
    if (!from_file) {
        cout << "Failed to open file!" << endl;
        exit(1);
    }

// Space and zero as delimiter
    while (getline(from_file, line)) {
        vector<int> variable;
        stringstream ss(line);
        string item;

        while (getline(ss, item, ' ')) {

            if (item != "0" && item != "0\r" && item != "P" && item != "CNF" && item != "p" && item != "cnf") {
                num = stoi(item);
                variable.push_back(num);
            }
        }
        clause.push_back(variable);
    }

// Test tokenizer
    for (int ii = 0; ii < clause.size(); ii++) {
        for (int jj = 0; jj < clause[ii].size(); jj++) {
            cout << clause[ii][jj] << " ";
        }
        cout << endl;
    }
};


void truthTable(int num) {
    int rows = pow(2, num);
    for (int i = 0; i < rows; i++) {
        vector<int> v;
        for (int j = num - 1; j >= 0; j--) {
            int res = (int) (i / pow(2, j)) % 2;
            v.push_back(res);
        }
        tt.push_back(v);
    }
};

void answerArrayInit(int size) {
    for (int i = 0; i < size; i++) {
        answerVector.push_back(false);
    }
    // Test function
    for (int i = 0; i < size; i++) {
        cout << answerVector[i] << " ";
    }
}

void truthConv(){
    for (int ii = 0; ii < tt.size(); ii++) {

        for (int jj = 0; jj < tt[ii].size(); jj++) {
            if(tt[ii][jj]==0){
                tt[ii][jj]=jj+1;
                cout << tt[ii][jj] << " ";
            }
            else if(tt[ii][jj]==1){
                tt[ii][jj]=(jj+1)*(-1);
                cout << tt[ii][jj] << " ";
            }
        }
        cout << endl;
    }
}

//matches converted truthtable with clauses and set boolean vector elements true
void Scan(int var){
    for (int ii = 1; ii < clause.size(); ii++) {

        for (int jj = 0; jj < clause[ii].size(); jj++) {

            if (clause[ii][jj]==var){
                answerVector[ii-1]=true;
            }
        }
        //cout << endl;
    }
}

bool answerCheck(){
    int allTrue=0;
    for(int ii=0; ii<answerVector.size();ii++){

        if(answerVector[ii]==true){
          allTrue++;
        }
    }

    if(allTrue==answerVector.size()){
        return true;
    }
    else{
        return false;
    }
}



int main() {
    // Host
    char *file = (char *) ("text.txt");
    tokenizer(file);
    cout << endl;

    int numVar = clause[0][0];
    cout << "Number of variables: " << numVar << endl;

    int numClause = clause[0][1];
    cout << "Number of clauses: " << numClause << endl;

    cout << "Answer array: ";
    answerArrayInit(numClause);
    cout << endl;

    cout << "Truth table of " << numVar << " variables: \n";
	truthTable(numVar);
	
	
	
    // Test truthTable
    for (int ii = 0; ii < tt.size(); ii++) {

        for (int jj = 0; jj < tt[ii].size(); jj++) {
            cout << tt[ii][jj] << " ";
        }
        cout << endl;
    }
	
	cout << "After transformation: " << endl;
	
	truthConv();

    //Scan each value of the converted truth table into the clauses

    int count=0;

    volatile bool flag=false;

    #pragma omp parallel shared(flag)//shared varibble flag
    {
        #pragma omp single
        {
            #pragma omp task
            {
                cout << "BackTrack Progress: " << count << endl;
            }



            #pragma omp task
            {
                for (int ii = 0; ii < tt.size() && !flag; ii++) {
                    count++;
                    for (int jj = 0; jj < tt[ii].size(); jj++) {
                        {
                            Scan(tt[ii][jj]);
                        }
                    }

                    //check if test solution is correct
                    if (answerCheck()) {
                        myfile.open("output.txt");
                        myfile << "v ";
                        cout << endl << "v ";
                        for (int i = 0; i < tt[ii].size(); i++) {
                            myfile << tt[ii][i] << " ";
                            cout << tt[ii][i] << " ";
                        }
                        cout<<endl;
                        //Stop the for loop and the backtrack progress loop
                        flag = true;
                    }

                    if(ii==(tt.size()-1)){
                        flag=true;//to stop the while condition when no answer is found
                    }


                }
            }

            //running in parallel with first task
            #pragma omp task
            {
                while(!flag){
                    //print Backtrack progress every 2 second
                    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                    cout << "BackTrack Progress: " << count << endl;

                }
            }
        }
    }

    myfile.close();
    return 0;
}