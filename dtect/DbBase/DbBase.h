/* 
 * File:   DbBase.h
 * Author: jameswong
 *
 * Created on December 31, 2012, 4:22 PM
 */

#ifndef DBBASE_H
#define	DBBASE_H

#include <mysql++/mysql++.h>
#include <string>
#include <stdio.h>
#include "SqlQryBuf.h"

using namespace std;

class DbBase
{
private:
    string dbname;
    string dbhost;
    string dbuser;
    string dbpswd;
    SqlQryBuf sql_qry_buf;
    
protected:
    mysqlpp::Connection db_conn;
    
protected:
    bool create_database();
    
    bool connect(
        const char* _dbhost,
        const char* _dbuser,
        const char* _dbpswd);

    bool connect(
        const char *_dbhost,
        const char *_dbuser,
        const char *_dbpswd,
        bool _create);
    
    bool connect(
        const char *_dbname,
        const char *_dbhost,
        const char *_dbuser,
        const char *_dbpswd,
        bool _create);
    
    void disconnect();
    
    bool query_execute(
        mysqlpp::Query &_qry) const;
    
    bool query_execute(
        mysqlpp::Query &_qry,
        mysqlpp::SimpleResult &_sim_res) const;
    
    bool query_execute(
        mysqlpp::Query &_qry,
        mysqlpp::sql_int_unsigned &_auto_inc) const;
    
    bool query_execute(
        mysqlpp::Query &_qry,
        mysqlpp::sql_bigint_unsigned &_auto_inc) const;
    
    bool query_store(
        mysqlpp::Query &_qry,
        mysqlpp::StoreQueryResult &_str_res);
    
public:
    DbBase();
    DbBase(const char *_dbname);
    virtual ~DbBase();
    
    const char *get_dbname() const;
    
    bool reconnect();
    
    static void print(
        FILE *_file,
        const mysqlpp::StoreQueryResult *_str_res);
};

#endif	/* DBBASE_H */

