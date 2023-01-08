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

int main() {
  ll B,L,D;
  cin >> B >> L >> D;
  vector<ll> S(B, 0);
  for(ll i=0; i<B; i++) {
    cin >> S[i];
  }
  vector<ll> DELAY(L, 0);
  vector<ll> SHIP(L, 0);
  vector<vector<ll>> BOOKS(L, vector<ll>{});
  for(ll i=0; i<L; i++) {
    ll k;
    cin >> k >> DELAY[i] >> SHIP[i];
    for(ll j=0; j<k; j++) {
      ll b;
      cin >> b;
      BOOKS[i].push_back(b);
    }
  }
  cerr << "B=" << B << " L=" << L << " D=" << D << endl;
  cerr << "S=" << S << endl;
  for(ll i=0; i<L; i++) {
    cerr << " delay=" << DELAY[i] << " SHIP=" << SHIP[i] << " BOOKS=" << BOOKS[i] << endl;
  }
}
