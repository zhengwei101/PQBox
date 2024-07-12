// JSONLib.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <iomanip>
#include <format>

#include "nlohmann/json.hpp"
#include "nlohmann/fifo_map.hpp"

using json = nlohmann::json; //命名空间， nlohmann是作者的简称

using std::cout;
using std::endl;
using std::string;
using std::vector;

static void Example1()
{
    json myJson; //可以把json类型理解为一种数据结构

    //加入普通的member, key/value对
    myJson["pi"] = 3.14;               //数字
    myJson["pass"] = true;             //bool
    myJson["region"] = string("Asia"); //字符串，需要显示使用string
    myJson["nope"] = nullptr;          //空指针

    //复杂一点的情况
    myJson["vector"] = {1, 2, 3};                               //数组
    myJson["1st"]["2nd"] = string("object inside");             //嵌套object: 写法1
    myJson["1st"]["3rd"] = 121;
    myJson["moreobj"] = {{"obj1", "hello"}, {"obj2", "world"}}; //嵌套object: 写法2

    cout << myJson << endl;                                     //非格式化输出json
    cout << std::setw(4) << myJson << endl; //格式化输出json，重载了setw

    //获取数据
    //方式一, find迭代器
    auto objJson = myJson.find("region");
    if (objJson != myJson.end()) {
        string s = objJson.value(); //必须显示指明接收value值的类型
    }

    try {
        //方式二, at
        bool pass = myJson.at("pass"); //如果不存在会抛 out_of_range 异常
        //方式三, []
        vector<int> v = myJson["vector"]; //如果不存在会抛 type_error 异常
    }
    catch (nlohmann::detail::exception& e) {
        string error = std::format("json throw an error:{}, Try to Fix it!", e.what());
        cout << error << endl;
    }
    //获取嵌套object的方式类似
    int n = myJson["1st"]["3rd"];
    double d = myJson["1st"]["3rd"].get<double>(); //可以显示指定value的类型
}

static void Example2()
{
    json myJson; //万能数据结构

    //1.可以装载n为vector
    vector<vector<int>> form;
    form.push_back(vector<int>({1, 2, 3, 4, 5}));
    form.push_back(vector<int>({6, 7, 8, 9, 10}));
    myJson["2D"] = form;
    cout << std::setw(4) << myJson << endl; //格式化输出json，重载了setw
    auto formR = myJson["2D"].get<vector<vector<int>>>();
    cout << typeid(formR).name() << endl;

    //2.可以装载key为string的关联容器
    std::map<string, double> wageTable;
    wageTable["Bob"] = 12.5;
    wageTable["Tom"] = 8.9;
    wageTable["Mary"] = 15.8;
    myJson["WageTable"] = wageTable;

    std::map<string, double> Rtable = myJson["WageTable"];

    cout << myJson.dump();
}

namespace magic {
struct Person
{
    string Name;
    int Age = 0;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Person, Name, Age)
};

struct DownParam
{
    string Url;
    string SaveDir;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DownParam, Url, SaveDir)
};
} // namespace magic

static void Example3()
{
    //3.支持自定义结构体
    magic::Person p = {"Peter", 18};
    magic::DownParam d = {"http://a/b.txt", "/user/download"};

    json j = p;
    cout << j << endl;

    j.clear();

    j = d;
    cout << j << endl;

    json myJson;
    myJson["Person"] = p;
    myJson["DownParam"] = d;

    auto re = myJson["Person"].get<magic::Person>();
    cout << re.Name << ", " << re.Age << endl;

    auto dw = myJson["DownParam"].get<magic::DownParam>();
    cout << dw.Url << ", " << dw.SaveDir << endl;
}

// 支持自定义类
class ComplexOne
{
private:
    vector<string> vStr;
    std::unordered_map<string, magic::Person> hashNameIndex;

public:
    vector<string> GetVStr() { return vStr; }

    void AddStr(const string& str) { vStr.emplace_back(str); }

    void AddPair(string key, magic::Person p) { hashNameIndex[key] = p; }

    friend void to_json(json& j, const ComplexOne& cp);
    friend void from_json(const json& j, ComplexOne& cp);
};

void to_json(json& j, const ComplexOne& cp)
{
    j = json{{"vstr", cp.vStr}, {"hash_name_index", cp.hashNameIndex}};
}

void from_json(const json& j, ComplexOne& cp)
{
    j.at("vstr").get_to(cp.vStr);
    j.at("hash_name_index").get_to(cp.hashNameIndex);
}

static void Example4()
{
    //4. 支持自定义类
    magic::Person p = {"Peter", 18};

    ComplexOne one;
    one.AddStr("apple");
    one.AddStr("banana");
    one.AddPair("001", p);

    json j = one;
    cout
        << j
        << endl; //输出：{"hash_name_index":{"001":{"Age":18,"Name":"Peter"}},"vstr":["apple","banana"]}

    ComplexOne anotherOne = j;

    for (auto& v : anotherOne.GetVStr()) {
        cout << v << endl;
    }
}

// A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
template <class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
using my_json = nlohmann::basic_json<my_workaround_fifo_map>;

static void Example5()
{
    //5.写JSON时保留插入顺序
    my_json j;
    j["f"] = 5;
    j["a"] = 2;
    my_json j2 = {{"pi", 3.141},
                  {"happy", true},
                  {"name", "Niels"},
                  {"nothing", nullptr},
                  {"answer", {{"everything", 42}}},
                  {"list", {1, 0, 2}},
                  {"object", {{"currency", "USD"}, {"value", 42.99}}}};

    std::cout << j.dump(4) << std::endl;
    std::cout << j2.dump(4) << std::endl;
}

int main()
{
    //Example1();

    //Example2();

    //Example3();

    //Example4();

    Example5();

    return 0;
}