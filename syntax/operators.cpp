#include <climits>
#include <map>
#include <fstream>
int main() {
    std::map <char,int> op_map;
    op_map['('] = INT_MAX - 5;
    op_map[')'] = INT_MAX - 4;
    op_map['+'] = INT_MAX - 3;
    op_map['-'] = INT_MAX - 2;
    op_map['*'] = INT_MAX - 1;
    op_map['/'] = INT_MAX;
    
    std::ofstream filestr;
    filestr.open("syntax.py", std::ios::out | std::ios::app);
    filestr << std::endl << "# Begin mathematic operator constants" << std::endl;
    auto iter = op_map.begin();
    filestr << "MoguOperators = {}" << std::endl;
    for (; iter !=  op_map.end(); ++iter) {
        filestr << "MoguOperators['"<< iter->first << "'] = " << iter->second << std::endl;
    }
    filestr.close();

    filestr.open("syntax.h", std::ios::out | std::ios::app);
    filestr << "\t,OUT_OF_RANGE_BEGIN\t\t\t\t="<< INT_MAX-6 << std::endl;
    filestr << "\t,OUT_OF_RANGE_END\t\t\t\t=" << INT_MAX-7 << std::endl;    
    filestr << "\t,OPER_OPPAREN\t\t\t\t=" << op_map['('] << std::endl;
    filestr << "\t,OPER_CLPAREN\t\t\t\t=" << op_map[')'] << std::endl;
    filestr << "\t,OPER_PLUS\t\t\t\t=" << op_map['+'] << std::endl;
    filestr << "\t,OPER_MINUS\t\t\t\t=" << op_map['-'] << std::endl;
    filestr << "\t,OPER_MULT\t\t\t\t=" << op_map['*'] << std::endl;
    filestr << "\t,OPER_DIV\t\t\t\t="<<op_map['/'] << std::endl;
    filestr << "\t,TOKEN_DELIM\t\t\t\t="<<INT_MAX-6<<std::endl;
    filestr.close();

    return 0;
}
