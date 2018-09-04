#include "tcp_server.h"


//获取Unix时间戳
long get_utc()
{
    time_t t;
    long ts;
    ts = time(&t);
    return ts;
}
