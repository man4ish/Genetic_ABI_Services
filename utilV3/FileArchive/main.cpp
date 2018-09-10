/* 
 * File:   main.cpp
 * Author: wuntyng
 *
 * Created on July 4, 2012, 11:13 AM
 */

#include <cstdlib>
#include <stdio.h>
#include <dirent.h>
#include "FileFilter.h"

using namespace std;

size_t rowParser(char *_source, const char _delimiter, vector<char*> &_fields_vec)
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

size_t rowParser(char *_source, const char *_delimiter, vector<char*> &_fields_vec)
{
    _fields_vec.clear();
    char *pch = strtok(_source, _delimiter);
    while (pch != NULL)
    {
        _fields_vec.push_back(pch);
        pch = strtok(NULL, _delimiter);
    }
    return _fields_vec.size();
}

int main(int argc, char** argv) 
{
    if (argc != 2) {
	fprintf(stdout,"Invalid Parameter(s)!\n");
        fprintf(stdout,"Usage: %s <PDF Output Folder(Absolute Path)>\n", argv[0]);
        exit(1);
    }
    
    FileReader filereader;
    set<string> filename_set;
    set<string>::iterator set_it;
    
    bool ret = filereader.get_filename(filename_set);
    assert(ret == false);
    assert(filename_set.size() > 0);
    
    DIR *dir = NULL;
    struct dirent *drnt = NULL;
    dir = opendir(argv[1]);
    if(dir == NULL)
    {
        cout << "Could not open directory\n";
        exit(1);
    }
    
    vector<char *>path;
    vector<char *>png;
    while(drnt = readdir(dir))
    {
        if(drnt != NULL)
        {
            if(drnt->d_name[0] == '.') { continue; }
            if(strcmp(drnt->d_name, "Archive") == 0) { continue; }

            if (strstr(drnt->d_name,".xml")== NULL && strstr(drnt->d_name,".png")== NULL &&
                strstr(drnt->d_name,".pdf")== NULL) { continue; }
            
            bool hit = false;
            for(set_it = filename_set.begin(); set_it != filename_set.end(); ++set_it)
            {
                char * filepath = new char [strlen(set_it->c_str())];;
                strcpy(filepath, set_it->c_str());
                size_t cnt = rowParser(filepath, "/.", path);
                
                if(strstr(drnt->d_name, path[cnt - 2]) != NULL)
                {
                    hit = true;
                    cout << "HIT: " << drnt->d_name << "\t" << path[cnt - 2] << endl;
                }
                
                if(strstr(drnt->d_name, "png") != NULL)
                {
                    char * tmp = new char[strlen(drnt->d_name)];
                    strcpy(tmp, drnt->d_name);
                    rowParser(tmp, '.', png);
                    
                    if(strstr(path[cnt - 2], png[0]) != NULL)
                    {
                        hit = true;
                        cout << "HIT png: " << drnt->d_name << "\t" << path[cnt - 2] << endl;
                    }
                }
            }
            
            if(hit == false)
            {
                char command[1000] = {0};
                sprintf(command, "mv %s/%s %s/Archive/", argv[1], drnt->d_name, argv[1]);
                cout << command << endl;
                system(command);
            }
        }
    }
    
    closedir(dir);
    return 0;
}
