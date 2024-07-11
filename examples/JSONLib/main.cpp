// JSONLib.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <iomanip>

#include "nlohmann/json.hpp"

using json = nlohmann::json; //命名空间， nlohmann是作者的简称

using namespace std;

int main()
{
    json myjson; //可以把json类型理解为一种数据结构

    //加入普通的member, key/value对
    myjson["pi"] = 3.14;               //数字
    myjson["pass"] = true;             //bool
    myjson["region"] = string("Asia"); //字符串，需要显示使用string
    myjson["nope"] = nullptr;          //空指针

    //复杂一点的情况
    myjson["vector"] = {1, 2, 3};                                //数组
    myjson["1st"]["2nd"] = string("object inside");              //嵌套object: 写法1
    myjson["1st"]["3rd"] = 121;
    myjson["more_obj"] = {{"obj1", "hello"}, {"obj2", "world"}}; //嵌套object: 写法2
    cout << myjson << endl;                                      //非格式化输出json
    cout << setw(4) << myjson << endl; //格式化输出json，重载了setw

    return 0;
}