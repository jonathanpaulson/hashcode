#include <iostream>
#include <vector>
#include <random>
using namespace std;
using ll = int64_t;
using ld = double;

ld U(ld lo, ld hi) {
  static default_random_engine RNG;
  return uniform_real_distribution<ld>(lo,hi)(RNG);
}

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

Solution simulated_annealing(const Input& I) {
  ld t_start = 0;
  for(ll i=0; i<I.B; i++) {
    t_start += I.SCORE[i];
  }
  //temp schedule: t = t_start * (t_final / t_start) ^ time_passed, where time_passed is in 0..1
  auto temperature = [&t_start](ld f) {
    ld t_final = 1.0;
    return t_start * pow(t_final / t_start, f);
  };
  //acceptance (when min is better): RNG() < exp((cur_result - new_result) / t), where RNG() returns 0..1 uniformly
  auto P = [](ll s_old, ll s_new, ld t) {
    return (s_old - s_new) / t;
  };

  Solution S = Solution::start(I);
  ll kmax = 1000;
  for(ll k=0; k<kmax; k++) {
    ll t = temperature(1.0 - static_cast<ld>(k+1)/kmax);

    Solution S2 = S.adjust();
    if(U(0,1) < P(S.score(I), S2.score(I), t)) {
      S = S2;
    }
  }
  return S;
}

int main() {
  Input in = Input::read();
  Solution S = simulated_annealing(in);
  //in.show();
  cout << S.score(in) << endl;
}
