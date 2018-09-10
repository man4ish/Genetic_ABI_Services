/* 
 * File:   DbBase.cpp
 * Author: jameswong
 * 
 * Created on December 31, 2012, 4:22 PM
 */

#include "DbBase.h"
#include <assert.h>
#include <stdio.h>

DbBase::DbBase()
{
    this->dbname.clear();
    this->dbhost.clear();
    this->dbuser.clear();
    this->dbpswd.clear();
}

DbBase::DbBase(const char* _dbname)
{
    assert(_dbname != NULL);
    this->dbname.assign(_dbname);
    this->dbhost.clear();
    this->dbuser.clear();
    this->dbpswd.clear();
}

DbBase::~DbBase()
{
    this->disconnect();
}

bool DbBase::create_database()
{
    if (this->db_conn.connected() == false)
        return false;
    if (this->dbname.empty())
        return false;
    
    try
    {
        this->db_conn.create_db(this->dbname);
    }
    catch (const mysqlpp::Exception &e)
    {
        fprintf(stderr, "Exception caught: %s\n", e.what());
        return false;
    }
    return true;
}

bool DbBase::connect(
        const char* _dbhost,
        const char* _dbuser,
        const char* _dbpswd)
{
    assert(_dbhost != NULL);
    assert(_dbuser != NULL);
    this->db_conn.enable_exceptions();
    this->sql_qry_buf.reset();
    
    if (this->db_conn.connected() == true)
        return true;
    
    bool ret = false;
    try
    {
        ret = this->db_conn.connect("", _dbhost, _dbuser, _dbpswd);
        if ((ret == false) || (this->db_conn.connected() == false))
            return false;
        
        if (this->dbname.empty() == false)
            ret = this->db_conn.select_db(this->dbname);
    }
    catch (const mysqlpp::DBSelectionFailed &e)
    {
        if ((e.errnum() == 1049) && (this->dbname.empty() == false)) // Unable to select db
        {
            ret = this->create_database();
            if (ret == false)
                throw e;
        }
        else
        {
            fprintf(stderr, "Exception caught: %s [%d]\n", e.what(), e.errnum());
            return false;
        }
    }
    catch (const mysqlpp::Exception &e)
    {
        fprintf(stderr, "Exception caught: %s\n", e.what());
        return false;
    }
    this->dbhost.assign(_dbhost);
    this->dbuser.assign(_dbuser);
    this->dbpswd.assign(_dbpswd);
    return ret;
}

bool DbBase::connect(
        const char* _dbhost,
        const char* _dbuser,
        const char* _dbpswd,
        bool _create)
{
    if (this->dbname.empty()) return false;
    this->db_conn.enable_exceptions();
    this->sql_qry_buf.reset();
    
    bool ret = this->connect(_dbhost, _dbuser, _dbpswd);
    if (ret == false)
        return false;
    
    bool db_exists = false;
    try
    {
        mysqlpp::Query qry = this->db_conn.query();
        qry << "SHOW DATABASES";
        mysqlpp::StoreQueryResult qry_store;
        bool ret = this->query_store(qry, qry_store);
        if (ret == false) return false;
        
        for (size_t i = 0; (i < qry_store.size()) && (db_exists == false); ++i)
            if (qry_store[i][0] == this->dbname)
                db_exists = true;
        
        if ((db_exists == false) && (_create == true))
        {
            if (this->db_conn.create_db(this->dbname) == false)
                return false;
        }
    }
    catch (const mysqlpp::Exception &e)
    {
        fprintf(stderr, "Exception caught: %s\n", e.what());
        return false;
    }
    return this->db_conn.select_db(this->dbname);
}

bool DbBase::connect(
        const char* _dbname,
        const char* _dbhost,
        const char* _dbuser,
        const char* _dbpswd,
        bool _create)
{
    assert(_dbname != NULL);
    assert(this->dbname.empty());
    this->dbname.assign(_dbname);
    return this->connect(_dbhost, _dbuser, _dbpswd, _create);
}

void DbBase::disconnect()
{
    if (this->db_conn.connected())
    {
        try
        {
            this->db_conn.disconnect();
        }
        catch (const mysqlpp::Exception &e)
        {
            fprintf(stderr, "Exception caught: %s\n", e.what());
        }
    }
    this->db_conn.thread_end();
    mysql_library_end();
}

