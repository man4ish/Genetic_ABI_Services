/* 
 * File:   DirHandler.cpp
 * Author: jameswong
 * 
 * Created on July 3, 2012, 2:50 PM
 */

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include <map>
#include "DirHandler.h"

DirHandler::DirHandler()
{
    this->gss_logger = GSSLogger::get_instance();
    assert(this->gss_logger != NULL);
    this->filename_ext_map.clear();
    time(&this->start);
}

DirHandler::~DirHandler()
{
    for (this->filename_ext_it = this->filename_ext_map.begin();
         this->filename_ext_it != this->filename_ext_map.end();
         ++this->filename_ext_it)
        this->filename_ext_it->second.clear();
    this->filename_ext_map.clear();
}

void DirHandler::convert_filesize(
        size_t _size,
        string& _size_in_str)
{
    _size_in_str.clear();
    
    static const char* SIZES[] = { "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
    size_t div = 0, rem = 0;
    
    while (_size >= 1024 && div < ((sizeof(SIZES)/sizeof(*SIZES)) - 1))
    {
        rem = _size % 1024;
        div++;
        _size /= 1024;
    }
    char str[1024] = {0};
    sprintf(str, "%.1f %s", double(_size) + double(rem)/1024.0, SIZES[div]);
    _size_in_str = str;
}

bool DirHandler::has_files(
        const string& _directory)
{
    if (_directory.empty() == true)
    {
        this->gss_logger->log(true, "ERROR: Invalid or no directory specified.\n");
        return false;
    }
    DIR *dp = opendir(_directory.c_str());
    if (dp == NULL)
    {
        this->gss_logger->log(true, "ERROR: Failed to open directory %s [%s].\n", _directory.c_str(), strerror(errno));
        return false;
    }
    
    time_t now;
    time(&now);
    if (now - this->start > 3600)   // Clear ignore list hourly
    {
        this->ignore_set.clear();
        time(&this->start);
    }
    
    struct dirent *dirp;
    while ((dirp = readdir(dp)) != NULL)
    {
        const char *filename = dirp->d_name;
        assert(filename != NULL);

        string file_root_name, file_ext;
        if ((strcmp(filename, ".") == 0) ||
            (strcmp(filename, "..") == 0) ||
            (this->check_file_name(filename, file_root_name, file_ext) == false))
        {
        }
        else
        {
            string full_path_filename_no_ext = _directory + "/" + file_root_name;
            
            // Ignore the file if it is already in the ignore set
            // ignore_set is hourly cleared
            if (this->ignore_set.find(full_path_filename_no_ext) != this->ignore_set.end())
                continue;
            
            string full_path_filename = _directory + "/" + string(filename);
            
            struct stat st;
            int ret = stat(full_path_filename.c_str(), &st);
            if ((ret != 0) || (st.st_size == 0))
            {
                this->gss_logger->log(true, "  WARNING: File '%s' is empty or failed to open. File ignored.\n", full_path_filename.c_str());
                this->ignore_set.insert(full_path_filename_no_ext);
                this->filename_ext_it = this->filename_ext_map.find(full_path_filename_no_ext);
                if (this->filename_ext_it != this->filename_ext_map.end())
                    this->filename_ext_map.erase(this->filename_ext_it);
                continue;
            }
            string filesize_str;
            this->convert_filesize(st.st_size, filesize_str);
            this->filename_ext_map[full_path_filename_no_ext].insert(pair<string, string>(file_ext, filesize_str));
        }
    }
    closedir(dp);
    dp = NULL;
    
    for (this->filename_ext_it = this->filename_ext_map.begin(); this->filename_ext_it != this->filename_ext_map.end();)
    {
        map<string, map<string, string> >::iterator del_it = this->filename_ext_it++;
        if (del_it->second.size() != 2) // Expecting file with two extensions (eds & txt)
        {
            this->ignore_set.insert(del_it->first);
            this->gss_logger->log(true, "  WARNING: File %s.* is not in a pair.\n",
                    del_it->first.c_str());
            del_it->second.clear();
            this->filename_ext_map.erase(del_it);
        }
    }
    this->filename_ext_it = this->filename_ext_map.begin();
    return !this->filename_ext_map.empty();
}

bool DirHandler::check_file_name(
        const char* _str,
        string& _file_root_name,
        string& _file_ext) const
{
    // Expected files have the following name formats:
    //  YYYYMMDD_<testcode>_<open_array_id>_<version>.eds
    //  YYYYMMDD_<testcode>_<open_array_id>_<version>.txt
    //
    // Examples:
    //  20120101_CRD01_HWG60_V1.eds
    //  20120101_CRD01_HWG60_V1.txt
    //
    // Note: Incoming files must come in a pair (eds & txt)
    
    _file_root_name.clear();
    _file_ext.clear();
    if (_str != NULL)
    {
        size_t len = strlen(_str);
        if (len != 27)
            return false;
        
        for (size_t i = 0; i < len; i++)
        {
            if ((i < 8) && (isdigit(_str[i]) == false))                                                 // YYYYMMDD
                return false;
            if ((i == 8) && (_str[i] != '_'))                                                           // _
                return false;
            if ((i >= 9) && (i <= 11) && (isalpha(_str[i]) == false) && (isupper(_str[i]) == false))    // CRD
                return false;
            if ((i >= 12) && (i <= 13) && (isdigit(_str[i]) == false))                                  // 01
                return false;
            if ((i == 14) && (_str[i] != '_'))                                                          // _
                return false;
            if ((i >= 15) && (i <= 17) && (isalpha(_str[i]) == false) && (isupper(_str[i]) == false))   // HWG
                return false;
            if ((i >= 18) && (i <= 19) && (isdigit(_str[i]) == false))                                  // 01
                return false;
            if ((i == 20) && (_str[i] != '_'))                                                          // _
                return false;
            if ((i == 21) && (isalpha(_str[i]) == false) && (isupper(_str[i]) == false))                // V
                return false;
            if ((i == 22) && (isdigit(_str[i]) == false))                                               // version number
                return false;
            if ((i == 23) && (_str[i] != '.'))                                                          // .
                return false;
            if ((strcmp(_str + 24, "eds") != 0) && (strcmp(_str + 24, "txt") != 0))                     // eds/txt
                return false;
        }
        _file_root_name.assign(_str, 23);
        _file_ext.assign(_str + 24);
        return true;
    }
    return false;
}

bool DirHandler::get_filenames_and_sizes(
        string& _txt_filename,
        string& _txt_filesize,
        string& _eds_filename,
        string& _eds_filesize)
{
    _txt_filename.clear();
    _txt_filesize.clear();
    _eds_filename.clear();
    _eds_filesize.clear();
    
    if (this->filename_ext_it == this->filename_ext_map.end())
        return false;
    
    map<string, string>::const_iterator file_ext_it;
    
    file_ext_it = this->filename_ext_it->second.find("txt");
    assert(file_ext_it != this->filename_ext_it->second.end());
    _txt_filename.assign(this->filename_ext_it->first + "." + file_ext_it->first);
    _txt_filesize = file_ext_it->second;
    
    file_ext_it = this->filename_ext_it->second.find("eds");
    assert(file_ext_it != this->filename_ext_it->second.end());
    _eds_filename.assign(this->filename_ext_it->first + "." + file_ext_it->first);
    _eds_filesize = file_ext_it->second;
    
    // Delete this record from the map
    map<string, map<string, string> >::iterator del_it = this->filename_ext_it;
    del_it->second.clear();
    this->filename_ext_map.erase(del_it);
    
    ++this->filename_ext_it;
    return true;
}