/* 
 * File:   OpenDb.h
 * Author: jameswong
 *
 * Created on February 23, 2013, 10:15 PM
 */

#ifndef OPENDB_H
#define	OPENDB_H

#include "DbBase.h"

// This class is used to make a connection to mysql server WITHOUT specifying a database.
// All SELECT statements must contain database names.

class OpenDb : public DbBase
{
public:
    OpenDb();
    ~OpenDb();
    
    bool init(
            const string& _host,
            const string& _user,
            const string& _pswd);
    
    bool get_outstanding_samples_from_limsdb_n_qsdb(
            mysqlpp::StoreQueryResult &_res);
    
    bool map_sample_from_qsdb_against_gssdb(
            const string& _sample_id,
            const string& _open_array_id,
            const string& _chip_type,
            const string& _chip_ver,
            mysqlpp::StoreQueryResult& _res);
};

#endif	/* OPENDB_H */

