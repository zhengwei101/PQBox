/**
 * 目的: 日期工具
 * 依赖: 对date库的封装，简化使用
 * 使用示例：
 */

#pragma once

#include <iomanip> // put_time

#include <date/date.h>

namespace MBase {
namespace {
inline std::tm localtime_xp(std::time_t timer)
{
    std::tm bt{};
#if defined(__unix__)
    localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
    localtime_s(&bt, &timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(&timer);
#endif
    return bt;
}
} // namespace
class MDate
{
public:
    static std::string GetDateString()
    {
        auto now = std::chrono::system_clock::now();
        auto today = date::floor<date::days>(now);
        auto ymd = date::year_month_day{today};

        std::stringstream ss;
        ss << ymd;

        std::string date = ss.str();
        ss.str(""); //清空缓冲区

        return date;
    }

    static std::string GetCurrentDate()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);

        std::tm bt = localtime_xp(in_time_t);
        std::stringstream ss;
        ss << std::put_time(&bt, "%F %T");
        return ss.str();
    }

    static std::string GetTimestamp(const std::string dateTime)
    {
        //时间格式 2022-3-8 16:00:00
        std::istringstream in{dateTime};
        date::sys_seconds tp;
        in >> parse("%F %T", tp);

        auto real_tp = tp + std::chrono::hours{-8};

        std::stringstream ss;
        ss << real_tp.time_since_epoch().count();
        std::string timestamp = ss.str();
        ss.str(""); //清空缓冲区

        return timestamp;
    }
};
} // namespace MBase