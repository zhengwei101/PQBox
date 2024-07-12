#include <iostream>
#include <windows.h>
#include <stdio.h>

using std::cout;
using std::endl;

#include "MBase/Md5.hpp"
#include "MBase/DllLoader.hpp"
#include "MBase/Singleton.hpp"
#include "MBase/Noncopyable.hpp"
#include "MBase/ThreadPool.hpp"
#include "MBase/Url.hpp"
#include "MBase/File.hpp"
#include "MBase/String.hpp"
#include "MBase/Date.hpp"
#include "MBase/Http.hpp"
#include "MBase/Logger.hpp"
#include "MBase/Zip.hpp"
#include "MBase/Xml.hpp"


#include <strsafe.h>

void ErrorExit(LPCTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf = nullptr;

    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0,
        NULL);

    // Display the error message and exit the process

    LPVOID lpDisplayBuf = (LPVOID)LocalAlloc(
        LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

    if (lpDisplayBuf)
    {
        StringCchPrintf((LPTSTR)lpDisplayBuf,
                        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                        TEXT("%s failed with error %d: %s"),
                        lpszFunction,
                        dw,
                        (LPTSTR)lpMsgBuf);

        MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
    }

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

    //ExitProcess(dw);
}

void testMd5()
{
    const std::string md5_value = MBase::Md5::BufferDigest("");
    if (md5_value != "d41d8cd98f00b204e9800998ecf8427e")
    {
        cout << "Error!";
    }
}

void testDll()
{
    class Advapi32
    {
        MBase::DllLoader dll_{L"Advapi32.dll"};
    public:
        decltype(GetUserNameW)* getUserName = dll_["GetUserNameW"];
    };

#define INFO_BUFFER_SIZE 256
    TCHAR infoBuf[INFO_BUFFER_SIZE] = {};
    DWORD bufCharCount = INFO_BUFFER_SIZE;

    Advapi32 adv;
    if (adv.getUserName && adv.getUserName(infoBuf, &bufCharCount))
    {
        wprintf(L"Username:  %s\n", infoBuf);
    }
    else
    {
        ErrorExit(L"getUserName");
    }
}

void testSingleton()
{
    cout << "Enter Test..." << endl;
    class Test final : public MBase::Singleton<Test>
    {
    public:
        Test() { cout << "Test Ctor" << endl; }
        ~Test() { cout << "Test Dtor" << endl; }

        void use() const { cout << " call use" << endl; }
    };

    auto const& t = Test::GetInstance();
    t.use();

    {
        Test::GetInstance().use();
    }

    cout << "Exit Test..." << endl;
}

void testNoncopyable()
{
    struct A : private MBase::NonCopyable
    {
    };
    struct B : private MBase::NonCopyable
    {
    };
    struct C
        : public A
        , public B
    {
    };
    std::cout << sizeof(C) << "\n";
}

void testUrl()
{
    std::string url1 = "https://www.deepl.com/zh/"
                       "app-installed?windows_app_version=24.4.3.12310";
    MBase::Url url;
    url.parse(url1);
    cout << url << endl;
    cout << url.getParamValue("windows_app_version") << endl;
}

void testFile()
{
    using namespace MBase;

    std::string errMsg;
    MFile::CreateDir("D:/zWork/zhengwei111/shurong.txt", errMsg);

}

void testDate()
{
    MBase::MDate d;
    cout  << "date: " << d.GetDateString() 
        << ", current: " << d.GetCurrentDate()
        << ", timestamp: " << d.GetTimestamp("2022-3-8 16:00:00") << endl;

}

void testDownload()
{
    MBase::DownloadParam param;
    param.url = "https://forspeed.onlinedown.net/alading/WajWpapersetup_huajun_1.1.2.0.exe";
    param.save_dir = "D:/Temp/Download/";
    std::string errMsg;
    MBase::MHttp::DownLoadFile(param, errMsg);
}

void testZip()
{
    std::vector<std::string> vMediaFiles;
    MBase::MZip zip("D:/zWork/Temp/test.docx");

    std::string saveDir{"D:/zWork/Temp/test/"};
    zip.ExtractMediaFiles(saveDir, vMediaFiles);

    zip.CreateZipFile("aa.zip", saveDir);
}

void testXml()
{
    MBase::MXml::Text2Html("abc");
}

class ThreadDemo
{
public:
    int Add(int a, int b) { return a + b; }
    int Multi(int a, int b) { return a * b;  }

    void threadTask()
    {
        MBase::ThreadPool pool;
        std::future<int> f = pool.enqueue(std::mem_fn(&ThreadDemo::Multi), this, 3, 5);
        while (f.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
            continue;
        }
        cout << "3*5=" << f.get() << endl;
    }
};

int main()
{   
    //MBase::Logger::enableFileLogging("mbaseTester.log");

    LOGGER_INFO("main", "main begin");
    //testXml();

    MBase::ThreadPool pool;
    for (int i = 0; i < 10; i++) {
        pool.enqueue([i]() {
            cout << std::format("this value is: {}\n", i);
        });
    }
    Sleep(100);

    ThreadDemo demo;
    std::future<int> f = pool.enqueue(std::bind(&ThreadDemo::Add, &demo, 3, 4));
    
    std::future_status status;
    do {
        status = f.wait_for(std::chrono::seconds(1));
        if (status == std::future_status::deferred) {
            std::cout << "deferred\n";
        }
        else if (status == std::future_status::timeout) {
            std::cout << "timeout\n";
        }
        else if (status == std::future_status::ready) {
            cout << "3+4=" << f.get() << endl; 
        }
    } while (status != std::future_status::ready);
    

    demo.threadTask();
    LOGGER_INFO("main", "main end");
}