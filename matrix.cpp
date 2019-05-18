// Recommender.cpp : Defines the entry point for the console application.
//
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctgmath>
#include <utility>
//#include <unordered_map>
//#include <unordered_set>
#include <chrono>

using namespace std;
//The class definition virtual matrix on real data (vector<int>)
//We have one shared data
//And many objects of this class - matrices
//It's reduce  a memory allocation
class SquareMatrix{
public:
    
    size_t maxN ;//size of main big matrix. It's always const value
	vector<int> *data;//pointer to data. It's always const value
	
	//submatrix values
	size_t n;//size of submatrix n*n
	//start cell of submatrix
    size_t startRow,startColumn;
    
    
	SquareMatrix() :maxN(0),data(nullptr),n(0),startRow(0),startColumn(0){}
	//create matrix from vector
	SquareMatrix(vector<int>* v):SquareMatrix() {
		double t = sqrt(v->size());
		int d = (int)t;
		//check. Is a square matrix?
		assert(t == d);
		n = d;
        maxN=n;
		data = v;
	}
	//copy constructor
	SquareMatrix(const SquareMatrix& right):SquareMatrix(){
		n = right.n;
		data = right.data;
        maxN=right.maxN;
        startRow=right.startRow;
        startColumn=right.startColumn;
	}
	//constructor for creating the submatrices from the main big matrix
	//we use it to divide the matrix into 4 submatrices
	SquareMatrix(const SquareMatrix& right/*main matrix*/,size_t newN,size_t newStartRow,size_t newStartColumn) {
		data=right.data;
        n=newN;
        startRow=right.startRow+newStartRow;
        startColumn=right.startColumn+ newStartColumn;
        maxN=right.maxN;
	}
	
	//return dimension of matrix n*n
	size_t GetSize(){ return n; }

	//Get value from m[row,column]
	int Get(int row, int column) const
	{
		return (*data)[(startRow+row)*maxN +startColumn+ column];
	}
	//Set value to m[row,column]
	void Set(int row, int column, int value)
	{
		(*data)[(startRow+row)*maxN +startColumn+ column] = value;
	}
	//assign operator for copy matrix
	//not using 
	SquareMatrix operator = (const SquareMatrix& right)
	{
		data = right.data;
		n = right.n;
        maxN=right.maxN;
        startRow=right.startRow;
        startColumn=right.startColumn;
		return *this;
	}
	
