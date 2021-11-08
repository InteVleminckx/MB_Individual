#include "PDA.h"


using namespace std;

int main() {
    CFG cfg("../../input-ll1.json");
    cfg.ll();
    return 0;
}
