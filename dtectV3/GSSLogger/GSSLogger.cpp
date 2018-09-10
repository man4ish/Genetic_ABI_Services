/* 
 * File:   GSSLogger.cpp
 * Author: Manish
 * 
 * Created on June 26, 2012, 8:59 AM
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <signal.h>
#include "GSSLogger.h"

extern char *__progname;

inline size_t rowParser(char *_source, const char _delimiter, vector<char*> &_fields_vec)
{
    _fields_vec.clear();
    _fields_vec.push_back(_source);
    
    while (*_source != '\0')
    {
        if (*_source == _delimiter)
        {
            *_source = '\0';
            _source++;
            _fields_vec.push_back(_source);
            continue;
        }
        _source++;
    }
    return _fields_vec.size();
}

GSSLogger *GSSLogger::log_instance = NULL;

GSSLogger::GSSLogger()
{
    this->log_file = NULL;
    this->year = 0;
    this->month = 0;
    this->day = 0;
    this->cur_time = NULL;
    int ret = pthread_mutex_init(&this->log_mutex, NULL);
    assert(ret == 0);
    
    ret = pthread_create(&this->age_thread, NULL, age, this);
    assert(ret == 0);
}

GSSLogger::~GSSLogger()
{
    if (this->log_file != NULL)
        fclose(this->log_file);
    this->log_file = NULL;
}

void* GSSLogger::age(void* _arg)
{
    GSSLogger *instance = reinterpret_cast<GSSLogger*>(_arg);
    assert(instance != NULL);
    
    time_t curr_time;
    time(&instance->start_time);
    instance->age_exit = false;
    instance->elapsed_days = 0;
    sleep(1);
    while (instance->age_exit == false)
    {
        time(&curr_time);
        if ((curr_time - instance->start_time) % 86400 == 0)
        {
            instance->elapsed_days++;
            instance->log(true, "... has been running for %lu %s.\n",
                    instance->elapsed_days,
                    instance->elapsed_days > 1 ? "days" : "day");
        }
        sleep(1);
    }
}

GSSLogger* GSSLogger::get_instance()
{
    if (log_instance == NULL)
        log_instance = new GSSLogger();
    return log_instance;
}

void GSSLogger::destroy()
{
    if (log_instance != NULL)
    {
        log_instance->age_exit = true;
        
        // wait for thread to exit before deleting the instance
        int ret = pthread_join(log_instance->age_thread, NULL);
        assert(ret == 0);
        delete log_instance;
    }
    log_instance = NULL;
}

bool GSSLogger::open_file(const string& _filename, bool _redirect_stdout, bool _redirect_stderr)
{
    pthread_mutex_lock(&this->log_mutex);
    if (this->log_file == NULL)
    {
        if (_filename.empty())
            fprintf(stderr, "\nERROR: Invalid log filename.\n");
        else
            this->log_file = fopen(_filename.c_str(), "a");

        if (this->log_file == NULL)
            fprintf(stderr, "\nERROR: %s (%s)\n", _filename.c_str(), strerror(errno));
        this->log_set.clear();
        
        // redirect stderr and stdout to logfile
        if (_redirect_stdout)
        {
            FILE *ret = freopen(_filename.c_str(), "a", stdout);
            assert(ret != NULL);
        }
        
        if (_redirect_stderr)
        {
            FILE *ret = freopen(_filename.c_str(), "a", stderr);
            assert(ret != NULL);
        }
    }
    
    if (this->log_file == NULL)
    {
        pthread_mutex_unlock(&this->log_mutex);
        return false;
    }
    pthread_mutex_unlock(&this->log_mutex);
    return true;
}

void GSSLogger::log(bool _timestamp, const char *_format, ...)
{
    pthread_mutex_lock(&this->log_mutex);
    if (this->log_file == NULL)
    {
        pthread_mutex_unlock(&this->log_mutex);
        return;
    }
    
    if (_format == NULL)
    {
        fprintf(stderr, "\nERROR: No or invalid format string provided.\n");
        pthread_mutex_unlock(&this->log_mutex);
        return;
    }
    va_list arg_list;
    va_start(arg_list, _format);
    
    time_t cur_time;
    struct tm *t_info;

    time(&cur_time);
    t_info = localtime(&cur_time);

    char current_time[64] = {0};
    sprintf(current_time, "%s", asctime(t_info));
    size_t len = strlen(current_time) - 1;
    while (len > 0)
    {
        if (current_time[len] == '\n')
            current_time[len] = '\0';
        --len;
    }
    vector<char*> time_fields;
    size_t f_cnt = rowParser(current_time, ' ', time_fields);
    assert((f_cnt == 5) || (f_cnt == 6));
    
    this->year = t_info->tm_year + 1900;
    this->month = t_info->tm_mon + 1;
    
    if (f_cnt == 5)
    {
        this->day = atoi(time_fields[2]);
        this->cur_time = time_fields[3];
    }
    else
    {
        this->day = atoi(time_fields[3]);
        this->cur_time = time_fields[4];
    }
    assert(this->cur_time != NULL);
    
    char log_buffer[2048] = {0};
    
    if (_timestamp == true)
    {
        char timestamp_buffer[64] = {0};
        sprintf(timestamp_buffer, "%04d%02d%02d %s (%s)\t",
                this->year,
                this->month,
                this->day,
                this->cur_time,
                __progname);
        strcat(log_buffer, timestamp_buffer);
    }
    char msg_buffer[1980] = {0};
    vsprintf(msg_buffer, _format, arg_list);
    
    strcat(log_buffer, msg_buffer);
    
    if (this->log_set.insert(log_buffer).second)
    {
        fprintf(this->log_file, "%s", log_buffer);
        fflush(this->log_file);
    }
    
    va_end(arg_list);
    pthread_mutex_unlock(&this->log_mutex);
}