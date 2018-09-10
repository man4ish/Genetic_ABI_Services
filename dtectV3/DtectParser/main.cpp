/* 
 * File:   main.cpp
 * Author: Manish
 *
 * Created on February 5, 2013, 12:24 PM
 */

#include <cstdlib>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "GSSConfig.h"
#include "GSSLogger.h"
#include "DirHandler.h"
#include "QSDb_Func.h"
#include "LimsDb.h"
#include "QSDb.h"

#define MAX_LENGTH 2048

extern char *__progname;

using namespace std;

bool exit_flag = false; // raise when signal is caught
GSSLogger *gss_logger = NULL;
GSSConfig gss_config;

void manual()
{
    fprintf(stdout, "\n Usage: %s [ start | stop ]\n\n", __progname);
}

void *parse_worker(void *_ptr)
{
    bool ret = gss_config.reinit();
    assert(ret == true);

    gss_logger = GSSLogger::get_instance();
    assert(gss_logger != NULL);
    ret = gss_logger->open_file(
            gss_config.get_logfile(),
            gss_config.get_redirect_stdout(),
            gss_config.get_redirect_stderr());
    assert(ret == true);
    
    gss_logger->log(true, "Started (pid:%u)\n", getpid());
    
    DirHandler dir_handler;
    while (exit_flag == false)
    {
        ret = gss_config.reinit();
        assert(ret == true);
        
        if (dir_handler.has_files(gss_config.get_qs_input_dir()) == false)
        {
            sleep(gss_config.get_interval());
            continue;
        }
        
        // Input file/s detected, parse it
        if (gss_config.get_use_lims() == false)
        {
            gss_logger->log(true, "ERROR: Incoming file/s pending but LIMS has been turned off. (abort)\n");
            continue;
        }
        
        string in_txt_filename, in_eds_filename;
        string in_txt_filesize, in_eds_filesize;
        while (dir_handler.get_filenames_and_sizes(in_txt_filename, in_txt_filesize, in_eds_filename, in_eds_filesize) == true)
        {
            FILE *inFile = fopen(in_txt_filename.c_str(), "r");
            assert(inFile != NULL);
            
            // rt library (/usr/lib64/librt.so) required for timing
            timespec start_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            
            // clock() for processor time
            clock_t start_clock = clock();
            
            gss_logger->log(true, "Files obtained: %s (%s), %s (%s)\n",
                    in_txt_filename.c_str(),
                    in_txt_filesize.c_str(),
                    in_eds_filename.c_str(),
                    in_eds_filesize.c_str());
            
            LimsDb lims_db("LIMS_DB");
            bool ret = lims_db.init(
                    gss_config.get_dbhost(lims_db.get_dbname()),
                    gss_config.get_dbuser(lims_db.get_dbname()),
                    gss_config.get_dbpswd(lims_db.get_dbname()));
            assert(ret == true);
            
            QSDb qs_db("QSDb");
            ret = qs_db.init(
                    gss_config.get_dbhost(qs_db.get_dbname()),
                    gss_config.get_dbuser(qs_db.get_dbname()),
                    gss_config.get_dbpswd(qs_db.get_dbname()));
            assert(ret == true);
            
            map<string, set<string> > sample_map;
            mysqlpp::sql_date test_date;
            mysqlpp::sql_varchar open_array_id;
            mysqlpp::sql_char chip_type;
            mysqlpp::sql_char chip_ver;
            
            set<size_t> array_sample_set;
            
            char qString[MAX_LENGTH] = {0};
            while (fgets(qString, MAX_LENGTH, inFile))
            {
                mysqlpp::sql_char qs_testcode;
                mysqlpp::sql_char sample_id;
                mysqlpp::sql_date qs_date;
                mysqlpp::sql_varchar assay_id;
                mysqlpp::sql_varchar vic_fam_call;
                mysqlpp::sql_enum manual;
                mysqlpp::sql_double quality = 0;
                mysqlpp::sql_double vic_rn = 0;
                mysqlpp::sql_double fam_rn = 0;
                mysqlpp::sql_double rox = 0;
                mysqlpp::sql_char well;
                bool lims_format = false;
                
                chomp(qString);
                vector<char*> fields;
                if (qString[0] == '#')  // comments
                {
                    if (strstr(qString, "Last Modified Date") != NULL)
                    {
                        char *ptr = qString;
                        
                        while (*ptr != ':') ptr++;
                        while (isdigit(*ptr) == false) ptr++;
                        test_date.month((unsigned char)(atoi(ptr)));
                        
                        while (*ptr != '/') ptr++;
                        while (isdigit(*ptr) == false) ptr++;
                        test_date.day((unsigned char)(atoi(ptr)));
                        
                        while (*ptr != '/') ptr++;
                        while (isdigit(*ptr) == false) ptr++;
                        test_date.year((unsigned short)(atoi(ptr)));
                        
                        continue;
                    }
                    continue;
                }
                if (qString[0] == '\n') continue;
                if (strstr(qString, "Assay Name") == qString) continue;
                if (rowParser(qString, '\t', fields) != 27) continue;
                
                if (is_valid_field(fields[1]) == true)      // assay id
                    assay_id = fields[1];
                if (is_valid_field(fields[4]) == true)      // sample id
                {
                    lims_format = is_lims_sample(fields[4]);
                    string t_buf = fields[4];
                    vector<char*> sample_id_vec;
                    size_t rCnt = rowParser(fields[4], '/', sample_id_vec);
                    if (rCnt == 3)
                    {
                        qs_testcode = sample_id_vec[0];
                        sample_id = sample_id_vec[1];
                        qs_date.convert(sample_id_vec[2]);
                    }
                    else
                        sample_id = t_buf;
                }

                if (is_valid_field(fields[5]) == true)      // call
                    vic_fam_call = fields[5];
                if (is_valid_field(fields[6]) == true)      // Manual
                {
                    manual = fields[6];
                    assert((strstr(manual.c_str(), "TRUE") != NULL) || (strstr(manual.c_str(), "FALSE") != NULL));
                }
                if (is_valid_field(fields[7]) == true)      // Quality
                {
                    char tmp[64] = {0};
                    strcpy(tmp, fields[7]);
                    if (filter_thousand_separator(tmp) == true)
                        quality = mysqlpp::sql_double(atof(tmp));
                }
                if (is_valid_field(fields[8]) == true)      // VIC(Rn)
                {
                    char tmp[64] = {0};
                    strcpy(tmp, fields[8]);
                    if (filter_thousand_separator(tmp) == true)
                        vic_rn = mysqlpp::sql_double(atof(tmp));
                }
                if (is_valid_field(fields[9]) == true)      // FAM (Rn)
                {
                    char tmp[64] = {0};
                    strcpy(tmp, fields[9]);
                    if (filter_thousand_separator(tmp) == true)
                        fam_rn = mysqlpp::sql_double(atof(tmp));
                }
                if (is_valid_field(fields[10]) == true)     // ROX
                {
                    char tmp[64] = {0};
                    strcpy(tmp, fields[10]);
                    if (filter_thousand_separator(tmp) == true)
                        rox = mysqlpp::sql_double(atof(tmp));
                }
                if (is_valid_field(fields[14]) == true)     // Well
                    well = fields[14];
                if (is_valid_field(fields[16]) == true)     // open_array_id
                {
                    if (open_array_id.empty() == true)
                        open_array_id = fields[16];
                    else { assert(open_array_id == fields[16]); }
                    assert(open_array_id.length() <= 8);
                }
                
                // if sample id is specified in config as filtered, do not proceed
                if (gss_config.is_filter(sample_id.c_str()) == true) continue;
                
                mysqlpp::sql_char lims_testcode;
                if ((lims_format) && (lims_db.is_valid(sample_id, open_array_id, lims_testcode) == false))
                {
                    gss_logger->log(true, "No/Invalid testcode obtained from LIMS for sampleid:%s and openarrayid:%s\n", sample_id.c_str(), open_array_id.c_str());
                    continue;
                }
                else if (lims_db.get_test_code(open_array_id, lims_testcode) == false)
                {
                    gss_logger->log(true, "No/Invalid testcode obtained from LIMS for openarrayid:%s\n", open_array_id.c_str());
                    continue;
                }
                if (is_valid_testcode(lims_testcode.c_str()) == false)
                {
                    gss_logger->log(true, "No/Invalid testcode obtained from LIMS for sampleid:%s and openarrayid:%s\n", sample_id.c_str(), open_array_id.c_str());
                    continue;
                }
                if ((chip_type.empty() == true) && (chip_ver.empty() == true))
                {
                    chip_type.assign(lims_testcode.c_str(), 3);
                    chip_ver.assign(lims_testcode.c_str() + 3, 2);
                    assert(chip_type.length() == 3);
                    assert(chip_ver.length() == 2);
                }
                else
                {
                    if ((strncmp(chip_type.c_str(), lims_testcode.c_str(), 3) != 0) ||
                        (strncmp(chip_ver.c_str(), lims_testcode.c_str() + 3, 2) != 0))
                    {
                        gss_logger->log(true, "ERROR: Inconsistent testcode from LIMS for %s\n", in_txt_filename.c_str());
                        continue;
                    }
                }
                // check the testcode obtained from the qs txt file against the one from LIMS
                if ((lims_format == true) && (qs_testcode != lims_testcode))
                {
                    gss_logger->log(true, "ERROR: LIMS testcode and QS testcode mismatch for sampleid:%s and openarrayid:%s\n", sample_id.c_str(), open_array_id.c_str());
                    continue;
                }
                
                // One line parsed and verified. Insert into QSDb.
                mysqlpp::sql_bigint_unsigned array_key = 0;
                ret = qs_db.insert_open_array(array_key, open_array_id, test_date, chip_type, chip_ver);
                assert(ret == true);
                assert(array_key != 0);
                
                
                mysqlpp::sql_bigint_unsigned sample_key = 0;
                // Initial insertion of sample, set ready flag to 0;
                ret = qs_db.insert_samples(sample_key, array_key, sample_id, mysqlpp::null, mysqlpp::null, mysqlpp::null, mysqlpp::null, 0);
                assert(ret == true);
                assert(sample_key != 0);
                
                // insert record into set to later on, set the 'ready' flag to true once file parsing is complete
                array_sample_set.insert(sample_key);
                
                mysqlpp::sql_bigint_unsigned result_key = 0;
                ret = qs_db.insert_results(result_key, sample_key, assay_id, vic_fam_call);
                assert(ret == true);
                assert(result_key != 0);
                
                ret = qs_db.insert_run_stats(result_key, well, manual, quality, vic_rn, fam_rn, rox);
                assert(ret == true);
                
                sample_map[sample_id].insert(string(assay_id + vic_fam_call));
            }
            for (map<string, set<string> >::iterator it = sample_map.begin(); it != sample_map.end(); ++it)
            {
                static size_t count = 0;
                if (it == sample_map.begin())
                    count = 1;
                
                gss_logger->log(true, " Sample %02lu of %02lu: %s found on array %s with %lu unique calls\n",
                        count,
                        sample_map.size(),
                        it->first.c_str(),
                        open_array_id.c_str(),
                        it->second.size());
            }
            fclose(inFile);
            inFile = NULL;
            timespec stop_time;
            clock_gettime(CLOCK_MONOTONIC, &stop_time);
            clock_t stop_clock = clock();
            gss_logger->log(true, "Done parsing file '%s' in %.2f sec (%.2f sec CPU).\n",
                    in_txt_filename.c_str(),
                    diff(start_time, stop_time),
                    double(stop_clock - start_clock)/double(CLOCKS_PER_SEC));
            
            // File parsed. Set the 'ready' flag to 1
            for (set<size_t>::iterator it = array_sample_set.begin(); it != array_sample_set.end(); ++it)
            {
                ret = qs_db.update_sample(*it, 1);
                assert(ret == true);
            }
            array_sample_set.clear();
            
            
            // File parsed. Move the file to archive directory
            string new_txt_filename;
            string new_eds_filename;
            
            new_txt_filename = string(gss_config.get_qs_move_dir()) + "/" + string(basename(in_txt_filename.c_str()));
            new_eds_filename = string(gss_config.get_qs_move_dir()) + "/" + string(basename(in_eds_filename.c_str()));
            
            if (rename(in_txt_filename.c_str(), new_txt_filename.c_str()) != 0)
            {
                gss_logger->log(true, "ERROR: Failed to move file '%s' to '%s' [%s].\n",
                        in_txt_filename.c_str(),
                        new_txt_filename.c_str(),
                        strerror(errno));
                gss_logger->log(true, "Exiting\n");
                exit(EXIT_FAILURE);
            }
            else
                gss_logger->log(true, "File %s moved to %s.\n", in_txt_filename.c_str(), new_txt_filename.c_str());
            
            if (rename(in_eds_filename.c_str(), new_eds_filename.c_str()) != 0)
            {
                gss_logger->log(true, "ERROR: Failed to move file '%s' to '%s' [%s].\n",
                        in_eds_filename.c_str(),
                        new_eds_filename.c_str(),
                        strerror(errno));
                gss_logger->log(true, "Exiting\n");
                exit(EXIT_FAILURE);
            }
            else
                gss_logger->log(true, "File %s moved to %s.\n", in_eds_filename.c_str(), new_eds_filename.c_str());
        }
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
    
    gss_logger->log(true, "SIGNAL '%s' caught (pid:%d).\n", strsignal(_sig), getpid());
    exit_flag = true;
}

int main(int argc, char** argv)
{
    // Debugging purpose
//    parse_worker(NULL);
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
            int ret = pthread_create(&thread1, NULL, parse_worker, NULL);
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
            fprintf(stdout, ".");
            fflush(stdout);
            sleep(1);
        }
        fprintf(stdout, "\r");
    }
    else
    {
        fprintf(stderr, "\n ERROR: Invalid usage of '%s'\n\n", argv[1]);
        manual();
        exit(EXIT_FAILURE);
    }
    return 0;
}

