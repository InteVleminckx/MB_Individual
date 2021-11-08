#include "PDA.h"


using namespace std;


int main() {
    CFG cfg("../../input-ll4.json");
    cfg.ll();
    return 0;
}

