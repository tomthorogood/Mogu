#include "syntax.h"
#include <iostream>
#include "../src/Types/NodeValue.h"
int main()
{
    int i = (int) MoguSyntax::widget;
    std::string s = (std::string) MoguSyntax::widget;
    std::cout << "Widget is " << i << std::endl;
    std::cout << "Widget is " << s << std::endl;
    std::cout << "Widget is " << MoguSyntax::widget.human << std::endl;
    std::cout << "Widget is " << (int) MoguSyntax::get(i) << std::endl;
    std::cout << "Widget is " << (std::string) MoguSyntax::get(s) << std::endl;
    std::cout << "Widget is " << (int) MoguSyntax::get("widget", MoguSyntax::HUMAN) << std::endl;
    NodeValue nv(i);
    nv.setString(MoguSyntax::get(i).human);
    std::cout << (std::string) nv << std::endl;
    return 0;
}
