/* 
 * File:   GSSDb_QryStoreBuf.cpp
 * Author: jameswong
 * 
 * Created on June 23, 2012, 4:07 PM
 */

#include "SqlQryBuf.h"
#include <stdio.h>

SqlQryBuf::SqlQryBuf()
{
    this->query_store_map.clear();
}

SqlQryBuf::~SqlQryBuf()
{
    this->reset();
}

bool SqlQryBuf::check(
        const string &_qry,
        mysqlpp::StoreQueryResult& _qry_res) const
{
    assert(_qry.empty() == false);
    if (strncmp(_qry.c_str(), "SELECT", 6) != 0)
        return false;
    map<string, mysqlpp::StoreQueryResult>::const_iterator const_it;
    const_it = this->query_store_map.find(_qry);
    if (const_it != this->query_store_map.end())
    {
        _qry_res = const_it->second;
        return true;
    }
    return false;
}

void SqlQryBuf::insert(
        const string &_qry,
        const mysqlpp::StoreQueryResult* _qry_res)
{
    if (strncmp(_qry.c_str(), "SELECT", 6) != 0)
        return;
    assert(_qry.empty() == false);
    assert(_qry_res != NULL);
    if (this->query_store_map.find(_qry) == this->query_store_map.end())
        this->query_store_map.insert(pair<string, mysqlpp::StoreQueryResult>(_qry, mysqlpp::StoreQueryResult(*_qry_res)));
}

void SqlQryBuf::reset()
{
    if (this->query_store_map.empty() == false)
        this->query_store_map.clear();
}
