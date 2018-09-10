/* 
 * File:   DirHandler.h
 * Author: jameswong
 *
 * Created on July 3, 2012, 2:50 PM
 */

#ifndef DIRHANDLER_H
#define	DIRHANDLER_H

#include <dirent.h>
#include <string.h>
#include <vector>
#include "GSSLogger.h"

using namespace std;

class DirHandler
{
private:        // private member variables
    // map-of-map to store root-filename (key of map) and extensions as keys in map with filesize as value
    map<string, map<string, string> > filename_ext_map;
    map<string, map<string, string> >::iterator filename_ext_it;
    
    set<string> ignore_set;
    GSSLogger *gss_logger;
    time_t start;
    
private:        // private member functions
    bool check_file_name(
            const char* _str,
            string& _file_root_name,
            string& _file_ext) const;
    
    void static convert_filesize(
            size_t _size,
            string& _size_in_str);
    
public:         // public member functions
    DirHandler();
    ~DirHandler();
    
    bool has_files(
            const string& _directory);
    
    bool get_filenames_and_sizes(
            string& _txt_filename,
            string& _txt_filesize,
            string& _eds_filename,
            string& _eds_filesize);
};

#endif	/* DIRHANDLER_H */

