/* 
 * File:   GSSDb_QryStoreBuf.h
 * Author: jameswong
 *
 * Created on June 23, 2012, 4:07 PM
 */

#ifndef GSSDB_QRYSTOREBUF_H
#define	GSSDB_QRYSTOREBUF_H

#include <string>
#include <map>
#include <mysql++/mysql++.h>

using namespace std;
class SqlQryBuf
{
private:
    map<string, mysqlpp::StoreQueryResult> query_store_map;
    
public:
    SqlQryBuf();
    ~SqlQryBuf();
    
    bool check(
        const string &_qry,
        mysqlpp::StoreQueryResult &_qry_res) const;
    
    void insert(
        const string &_qry,
        const mysqlpp::StoreQueryResult *_qry_res);
    
    void reset();
};

#endif	/* GSSDB_QRYSTOREBUF_H */

