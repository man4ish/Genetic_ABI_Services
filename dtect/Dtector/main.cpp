/*
 * File:   main.cpp
 * Author: jameswong
 *
 * Created on February 5, 2013, 12:24 PM
 */

#include <cstdlib>
#include <stdio.h>
#include <mysql++/datetime.h>
#include <mysql++/sql_types.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "GSSConfig.h"
#include "GSSLogger.h"
#include "QSDb_Func.h"
#include "OpenDb.h"
#include "DtectMapper.h"

extern char *__progname;

using namespace std;

bool exit_flag = false; // raise when signal is caught
GSSLogger *gss_logger = NULL;
GSSConfig gss_config;
TransConfig trans_config;

void manual()
{
    fprintf(stdout, "\n Usage: %s [ start | stop ]\n\n", __progname);
}

void *dtect_worker(void *_ptr)
{
    bool ret;
    gss_logger = GSSLogger::get_instance();
    assert(gss_logger != NULL);
    ret = gss_logger->open_file(
            gss_config.get_logfile(),
            gss_config.get_redirect_stdout(),
            gss_config.get_redirect_stderr());
    assert(ret == true);
    
    ret = gss_config.reinit();
    assert(ret == true);
    
    OpenDb open_db;
    ret = open_db.init(
            gss_config.get_dbhost("LIMS_DB"),
            gss_config.get_dbuser("LIMS_DB"),
            gss_config.get_dbpswd("LIMS_DB"));
    assert(ret == true);
    
    gss_logger->log(true, "Started (pid:%u)\n", getpid());
    
    // dtect_mapper object put outside the infinite loop to keep the 'global population' data
    // in memory
    DtectMapper dtect_mapper;
    
    while (exit_flag == false)
    {
        ret = open_db.reconnect();
        assert(ret == true);
        
        // Obtain any outstanding sample from LIMS and QSDb.
        // Outstanding samples are those without a report date AND exist in QSDb.
        mysqlpp::StoreQueryResult out_res;
        ret = open_db.get_outstanding_samples_from_limsdb_n_qsdb(out_res);
        assert(ret == true);
        
        for (size_t i = 0; i < out_res.size(); ++i)
        {
            // potential point for performance improvement by spawning a thread for each sample
            // but underlying classes/functions have to be made thread-safe
            gss_logger->log(true, "Processing sample %02lu of %02lu : %s-%s-%s\n",
                    i + 1,
                    out_res.size(),
                    out_res[i]["SampleID"].c_str(),
                    out_res[i]["TestCode"].c_str(),
                    out_res[i]["LastOpenArrayID"].c_str());
            
            ret = dtect_mapper.map_sample(
                    out_res[i]["SampleID"].c_str(),
                    out_res[i]["TestCode"].c_str(),
                    out_res[i]["LastOpenArrayID"].c_str());
            assert(ret == true);
        }
        out_res.clear();
        sleep(1);
    }
    gss_logger->log(true, "Stopped (pid:%u)\n", getpid());
    GSSLogger::destroy();

    char pid_filename[MAX_LENGTH] = {0};
    sprintf(pid_filename, "/tmp/%s.pid", __progname);
    remove(pid_filename);
}

void signal_callback_handler(int _sig)
{
    bool ret = gss_config.reinit();
    assert(ret == true);

    gss_logger = GSSLogger::get_instance();
    assert(gss_logger != NULL);
    gss_logger->open_file(
            gss_config.get_logfile(),
            gss_config.get_redirect_stdout(),
            gss_config.get_redirect_stderr());
    gss_logger->log(true, "SIGNAL '%s' caught (pid:%d)\n", strsignal(_sig), getpid());
    exit_flag = true;
}

int main(int argc, char** argv)
{
    // for debugging purpose, call function directly and return
//    dtect_worker(NULL);
//    return 0;
    
    char pid_filename[MAX_LENGTH] = {0};
    sprintf(pid_filename, "/tmp/%s.pid", __progname);
    struct stat st;
    struct tm *timeinfo = NULL;
    if (stat(pid_filename, &st) == 0)
    {
        time_t ftime = st.st_mtim.tv_sec;
        timeinfo = localtime(&ftime);
        
        time_t now;
        time(&now);
        time_t elapsed = now - st.st_mtim.tv_sec;
        double days = double(elapsed)/double(60*60*24);
        double hours = double(elapsed - (time_t(days)*60*60*24))/double(60*60);
        double minutes = double(elapsed - (time_t(days)*60*60*24) - (time_t(hours)*60*60))/double(60);
        double seconds = double(elapsed - (time_t(days)*60*60*24) - (time_t(hours)*60*60) - (time_t(minutes)*60));
        
        fprintf(stdout, "\n NOTE: Instance running for %02dD:%02dH:%02dM:%02dS since %s",
                int(days),
                int(hours),
                int(minutes),
                int(seconds),
                asctime(timeinfo));
    }
    if (argc != 2)
    {
        manual();
        exit(EXIT_SUCCESS);
    }
    
    int curr_pid = 0;
    FILE *pid_file = fopen(pid_filename, "r");
    if (pid_file != NULL)
    {
        char qString[MAX_LENGTH] = {0};
        char *ret = fgets(qString, MAX_LENGTH, pid_file);
        assert(ret != NULL);
        curr_pid = atoi(qString);
        fclose(pid_file);
        pid_file = NULL;
    }

    if (strcmp(argv[1], "start") == 0)
    {
        if (curr_pid != 0)
        {
            fprintf(stderr, "\n IGNORED: Another instance (pid:%d) is running.\n\n", curr_pid);
            exit(EXIT_SUCCESS);
        }

        pid_t pID = fork();
        if (pID == 0)
        {
            pthread_t thread1;
            int ret = pthread_create(&thread1, NULL, dtect_worker, NULL);
            assert(ret == 0);

            signal(SIGINT, signal_callback_handler);
            signal(SIGABRT, signal_callback_handler);
            signal(SIGTERM, signal_callback_handler);

            pthread_join(thread1, NULL);
        }
        else if (pID < 0)
        {
            fprintf(stderr, "\n ERROR %d\n", pID);
            exit(EXIT_FAILURE);
        }
        else
        {
            pid_file = fopen(pid_filename, "w");
            fprintf(pid_file, "%d", int(pID));
            fclose(pid_file);
            pid_file = NULL;
        }
    }
    else if (strcmp(argv[1], "stop") == 0)
    {
        if (curr_pid == 0)
        {
            fprintf(stderr, "\n IGNORED: No running instance.\n\n");
            exit(EXIT_SUCCESS);
        }
        assert(curr_pid > 0);
        int ret = kill(curr_pid, SIGTERM);
        if (ret != 0)
        {
            fprintf(stderr, "\n ERROR: Failed to kill process %d. [%s]\n", curr_pid, strerror(errno));
            exit(EXIT_FAILURE);
        }
        struct stat st;
        while (stat(pid_filename, &st) == 0)
        {
            sleep(1);
            fprintf(stdout, ".");
            fflush(stdout);
        }
        fprintf(stdout, "\n\r");
    }
    else
    {
        fprintf(stderr, "\n ERROR: Invalid usage of '%s'\n\n", argv[1]);
        manual();
        exit(EXIT_FAILURE);
    }
    return 0;
}

