#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
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

struct Move;
struct Solution {
  vector<ll> L; // permutation of 0..L-1 libraries
  vector<vector<ll>> B; // L permutations of BOOKS[pos]
  // Derived
  vector<ll> scanned; // for each library, how many books it scanned
  vector<ll> reads; // how many times each book is read
  ll score;

  Solution clone() {
    return {vector<ll>(L), vector<vector<ll>>(B), vector<ll>(scanned), vector<ll>(reads), score};
  }
  static Solution start(const Input& inp) {
    Solution S;
    S.L = vector<ll>(inp.L, 0);
    for(ll pos=0; pos<S.L.size(); pos++) {
      S.L[pos] = pos;
    }
    sort(S.L.begin(), S.L.end(), [&inp](ll i, ll j) {
        return inp.DELAY[i] < inp.DELAY[j];
    });

    S.B = vector<vector<ll>>(inp.L, vector<ll>{});
    for(ll pos=0; pos<inp.L; pos++) {
      S.B[pos] = vector<ll>(inp.BOOKS[pos].size(), 0);
      for(ll j=0; j<S.B[pos].size(); j++) {
        S.B[pos][j] = inp.BOOKS[pos][j];
      }
      sort(S.B[pos].begin(), S.B[pos].end(), [&inp](ll i, ll j) {
          return inp.SCORE[i] > inp.SCORE[j];
      });
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
      ll to_scan = days_left * inp.SHIP[lib];
      new_scanned[lib] = to_scan;

      for(ll j=0; j<min(to_scan, static_cast<ll>(B[lib].size())); j++) {
        ll book = B[lib][j];
        if(new_reads[book] == 0) {
          new_score += inp.SCORE[book];
          //cerr << " SCANNING book=" << book << " from library=" << lib << " score=" << score << endl;
        }
        new_reads[book] += 1;
      }
    }
    if (check) {
      for (int lib=0; lib < inp.L; lib++){
        assert(scanned[lib] == new_scanned[lib]);
      }
      for (int book=0; book < inp.B; book++){
        if(reads[book] != new_reads[book]) {
          cerr << " book=" << book << " reads[book]=" << reads[book] << " new_reads[book]=" << new_reads[book] << endl;
        }
        assert(reads[book] == new_reads[book]);
      }
      if(score != new_score) {
        cerr << " score=" << score << " new_score=" << new_score << endl;
      }
      assert(score == new_score);
    }
    scanned = new_scanned;
    reads = new_reads;
    score = new_score;
  }

  Move* rand_move();

  void show() {
    cerr << " L=" << L << endl;
    for(ll i=0; i<L.size(); i++) {
      cerr << " B[i]=" << B[i] << endl;
    }
    cerr << " score=" << score << endl;
    cerr << " scanned=" << scanned << endl;
    cerr << " reads=" << reads << endl;
  }
};

struct Move {
  virtual void apply(Solution& sol, const Input& inp) = 0;
  virtual void undo(Solution& sol, const Input& inp) = 0;
  virtual string show() = 0;
  virtual ~Move() = default;
};

