#include <iostream>
#include <vector>
#include <random>
#include <cassert>
using namespace std;
using ll = int64_t;
using ld = double;

ll r(ll lo, ll hi) {
  assert(lo < hi);
  static default_random_engine RNG;
  return uniform_int_distribution<ll>(lo,hi - 1)(RNG);
}

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

struct Move;
struct Swap_Lib;
struct Swap_Book;

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
    for(ll pos=0; pos<in.B; pos++) {
      cin >> in.SCORE[pos];
    }
    in.DELAY = vector<ll>(in.L, 0);
    in.SHIP = vector<ll>(in.L, 0);
    in.BOOKS = vector<vector<ll>>(in.L, vector<ll>{});
    for(ll pos=0; pos<in.L; pos++) {
      ll k;
      cin >> k >> in.DELAY[pos] >> in.SHIP[pos];
      for(ll j=0; j<k; j++) {
        ll b;
        cin >> b;
        in.BOOKS[pos].push_back(b);
      }
    }
    return in;
  }

  void show() {
    cerr << "B=" << B << " L=" << L << " D=" << D << endl;
    cerr << "S=" << SCORE << endl;
    for(ll pos=0; pos<L; pos++) {
      cerr << " delay=" << DELAY[pos] << " SHIP=" << SHIP[pos] << " BOOKS=" << BOOKS[pos] << endl;
    }
  }
};

struct Solution {
  vector<ll> L; // permutation of 0..L-1 libraries
  vector<vector<ll>> B; // L permutations of BOOKS[pos]
  // Derived
  vector<ll> scanned; // for each library, how many books it scanned
  vector<ll> reads; // how many times each book is read
  ll score;

  //Solution clone() {
  //  return {vector<ll>(L), vector<vector<ll>>(B), vector<ll>(reads), score};
  //}
  static Solution start(const Input& inp) {
    Solution S;
    S.L = vector<ll>(inp.L, 0);
    for(ll pos=0; pos<S.L.size(); pos++) {
      S.L[pos] = pos;
    }
    S.B = vector<vector<ll>>(inp.L, vector<ll>{});
    for(ll pos=0; pos<inp.L; pos++) {
      S.B[pos] = vector<ll>(inp.BOOKS[pos].size(), 0);
      for(ll j=0; j<S.B[pos].size(); j++) {
        S.B[pos][j] = j;
      }
    }
    S.compute_score(inp, false);
    return S;
  }

  // how many points do we get in D days
  void compute_score(const Input& inp, bool check) {
    ll new_score = 0;
    vector<ll> new_reads(inp.B, 0);
    vector<ll> new_scanned(inp.L, 0);
    ll cur_day = 0;
    for(ll pos=0; pos<inp.L; pos++) {
      ll lib = L[pos];
      cur_day += inp.DELAY[lib];
      ll days_left = inp.D - cur_day;
      if(days_left <= 0) { continue; }
      ll to_scan = min(days_left * inp.SHIP[lib],
          static_cast<ll>(B[lib].size()));

      for(ll j=0; j<to_scan; j++) {
        ll book = inp.BOOKS[lib][B[lib][j]];
        if(new_reads[book] == 0) {
          new_score += inp.SCORE[book];
          //cerr << " SCANNING book=" << book << " from library=" << lib << " score=" << score << endl;
        }
        new_scanned[lib] = to_scan;
        new_reads[book] += 1;
      }
    }
    if (check) {
      for (int lib=0; lib < inp.L; lib++){
        assert(scanned[lib] == new_scanned[lib]);
      }
      for (int book=0; book < inp.B; book++){
        assert(reads[book] == new_reads[book]);
      }
      assert(score == new_score);
    }
    scanned = new_scanned;
    reads = new_reads;
    score = new_score;
  }

  Move* rand_move() {
    ll lib = r(0, L.size());
    if (r(0, 2) == 0) {
      return new Swap_Lib{lib};
    }
    if (scanned[lib] <= 0 || scanned[lib] >= B[lib].size()) {
      return rand_move();
    }
    return new Swap_Book{lib, 
      r(0, scanned[lib]),
      r(scanned[lib], B[lib].size())}
  }
};

struct Move {
  virtual void apply(Solution& sol, const Input& inp) = 0;
  virtual void undo(Solution& sol, const Input& inp) = 0;
};

struct Swap_Lib : Move {
  ll pos;
  void apply(Solution& sol, const Input& inp) {
    // Swap dem libs
    ll temp = sol.L[pos];
    sol.L[pos] = sol.L[pos + 1];
    sol.L[pos + 1] = temp;

    // how much change sir
    sol.compute_score(inp, false); // TODO
  }
  void undo(Solution& sol, const Input& inp) {
    apply(sol, inp);
  }
};

struct Swap_Book : Move {
  ll lib;
  ll pos1;
  ll pos2;
  void apply(Solution& sol, const Input& inp) {
    vector<ll>& books = sol.B[lib];
    ll book1 = books[pos1];
    ll book2 = books[pos2];
    books[pos1] = book2;
    books[pos2] = book1;

    sol.reads[book1]--;
    sol.reads[book2]++;
    if (sol.reads[book1] == 0) {
      sol.score -= inp.SCORE[book1];
    }
    if (sol.reads[book2] == 1) {
      sol.score += inp.SCORE[book1];
    }
    sol.compute_score(inp, true);
  }
  void undo(Solution& sol, const Input& inp) {
    apply(sol, inp);
  }
};

Solution simulated_annealing(const Input& inp) {
  ld t_start = 0;
  for(ll pos=0; pos<inp.B; pos++) {
    t_start += inp.SCORE[pos];
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

  Solution S = Solution::start(inp);
  ll kmax = 1000;
  for(ll k=0; k<kmax; k++) {
    ll t = temperature(1.0 - static_cast<ld>(k+1)/kmax);

    Move* M = S.rand_move();
    ll old_score = S.score;
    M->apply(S, inp);
    ll new_score = S.score;
    if(!(U(0,1) < P(old_score, new_score, t))) {
      M->undo(S, inp);
    }
  }
  return S;
}

int main() {
  Input in = Input::read();
  Solution S = simulated_annealing(in);
  //in.show();
  cout << S.score << endl;
}
