#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>

using namespace std;

bool hasNumber(string str) {
	for (int i = 0; i < str.length(); i++)
		if (str[i] >= '0' && str[i] <= '9')	return true;
	return false;
}

double strToDouble(string str) {
	if (str == "" || !hasNumber(str))	return 0;
	string res = (str[0] == '-' ? "-" : "");
	bool flag = true;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == '.' && flag) {
			res += str[i];
			flag = false;
		}
		else if (str[i] >= '0' && str[i] <= '9') res += str[i];
	}
	return stod(res);
}

int main() {
	string str;
	while (getline(cin, str)) {
		cout << strToDouble(str) << endl;
	}
}