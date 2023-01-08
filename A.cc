#include <iostream>
#include <vector>
using namespace std;
using ll = int64_t;

ostream& operator<<(ostream& o, const vector<ll>& A) {
  o << "[";
  for(ll x : A) {
    o << " " << x;
  }
  o << "]";
  return o;
}


struct Input {
  ll B,L,D;
  vector<ll> SCORE;
  vector<ll> DELAY;
  vector<ll> SHIP;
  vector<vector<ll>> BOOKS;

  static Input read() {
    Input in;
    cin >> in.B >> in.L >> in.D;
    in.SCORE = vector<ll>(in.B, 0);
    for(ll i=0; i<in.B; i++) {
      cin >> in.SCORE[i];
    }
    in.DELAY = vector<ll>(in.L, 0);
    in.SHIP = vector<ll>(in.L, 0);
    in.BOOKS = vector<vector<ll>>(in.L, vector<ll>{});
    for(ll i=0; i<in.L; i++) {
      ll k;
      cin >> k >> in.DELAY[i] >> in.SHIP[i];
      for(ll j=0; j<k; j++) {
        ll b;
        cin >> b;
        in.BOOKS[i].push_back(b);
      }
    }
    return in;
  }

  void show() {
    cerr << "B=" << B << " L=" << L << " D=" << D << endl;
    cerr << "S=" << SCORE << endl;
    for(ll i=0; i<L; i++) {
      cerr << " delay=" << DELAY[i] << " SHIP=" << SHIP[i] << " BOOKS=" << BOOKS[i] << endl;
    }
  }
};

struct Solution {
  vector<ll> S; // score of books; constant

  vector<ll> L; // permutation of 0..L-1 libraries
  vector<vector<ll>> B; // L permutations of BOOKS[i]

  // how many points do we get in D days
  ll score(ll D) {
    return D;
  }
};

int main() {
  Input in = Input::read();
  in.show();
}
