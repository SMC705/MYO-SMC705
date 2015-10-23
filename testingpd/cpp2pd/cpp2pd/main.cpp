

#include <stdio.h>
#include <iostream>
#include "PdBase.hpp"
#include "PdObject.h"

using namespace std;
using namespace pd;




int main() {
    
    float N = 100;
    
    pd.sendbang("myPlay1");
    //    pd.sendfloat("myPlay1", N);
    
    return 0;
    
}