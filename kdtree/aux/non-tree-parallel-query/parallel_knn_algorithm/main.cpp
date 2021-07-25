#include <iostream>
#include <cmath>
#include <fstream>
#include <omp.h>
#include <ctime>
#include <stdlib.h>
using namespace std;
#ifdef _OPENMP
#else
    #define omp_get_thread_num() 0
#endif
// Compute Levenshtein Distance(distance between strings)
size_t uiLevenshteinDistance(const std::string &s1, const std::string &s2)
{
	const size_t m(s1.size());
	const size_t n(s2.size());
	
	if( m==0 ) return n;
	if( n==0 ) return m;
	
	size_t *costs = new size_t[n + 1];
	
	for( size_t k=0; k<=n; k++ ) costs[k] = k;
	
	size_t i = 0;
	for ( std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i )
	{
		costs[0] = i+1;
		size_t corner = i;
		
		size_t j = 0;
		for ( std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j )
		{
			size_t upper = costs[j+1];
			if( *it1 == *it2 )
			{
				costs[j+1] = corner;
			}
			else
			{
				size_t t(upper<corner?upper:corner);
				costs[j+1] = (costs[j]<t?costs[j]:t)+1;
			}
			
			corner = upper;
		}
	}
	
	size_t result = costs[n];
	delete [] costs;
	
	return result;
}

//function for converting string to float
float stringtofloat(string s){
	float m = atof(s.c_str());
	return m;
}


int main()
{
	int const n = 2500 ; int const s = 100; int k = 1;
	string sample[n][s] ;
	string description[n];
	string newsample[s];
	ifstream filesample,filedescription,filenewsample;
	//inputing data for sample
	filesample.open ("data1.txt", ios::in);
	
	for (int i=0; i<n; i++){
		for (int j=0; j<s; j++){
			filesample>> sample[i][j];
			// cout<<" "<< sample[i][j]<<"\t\t";
		}
		//cout<<"\n\n";
	}
	filesample.close();
	// cout<<endl;
	//inputing data for description
	filedescription.open ("data2.txt", ios::in);
	//cout<<" the remarks for the training data sets respectively are \n\n";
	for (int i=0;i<n;i++){
		filedescription >> description[i];
		//cout<< description[i]<<" ";
	}
	filedescription.close();
	//  cout<<endl;
	//inputing data for newsample
	filenewsample.open ("data3.txt", ios::in);
	//cout<< "\nthe tested samples are "<< "\n\n";
	for (int i=0;i<s;i++){
		filenewsample >> newsample[i];
		// cout<< newsample[i]<<"\t\t";
	
	}
	filedescription.close();
	//cout<<"\n";
	clock_t startTime = clock();
	float dresult[n] ;// dresult = distance calculated between sample and newsample
	float sum1,sum2, sumt;
	float samplec[n][s];
	float newsamplec[s];
	cout<< endl;
	#pragma omp parallel for private (sum1,sum2,sumt,i)
	for (int j=0; j<n; j++){
		sum1 = 0; sum2 = 0;
		for (int i=0; i<s; i++){
			samplec[j][i]= stringtofloat(sample[j][i]);
			newsamplec[i] = stringtofloat(newsample[i]);
			
			//cout<<" converted "<< sample[j][i]<<"\t"<<samplec[j][i]<<"\t"<<newsample[i]<<"\t"<<newsamplec[0]<<endl;
			if ( (samplec[j][i] ) && (newsamplec[i]) != 0 )
			sum1 = sum1 + pow((samplec[j][i]- newsamplec[i]),2);
			else{ sum2 = (sum2 + uiLevenshteinDistance(sample[j][i],newsample[i])); }
			//cout<< sum1<<"and "<<sum2<<endl;
		}
		sumt=  pow(sum1,0.5);
		
		dresult[j]= sum2 + sumt;
		//  cout<< dresult[j]<<"\t"<<description[j]<<endl;
	}
	//Sorting distance(dresult)from low to high with its corresponding description
	
	for (int i = 0; i < n-1;i++) {
		for (int j =i+1; j < n;j++){
			if (dresult[i]> dresult[j]) {
				double temp1 = dresult[i];
				dresult[i]= dresult[j];
				dresult[j]= temp1 ;
				string temp2 = description[i];
				description[i]= description[j];
				description[j]= temp2 ;
			}
		}
	}
	
	/*cout<<"The distance calculated and its corresponding remark are \n";
	for(int i=0;i<n;i++){
	cout<<dresult[i]<<" \t "<<description[i]<<endl;
	}*/
	
	// counting similar words from description (result) after choosing k value from knn algorithm
	//cout<<"\n The majority counting for the remarks \n\n";
	int count[k];
	//#pragma omp parallel for
	for ( int i = 0; i < k; i++ )
	{
		count[i] = 0;
		int duplicate = 1;
		for ( int j = 0; j < i; j++ ) {
			if ( description[i].compare(description[j]) == 0)
			duplicate = duplicate + 1;
		}
		if ( duplicate == 1 )
		{
			for (int r = 0; r < k ; r++ )
			{
				if ( description[i].compare(description[r]) ==0 )
				count[i] = count[i] + 1;
			}
			// cout << description[i] << " in number was " << count[i] <<endl;
		}
	}
	//counting description with highest frequency within the value of k
	for (int i = 0; i < k-1;i++) {
		for (int j =i+1; j < k;j++) {
			if (count[i]< count[j]) {
				int temp3 = count[i];
				count[i]= count[j];
				count[j]= temp3 ;
				string temp4 = description[i];
				description[i]= description[j];
				description[j]= temp4 ;
			}
		}
	}
	
	cout<<"\nThe better choice is "<<description[0]<<endl;
	clock_t endTime = clock();
	clock_t clockTicksTaken = endTime - startTime;
	double timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
	cout<<"THE EXECUTION TIME  IN SECONDS IS  "<< timeInSeconds;
	
	return 0;
	
}