bool DbBase::query_execute(
    mysqlpp::Query& _qry) const
{
    try
    {
        _qry.execute();
    }
    catch (const mysqlpp::BadQuery &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::BadConversion &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::Exception &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    return true;
}

bool DbBase::query_execute(
        mysqlpp::Query& _qry,
        mysqlpp::SimpleResult &_sim_res) const
{
    try
    {
        _sim_res = _qry.execute();
    }
    catch (const mysqlpp::BadQuery &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::BadConversion &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::Exception &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    return true;
}

bool DbBase::query_execute(
        mysqlpp::Query& _qry,
        mysqlpp::sql_int_unsigned& _auto_inc) const
{
    try
    {
        mysqlpp::SimpleResult sim_res = _qry.execute();
        _auto_inc = mysqlpp::sql_int_unsigned(sim_res.insert_id());
    }
    catch (const mysqlpp::BadQuery &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::BadConversion &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::Exception &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    return true;
}

bool DbBase::query_execute(
        mysqlpp::Query& _qry,
        mysqlpp::sql_bigint_unsigned& _auto_inc) const
{
    try
    {
        mysqlpp::SimpleResult sim_res = _qry.execute();
        _auto_inc = mysqlpp::sql_bigint_unsigned(sim_res.insert_id());
    }
    catch (const mysqlpp::BadQuery &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::BadConversion &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::Exception &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    return true;
}

bool DbBase::query_store(
        mysqlpp::Query& _qry,
        mysqlpp::StoreQueryResult& _str_res)
{
    string qry_buf = _qry.str();
    if (this->sql_qry_buf.check(qry_buf, _str_res) == true)
        return true;
    try
    {
        _str_res = _qry.store();
        this->sql_qry_buf.insert(qry_buf, &_str_res);
    }
    catch (const mysqlpp::BadQuery &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::BadConversion &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    catch (const mysqlpp::Exception &e)
    {
        cerr << e.what() << endl;
        return false;
    }
    return true;
}

const char *DbBase::get_dbname() const
{
    if (this->dbname.empty())
        return NULL;
    return this->dbname.c_str();
}

bool DbBase::reconnect()
{
    if (this->dbhost.empty())
        return false;
    
    this->disconnect();
    return this->connect(this->dbhost.c_str(), this->dbuser.c_str(), this->dbpswd.c_str());
}

void DbBase::print(
        FILE *_file,
        const mysqlpp::StoreQueryResult* _str_res)
{
    assert(_file != NULL);
    assert(_str_res != NULL);
    
    fprintf(_file, "\n");
    size_t field_cnt = _str_res->num_fields();
    map<size_t, size_t> field_width;
    
    for (size_t i = 0; i < _str_res->size(); ++i)
    {
        mysqlpp::Row row = _str_res->at(i);
        for (size_t j = 0; j < field_cnt; ++j)
        {
            if (field_width.find(j) == field_width.end())
                field_width.insert(pair<size_t, size_t>(j, row.at(j).length()));
            else if (field_width[j] < row.at(j).length())
                field_width[j] = row.at(j).length();
        }
    }
    for (size_t j = 0; j < field_cnt; ++j)
    {
        if (field_width.find(j) == field_width.end())
            field_width.insert(pair<size_t, size_t>(j, _str_res->field_name(j).length()));
        else if (field_width[j] < _str_res->field_name(j).length())
            field_width[j] = _str_res->field_name(j).length();
    }
    
    for (size_t j = 0; j < field_cnt; ++j)
        fprintf(_file, "%-*s  ", (int)field_width.find(j)->second, _str_res->field_name(j).c_str());
    fprintf(_file, "\n");
    
    for (size_t i = 0; i < _str_res->size(); ++i)
    {
        mysqlpp::Row row = _str_res->at(i);
        for (size_t j = 0; j < field_cnt; ++j)
            fprintf(_file, "%-*s  ", (int)field_width.find(j)->second, row.at(j).c_str());
        fprintf(_file, "\n");
    }
    fprintf(_file, "\n");
    
    fprintf(_file, "%lu records.", _str_res->size());
    fprintf(_file, "\n");
}