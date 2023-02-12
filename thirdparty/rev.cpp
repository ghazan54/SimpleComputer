#include <bits/stdc++.h>

using namespace std;

int main(void)
{
    vector<vector<int>> v = {
        { 0, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 0 }
    };

    for (auto& i : v) {
        for (auto& j : i) {
            cout << j;
        }
        cout << '\n';
    }
    cout << "\n\n";
    for (auto& i : v) {
        reverse(i.begin(), i.end());
    }
    for (int i = 3; i >= 0; --i) {
        for (auto& j : v[i]) {
            cout << j;
        }
    }
    cout << '\n';
    for (int i = 7; i >= 4; --i) {
        for (auto& j : v[i]) {
            cout << j;
        }
    }
    cout << "\n";
    return 0;
}
