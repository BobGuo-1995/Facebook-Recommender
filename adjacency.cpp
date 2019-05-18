
// Recommender.cpp : Defines the entry point for the console application.
//
#include <cassert>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctgmath>
#include <map>
#include <unordered_set>
#include <chrono>

using namespace std;

typedef list<pair<int,int>> TAjacency;

//comments based on the Follower Warm Up

//***************************
//algorithm for adjacency list
//return pair(column, max value)
// use 1-indexing 
pair<int, int> GetRecommend(const vector<TAjacency>& adjacencyList, int row/*user u*/)
{
    /*
     if you are wondering how this works, I will try my best to explain here
     1. We get all vertices from "a". There are b,c,d
     code: auto onePathVertices=....
     2. In loop
     we get all vertices from "b". There are a,d,f
     and compute count of two-path vertices
     code: ++m[v.first] -> ++m["a"], ++m["d"], ++m["f"] (if m has not a m["a"] than ++m["a"]=1)
     do the same for vertices c and d
     
     Now m["c"] is number of two-paths from "a" to "c"
     
     */
	--row;//we working with 0-indexing
	pair<int, int> result;
    
    // where we store our num of two path
	map<int, int> m;
	//list of one-path vertices
	auto onePathVertices = adjacencyList[row];
	unordered_set<int> setOnePathVertices;
    for(auto& weightEdge:onePathVertices)setOnePathVertices.emplace(weightEdge.first);

    //compute number of two-paths from user u to each other user
    //lets use the our dear old friend warm up as an example to go through this 
    // first in detail what are we doing here 
    /*
    	for "a"
		onePathVertices={"b","c","d"}
		
		In first loop p will be "b", "c", "d"
		when p="b"
		p2={"a","d","f"}
		
		ok no problem, lets keep going 
		what do we do next: hint see the second for loop 
		we check result for b and increment the end-relationship vertex 
		
		for example: 
		++m["a"]
		++m["d"]
		++m["f"]
		a->b->a m["a"]=1
		a->b->d m["d"]=1
		a->b->f m["f"]=1
		
		let keep tracing for a while and just to make sure, shall we? 
		
		now p = "c"
		What is going on on the second loop? 
		lets see
		++m["e"]
		++n["f"]
		our good old friend the incriminator 
		
		a->c->e m["e"]=1
		a->c->f m["f"]=2
		
		see, what I am doing with my map 
		I am storing the two-way path result
		ok just for fun lets keep going 
		
		where we at now ? p = "d"
		p="d"
		a->d->c m["c"]=1
		a->d->e m["e"]=2
		a->d->f m["f"]=3
		
		what it looks like in our map?????
		it looks magnificent !!!!!!
		
		m["a"] =1,m["d"]=1,m["c"]=1,m["e"]=2,m["f"]=3
		
		and you look at v and you may say what is v doing here, I use it to store my result so 
		I can refer to it later 
		so what it is like in our map 
		it is like 
		<0,1>,<2,1>,<3,1>,<4,2>,<5,3>
        our map is sorted by keys
		
		

    */
	for (auto& p:onePathVertices){
		//list of two-path vertices
		auto p2 =adjacencyList[p.first];
		for (auto v:p2)++m[v.first];
	}
	
	//Select the user who has the most two-paths from u but isn't already followed by u 
	//and isn't u itself
	//further detail to explain this 
	/*
	Here we go, to explain this, it is best we try our second row, user b 
	same we explain it using our good old friend warm-up 
	see el as pointer points to my map 
	
	first tace through our map for user b 
	
	for "b" onePathVertices=["a","d","f"]

	m={} empty

	p="a"
	b->a->b m["b"]=1
	b->a->c m["c"]=1
	b->a->d m["d"]=1

	p="d"
	b->d->a m["a"]=1
	b->d->c m["c"]=2
	b->d->e m["e"]=1
	b->d->f m["f"]=1
	
	p = "f"
	b->f->e
	b->f->h
	
	what is in map m then, 
	m["b"] =1
	m["c"] =2
	m["e"] =2
	m["f"] =1
	m["h"] =1
	m["a"] =1
	lets look what is it like to stored in our map 
	<0,1>,<1,1>,<2,2>,<4,2>,<5,1>,<7,1>,
	
     we use map instead of unordered_map
     because after m[2]=2; m[10]=10;m[5]=5:
     we have
     {2,2}, {5,5}, {10,10}
     sorted list by key
     
     then
     we look up max element from min key to max key
     so if we find same max element with key > key last we don't store it
     
     el.second>result.second
     
     we store el.second only if it more than result.second
     

	*/
	
	for (auto &el : m)
	{
		if (el.second>result.second&&//most two-paths
            setOnePathVertices.find(el.first) == setOnePathVertices.end()&&//isn't already followed by u
            //isn't u itself
            el.first!=row)
		{
			result = el;
		}
	}
	++result.first;
	return result;
}
//there is nothing fancy in main, read in file and apply the get recommend function 

int main(int argc, char* argv[])
{

vector<pair<string,string> > files= {{"A16.txt","rec16.txt"},{"A1024.txt","rec1024.txt"}};
    for(auto &p:files){
    ifstream fInput(p.first);
	if (!fInput.is_open())
	{
		cout << "Can't open file" << endl;
		cout << "Using ./adjacency filename" << endl;
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

	
    
	chrono::high_resolution_clock timer;

	//*************
	//adjacency list
	//*************

	//Create an adjacency list from a matrix
	vector<TAjacency> adjacencyList;
    size_t n=sqrt(temp.size());
    size_t index=0;
	for (size_t i = 0; i < n; ++i)
	{
		TAjacency cols;
		for (size_t j = 0; j < n; ++j)
		{
			if(temp[index++])
                cols.emplace_back(j,1);
		}
		adjacencyList.push_back(cols);
	}

	//algorithm for adjacency list
	 ofstream foutput(p.second);
	auto tm1 = timer.now();
	for (int i = 1; i <= n; ++i)
	{
			auto r = GetRecommend( adjacencyList, i);
        if(r.second)
            foutput<<r.first<<endl;
        else
            foutput<<0<<endl;
	}
	auto tm2 = timer.now();
	auto d = chrono::duration_cast<chrono::duration<double>>(tm2 - tm1);
    cout <<"for "<<p.first<< " time=" <<d.count()  << endl;
    }
	return 0;
}

