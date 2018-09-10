/* 
 * File:   GSSLogger.h
 * Author: jameswong
 *
 * Created on June 26, 2012, 8:59 AM
 */

#ifndef GSSLOGGER_H
#define	GSSLOGGER_H

#include <string>
#include <set>

using namespace std;

class GSSLogger
{
private:
    FILE *log_file;
    GSSLogger();
    ~GSSLogger();
    
    int year;
    int month;
    int day;
    char *cur_time;
    time_t start_time;
    
    static GSSLogger *log_instance;
    
    set<string> log_set;
    pthread_mutex_t log_mutex;
    
    size_t elapsed_days;
    pthread_t age_thread;
    bool age_exit;
    static void* age(void *_arg);
    
public:
    static GSSLogger *get_instance();
    static void destroy();
    bool open_file(const string &_filename, bool _redirect_stdout, bool _redirect_stderr);
    void log(bool _timestamp, const char *_format, ...);
};

#endif	/* GSSLOGGER_H */
