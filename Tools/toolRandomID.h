#ifndef TOOLRANDOMID_H
#define TOOLRANDOMID_H
#include <QDateTime>
#include <QTime>
#include <chrono>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

using namespace std::chrono;
class ToolRandomID{
protected:
    /** 最大值 */
      static const long long MAX = 122009690880L; // 36*35*34*33*32*31*30*29
      /** 乘法用的素数 */
      static const long P = 982451653L;
      /** 加法用的素数 */
      static const long Q = 9007;
      /** 编码长度 为10的整数次幂*/
      static const long LEN = 1000000;
      /** 采用36进制 */
      static const int RADIX = 36;
public:
    static long CreatID(){
        long number = GetID();
        if (number <= 0 || number > MAX){
          return 0;
        }
        long x = ((number * P + Q) % MAX);
        x = x >0? x : x *-1;
        x %= LEN;
//        if(x<LEN/10){
//            x = CreatID();
//        }
        return x;
    }
    /**
     * @brief 毫秒级时间戳
     * @return
     */
    static long GetID(){
        auto c_time= duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        return c_time.count() % (LEN * 100);
    }
    static std::string GetUID(){
        boost::uuids::uuid a_uuid = boost::uuids::random_generator()(); // 这里是两个() ，因为这里是调用的 () 的运算符重载
        return  boost::uuids::to_string(a_uuid);
    }
};

#endif // TOOLRANDOMID_H
