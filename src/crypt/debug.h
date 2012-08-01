/*debug.h*/

#ifndef DEBUG_H_
#define DEBUG_H_

#ifndef NO_DEBUG
#include <iostream>
using std::cout;
using std::endl;

#include <assert.h>

#define DEBUG_PLZ(X,Y) (cout << __FILE__ << " :" << __LINE__ << \
		" :" << (X) << "{" << (Y) << "}" << endl)
#endif

#endif /* DEBUG_H_ */
