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

  static Solution start(const Input& I) {
    Solution S;
    S.L = vector<ll>(I.L, 0);
    for(ll i=0; i<S.L.size(); i++) {
      S.L[i] = i;
    }
    S.B = vector<vector<ll>>(I.L, vector<ll>{});
    for(ll i=0; i<I.L; i++) {
      S.B[i] = vector<ll>(I.BOOKS[i].size(), 0);
      for(ll j=0; j<S.B[i].size(); j++) {
        S.B[i][j] = j;
      }
    }
    return S;
  }

  // how many points do we get in D days
  ll score(const Input& I) {
    ll score = 0;
    vector<int> SEEN(I.B, false);
    ll cur_day = 0;
    for(ll i=0; i<I.L; i++) {
      ll library = L[i];
      cur_day += I.DELAY[library];
      ll days_left = I.D - cur_day;
      if(days_left <= 0) { continue; }
      ll books_left = days_left * I.SHIP[library];
      if(books_left > B[library].size()) {
        books_left = B[library].size();
      }
      for(ll j=0; j<books_left; j++) {
        ll book = I.BOOKS[library][B[library][j]];
        if(!SEEN[book]) {
          SEEN[book] = true;
          score += I.SCORE[book];
          cerr << " SCANNING book=" << book << " from library=" << library << " score=" << score << endl;
        }
      }
    }
    return score;
  }
};

int main() {
  Input in = Input::read();
  Solution S = Solution::start(in);
  /*S.L = vector<ll>{1,0};
  S.B[0] = vector<ll>{0,1,2,3,4};
  S.B[1] = vector<ll>{3,1,2,0};*/
  in.show();
  cout << S.score(in) << endl;
}
