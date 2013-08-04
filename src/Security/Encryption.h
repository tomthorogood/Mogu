#ifndef SECURITY_H_
#define SECURITY_H_

#ifndef BF_SIZE_
#define BF_SIZE_ 8
#endif

#include <string>

namespace Security {

    std::string encrypt(const std::string&);
    std::string decrypt(const std::string&);

    std::string processPacket(const std::string&, int method);
    std::string processString(const std::string&, int method);
    std::string trimPadding (const std::string&);

}

#endif
