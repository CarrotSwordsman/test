#include <iostream>
#include <vector>
using namespace std;

int main()
{
    int m, t, u, f, d, i;
    cin >> m >> t >> u >> f >> d;
    vector<char> vc;
    vc.resize(t);
    for (i = 0; i < t; ++i)
    {
        cin >> vc[i];
    }
    int sum = 0;
    for (i = 0; i < t; ++i)
    {
        switch (vc[i])
        {
        case 'u':
        case 'd':
            sum += u + d;
            break;
        case 'f':
            sum += f * 2;
            break;
        default:
            break;
        }
        if (sum > m)
        {
            break;
        }
    }
    cout << i << endl;
}