#include <regex>
#include <iostream>

using namespace std;

int main(){
	string word = R"(-9.85 da 89 )";
	string rxs = R"(([0-9]+)@(qq.com)(.*\)(^[w_0-9].*)(\..*))";
	regex rx(R"((-[0-9]\.*[1-9][0-9]*))");
	cmatch resault;
	regex_search(word.c_str(),resault, rx);
	for (int i = 0; i < resault.size(); i++)
		cout << resault[i]<<endl;
	system("pause");
	return 0;  
}