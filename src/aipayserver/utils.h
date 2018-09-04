/*
 * 文件：utils.h
 * 作者：Xiwei Sun
 * 日期：2018年9月1日
 * 描述：常用的函数
 */
#ifndef AI_PAY_UTILS
#define AI_PAY_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <vector>

//全局日志文件变量
extern std::fstream g_log_file;
//控制台消息输出：输出格式为时间+消息
void msg_print(std::string msg);
//日志输出初始化，生成日志文件，追加模式
void log_init();
//日志输出：输出格式为时间+日志消息
void log_output(std::string msg, bool std_print = true);
//获取时间，精确到微秒
std::string get_time_us();
//获取时间，精确到毫秒
std::string get_time_ms();
//获取时间，精确到妙
std::string get_time_sec();
//获取日期，精确到天
std::string get_time_date();
//获取Unix时间戳
long get_unix_ts();
//创建文件夹
int create_dir(const char *s_path_name);
//进程互斥
bool is_have_instance();
//string 字符串分割
std::vector<std::string> string_split(std::string str, std::string pattern);

#endif  //AI_PAY_UTILS
