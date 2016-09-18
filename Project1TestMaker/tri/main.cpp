#include <iostream>
#include <vector>

using namespace std;

int main() {
    int num_vertices;
    cin >> num_vertices;
   
    cout << 64 << endl;
    int count = 0;
    for (int i = 1; count < 32; i += 2) {
        cout << i << ' ';
        count++;
    }
    for (int i = num_vertices - 1; count < 64; i -=2) {
        cout << i << ' ';
        count++;
    }
    cout << endl; // houses

    cout << num_vertices << endl;

    cout << (num_vertices % 2 == 0 ? (num_vertices - 1) : (num_vertices - 2)) << endl;
    
    {
        int i;
        for (i = 1; i <= num_vertices - 2; i++) {
            cout << i << ' ';
            if (i % 2 == 1) {
                cout << 2 << ' ' << (i + 1) << ' ' << (i + 2) << endl;
            } else {
                cout << 1 << ' ' << (i + 2) << endl;
            }
        }

        if (i % 2 == 1) {
            cout << i << ' ' << 1 << ' ' << (i + 1) << endl;
        } else {
            // do nothing
        }
    }

    return 0;
}
