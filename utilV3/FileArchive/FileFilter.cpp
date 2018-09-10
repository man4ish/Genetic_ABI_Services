/* 
 * File:   GBLPOP.cpp
 * Author: wuntyng
 * 
 * Created on June 18, 2012, 2:37 PM
 */

#include <mysql++/connection.h>

#include "FileFilter.h"

FileReader::FileReader()
{
    this->db_conn.connect("LIMS_DB", "192.168.7.5", "root", "");
    assert(this->db_conn.connected() == true);
}

FileReader::~FileReader()
{
    this->db_conn.disconnect();
}

bool FileReader::get_filename(set<string>& _filename_set)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT Filename FROM LIMS_DB.SampleTestReport WHERE Filename!=\"NULL\" ORDER BY Filename";
    this->store = qry.store();
    
    if(this->store.size() != 0)
    {
        for(size_t i = 0; i < this->store.size(); ++i)
            _filename_set.insert(this->store[i]["Filename"].c_str());
    }
    
    return this->store.empty();
}
