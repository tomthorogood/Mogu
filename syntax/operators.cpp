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
    for (; iter !=  op_map.end(); ++iter) {
        filestr << "MoguSyntax['"<< iter->first << "'] = (" 
            << iter->second << ", 'operator')" << std::endl;
    }
    filestr.close();

    filestr.open("syntax.h", std::ios::out | std::ios::app);
    
    filestr << "\t,OPER_OPPAREN\t\t\t\t=" << op_map['('] << std::endl;
    filestr << "\t,OPER_CLPAREN\t\t\t\t=" << op_map[')'] << std::endl;
    filestr << "\t,OPER_PLUS\t\t\t\t=" << op_map['+'] << std::endl;
    filestr << "\t,OPER_MINUS\t\t\t\t=" << op_map['-'] << std::endl;
    filestr << "\t,OPER_MULT\t\t\t\t=" << op_map['*'] << std::endl;
    filestr << "\t,OPER_DIV\t\t\t\t="<<op_map['/'] << std::endl;
    filestr.close();

    return 0;
}
