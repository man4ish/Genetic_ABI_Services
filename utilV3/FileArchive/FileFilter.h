/* 
 * File:   GBLPOP.h
 * Author: wuntyng
 *
 * Created on June 18, 2012, 2:37 PM
 */

#ifndef GBLPOP_H
#define	GBLPOP_H

#include <cstdlib>
#include <cstdio>
#include <assert.h>
#include <mysql++/mysql++.h>

using namespace std;

class FileReader
{
private:
    mysqlpp::Connection db_conn;
    mysqlpp::StoreQueryResult store;
    
public:
    FileReader();
    ~FileReader();
    
    bool get_filename(set<string>& _filename_set);
};

#endif	/* GBLPOP_H */