struct Swap_Lib : Move {
  ll pos;
  Swap_Lib(ll pos_) : pos(pos_) {}
  void apply(Solution& sol, const Input& inp) {
    // Swap dem libs
    ll lib1 = sol.L[pos];
    ll lib2 = sol.L[pos+1];
    sol.L[pos] = lib2;
    sol.L[pos + 1] = lib1;

    // update derived

    ll old_lib1 = sol.scanned[lib1];
    sol.scanned[lib1] -= inp.DELAY[lib2] * inp.SHIP[lib1];
    ll new_lib1 = sol.scanned[lib1];
    for(ll i = max(new_lib1, ll(0)); i < min(old_lib1, static_cast<ll>(sol.B[lib1].size())); i++) {
      ll book = sol.B[lib1][i];
      sol.reads[book]--;
      if(sol.reads[book]==0) {
        sol.score -= inp.SCORE[book];
      }
    }

    ll old_lib2 = sol.scanned[lib2];
    sol.scanned[lib2] += inp.DELAY[lib1] * inp.SHIP[lib2];
    ll new_lib2 = sol.scanned[lib2];
    for(ll i = max(old_lib2, ll(0)); i<min(new_lib2, static_cast<ll>(sol.B[lib2].size())); i++) {
      ll book = sol.B[lib2][i];
      sol.reads[book]++;
      if(sol.reads[book]==1) {
        sol.score += inp.SCORE[book];
      }
    }
    // check that the delta update is right
    //sol.compute_score(inp, true);
  }
  void undo(Solution& sol, const Input& inp) {
    apply(sol, inp);
  }
  string show() {
    ostringstream o;
    o << "SwapLib(" << pos << ")";
    return o.str();
  }
};

struct Swap_Book : Move {
  ll lib;
  ll pos1;
  ll pos2;
  Swap_Book(ll lib_, ll pos1_, ll pos2_) : lib(lib_), pos1(pos1_), pos2(pos2_) {}
  void apply(Solution& sol, const Input& inp) {
    assert(pos1 < sol.scanned[lib]);
    assert(pos2 >= sol.scanned[lib]);

    // update actual solution
    vector<ll>& books = sol.B[lib];
    ll book1 = books[pos1];
    ll book2 = books[pos2];
    books[pos1] = book2;
    books[pos2] = book1;

    // update derived fields
    sol.reads[book1]--;
    sol.reads[book2]++;
    if (sol.reads[book1] == 0) {
      sol.score -= inp.SCORE[book1];
    }
    if (sol.reads[book2] == 1) {
      sol.score += inp.SCORE[book2];
    }

    // check that the delta update is right
    //sol.compute_score(inp, true);
  }
  void undo(Solution& sol, const Input& inp) {
    apply(sol, inp);
  }
  string show() {
    ostringstream o;
    o << "SwapBook(lib=" << lib << " pos1=" << pos1 << " pos2=" << pos2 << ")";
    return o.str();
  }
};

Move* Solution::rand_move() {
  if (r(0, 2) == 0) {
    ll lib = r(0, L.size()-1);
    return new Swap_Lib{lib};
  } else {
    // pick a move within this library that changes the score
    ll lib = r(0, L.size());
    // no valid moves for this library; either all books are read or none are
    if (scanned[lib] <= 0 || scanned[lib] >= B[lib].size()) {
      return rand_move();
    }
    ll book1 = r(0, scanned[lib]);
    ll book2 = r(scanned[lib], B[lib].size());
    return new Swap_Book{lib, book1, book2};
  }
}

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
    return exp((s_new - s_old) / t);
  };

  Solution S = Solution::start(inp);
  ll best_cnt = 0;
  Solution best = S.clone();

  ll kmax = 10000000;
  for(ll k=0; k<kmax; k++) {
    ld t = temperature(static_cast<ld>(k+1)/kmax);

    Move* M = S.rand_move();
    ll old_score = S.score;
    M->apply(S, inp);
    ll new_score = S.score;
    if(new_score > best.score) {
      best_cnt++;
      // TODO: this line is slow
      best = S;
    }

    ld p_accept = P(old_score, new_score, t);
    bool accept = (U(0,1) < p_accept);
    if(!accept) {
      M->undo(S, inp);
    }
    if(k%1000000==0) {
      cerr << "k=" << k << " t=" << t << " S.score=" << S.score << " old_score=" << old_score << " new_score=" << new_score << " p_accept=" << p_accept << " accept=" << accept << endl;
    }
    delete M;
  }
  cerr << " best_updated=" << best_cnt << endl;
  return best;
}

int main() {
  Input in = Input::read();
  in.show();
  Solution S = simulated_annealing(in);
  cout << S.score << endl;
}