	//Save matrix to txt file
	void Save(string fname)
	{
		ofstream f1(fname);
		if (!f1.is_open())throw invalid_argument("Can't open file " + fname + " for write");
		for (size_t i = 0; i < n; ++i)
		{
			for (size_t j = 0; j < n; ++j){
				f1 << Get(i,j)<< " ";
			}
			f1 << endl;
		}
	}
	
	
	
};
//**********
//simple Divide and Conquer
	//   |a,b| * |e,f| = |ae+bg,af+bh| 
	//   |c,d|   |g,h|   |ce+dg,cf+dh|
	void Multiply(SquareMatrix left, SquareMatrix right, SquareMatrix& result)
	{
        //We can stop recursion for matrix size = 1  (left.n==1)
        //if (left.n == 1){result.Set(0,0,result.Get(0,0)+left.Get(0,0)*right.Get(0,0));}
        //But I think for n==2 it's more clearly
		if (left.n == 2){
			//end recursion
            int a=left.Get(0,0); int b=left.Get(0,1);
            int c=left.Get(1,0); int d=left.Get(1,1);
            int e=right.Get(0,0);int f=right.Get(0,1);
            int g=right.Get(1,0);int h=right.Get(1,1);
			result.Set(0,0,result.Get(0,0)+a*e+b*g);
            result.Set(0,1,result.Get(0,1)+a*f+b*h);
            result.Set(1,0,result.Get(1,0)+c*e+d*g);
            result.Set(1,1,result.Get(1,1)+c*f+d*h);
			return;
		}
		//Divide the matrix into four smaller matrices with half size 
		size_t N = left.n / 2;
		SquareMatrix a(left,N,0,0), b(left,N,0,N), 
                     c(left,N,N,0), d(left,N,N,N);
        SquareMatrix e(right,N,0,0), f(right,N,0,N), 
                     g(right,N,N,0), h(right,N,N,N);
	//Divide the relult matrix into four smaller matrices with half size 	
SquareMatrix r11(result,N,0,0),r12(result,N,0,N),r21(result,N,N,0),r22(result,N,N,N);
		//Now we have 3 matrices with submatrices
		//   |a,b|   |e,f|   |r11,r12| 
	    //   |c,d|   |g,h|   |r21,r22|
		Multiply(a, e,r11);
        //result
        // |r11+a*e,r12|
        // |r21    ,r22|
        Multiply(b, g,r11);
        //result
        // |r11+a*e+b*g,r12|
        // |r21        ,r22|
        Multiply(a, f,r12); 
        //result
        // |r11+a*e+b*g,r12+a*f|
        // |r21        ,r22    |
        Multiply(b, h,r12);
         //result
        // |r11+a*e+b*g,r12+a*f+b*h|
        // |r21        ,r22        |
        Multiply(c, e,r21); 
         //result
        // |r11+a*e+b*g,r12+a*f+b*h|
        // |r21+c*e    ,r22        |
        Multiply(d, g,r21);
         //result
        // |r11+a*e+b*g,r12+a*f+b*h|
        // |r21+c*e+d*g,r22        |
        Multiply(c, f,r22);
         //result
        // |r11+a*e+b*g,r12+a*f+b*h|
        // |r21+c*e+d*g,r22+c*f    |
         Multiply(d, h,r22);
          //result
        // |r11+a*e+b*g,r12+a*f+b*h|
        // |r21+c*e+d*g,r22+c*f+d*h|
	}
	
//recommendation for user u is then the column index w of the largest value in row u of B such that A uw =0
	//return pair(column, max value)
	// use 1-indexing 
	pair<int, int> GetRecommend(const SquareMatrix& a, const SquareMatrix& b, int row/*user u*/)
	{
		--row;//In this code we use 0-indexing
		size_t n = a.n;
		pair<int, int> result;
		for (int i = 0; i < n; ++i)
		{
			int bVal = b.Get(row, i);
			if (i!=row&&a.Get(row, i) == 0 && result.second < bVal)
			{
				result.first = i; result.second = bVal;
			}
		}
		++result.first;//return 1-indexing result
		return result;
	}



int main(int argc, char* argv[])
{
// 	if (argc != 2){
// 		cout << "Using ./matrix filename" << endl;
// 		return 0;
// 	}
	
//     string fname = argv[1];
	vector<pair<string,string> > files= {{"A16.txt","rec16.txt"},{"A1024.txt","rec1024.txt"}};
    for(auto &p:files){
    ifstream fInput(p.first);
	if (!fInput.is_open())
	{
		cout << "Can't open file" << endl;
		cout << "Using ./matrix filename" << endl;
		return 0;
	}
	
	vector<int> temp;
	while (!fInput.eof())
	{
		int val = -1;
		fInput >> val;
		if (val >= 0)
			temp.push_back(val);
	}

	SquareMatrix a(&temp);
    vector<int> temp1(temp.size());
    SquareMatrix b(&temp1);

    
    ofstream foutput(p.second);
	chrono::high_resolution_clock timer;
	auto tm1=timer.now();
	 Multiply(a, a,b);
	size_t n = a.GetSize();
	for (int i = 1; i <= n; ++i)
	{
		auto r = GetRecommend(a, b, i);
        if(r.second)
            foutput<<r.first<<endl;
        else
            foutput<<0<<endl;
        	
	}
	auto tm2 = timer.now(); 
	auto d = chrono::duration_cast<chrono::duration<double>>(tm2 - tm1);
	cout <<"for "<<p.first<< " time=" <<d.count()  << endl;
	//Save A*A matrix for debug
// 	b.Save("result.txt");
    }
    
	

	return 0;
}

