/*
 * 文件：utils.cpp
 * 作者：Xiwei Sun
 * 日期：2018年9月1日
 * 描述：常用的函数
 */
#include "utils.h"

#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <fstream>

std::fstream g_log_file;

std::string get_time_us()
{
    char t[256];
    struct timeval tv;
    struct timezone tz;
    struct tm *p;
    gettimeofday(&tv, &tz);
    p = localtime(&tv.tv_sec);
    
    sprintf(t, "%04d%02d%02d-%02d:%02d:%02d.%06d", 1900 + p->tm_year,
            1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min,
            p->tm_sec, (int) tv.tv_usec);
    std::string str = t;
    return str;
}

std::string get_time_ms()
{
    char t[256];
    struct timeval tv;
    struct timezone tz;
    struct tm *p;
    gettimeofday(&tv, &tz);
    p = localtime(&tv.tv_sec);
    
    sprintf(t, "%04d%02d%02d-%02d%02d%02d.%03d", 1900 + p->tm_year,
            1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min,
            p->tm_sec, (int) tv.tv_usec/1000);
    std::string str = t;
    return str;
}

std::string get_time_sec()
{
    char t[256];
    struct timeval tv;
    struct timezone tz;
    struct tm *p;
    gettimeofday(&tv, &tz);
    p = localtime(&tv.tv_sec);
    
    sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + p->tm_year,
            1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min,
            p->tm_sec);
    std::string str = t;
    return str;
}

std::string get_time_date()
{
    char t[256];
    struct timeval tv;
    struct timezone tz;
    struct tm *p;
    gettimeofday(&tv, &tz);
    p = localtime(&tv.tv_sec);
    sprintf(t, "%04d%02d%02d", 1900 + p->tm_year, 1 + p->tm_mon,
            p->tm_mday);
    std::string str = t;
    return str;
}

//获取Unix时间戳
long get_unix_ts()
{
    time_t t;
    long ts;
    ts = time(&t);
    return ts;
}

void msg_print(std::string msg)
{
    std::cout << "[" << get_time_us() << "]\t" << msg << std::endl;
}

void log_init()
{
    //创建log文件夹
    create_dir("../log");
    std::string log_file_name = "../log/aipaysvr_" + get_time_date() + ".log";
    g_log_file.open(log_file_name.c_str(), std::ios::out | std::ios::app);
    if (g_log_file.good())
        msg_print("日志系统初始化完成。");
    else
        msg_print("日志系统初始化失败，程序将在无日志状态下运行。");
}

void log_output(std::string msg, bool std_print)
{
    if (std_print)
        msg_print(msg);
    if (g_log_file.good())
    {
        std::string log_msg = "[" + get_time_us() + "]" + msg;
        g_log_file << log_msg << std::endl;
    }
}

int create_dir(const char *s_path_name)
{
    char DirName[256];
    strcpy(DirName, s_path_name);
    int i, len = strlen(DirName);
    if (DirName[len - 1] != '/')
        strcat(DirName, "/");
    
    len = strlen(DirName);
    
    for (i = 1; i < len; i++)
    {
        if (DirName[i] == '/')
        {
            DirName[i] = 0;
            if (access(DirName, F_OK) != 0)
            {
                if (mkdir(DirName, 0755) == -1)
                {
                    perror("mkdir   error");
                    return -1;
                }
            }
            DirName[i] = '/';
        }
    }
    
    return 0;
}

/*********************进程互斥（用文件）***********************************/
bool is_have_instance()
{
    int file_id = open("./aipaysvr.tmp", O_RDWR | O_CREAT, 0640);
    if (file_id < 0)
    {
        return true;
    }
    
    if (flock(file_id, LOCK_EX | LOCK_NB) < 0)
    {
        return true;
    }
    
    return false;
}

std::vector<std::string> string_split(std::string str,std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str+=pattern;
    int size=str.size();
    
    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}
