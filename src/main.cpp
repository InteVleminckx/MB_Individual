#include "PDA.h"
#include <math.h>

using namespace std;




//cfg
//int main() {
//
//    CFG cfg("../../input-ll3.json");
//    cfg.ll();
//    return 0;
//}

//pda
int main() {
    PDA pda("../../input-pda2cfg2.json");
    pda.toCFG().print();
}

