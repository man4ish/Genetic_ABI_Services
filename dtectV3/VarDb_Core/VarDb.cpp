/* 
 * File:   VarDb.cpp
 * Author: Manish
 * 
 * Created on November 6, 2012, 4:07 PM
 */

#include <stdio.h>
#include "VarDb.h"

VarDb::VarDb(
        const char* _dbname,
        const char* _host,
        const char* _user,
        const char* _pswd) : DbBase(_dbname)
{
    bool ret = this->connect(_host, _user, _pswd, false);
    assert(ret == true);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SET NAMES utf8";
    ret = this->query_execute(qry);
    assert(ret == true);
}

VarDb::VarDb(const char *_dbname) : DbBase(_dbname)
{
}

VarDb::~VarDb()
{
}


bool VarDb::init(
        const char* _host,
        const char* _user,
        const char* _pswd)
{
    bool ret = this->connect(_host, _user, _pswd, false);
    assert(ret == true);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SET NAMES utf8";
    ret = this->query_execute(qry);
    assert(ret == true);

}

bool VarDb::reinit()
{
    bool ret = this->reconnect();
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SET NAMES utf8";
    ret = this->query_execute(qry);
    assert(ret == true);
}

bool VarDb::get_gene_symbol(unsigned _rsid, string& _gene_symbol)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT GeneMain.EntrezSymbol "
            "FROM GeneMain "
            "JOIN SnpGeneLink "
            "ON GeneMain.EntrezId_NK=SnpGeneLink.EntrezId_FK "
            "WHERE "
            "SnpGeneLink.RsId_FK=" << mysqlpp::quote << _rsid;
    
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    for (size_t i = 0; i < str_res.size(); ++i)
    {
        _gene_symbol += str_res[i][0].c_str();
        if ((str_res.size() > 1) && (i < (str_res.size() - 1)))
            _gene_symbol += ", ";
    }
    return ret;
}

bool VarDb::get_citation(
        unsigned _pmid,
        string &_author,
        string &_title,
        string &_journal,
        string &_pub_date)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT "
            "Citation.Author,"
            "Citation.ArticleTitle,"
            "Citation.ISOAbbreviation,"
            "Citation.PublicationDate "
            "FROM Citation "
            "WHERE "
            "Citation.PmId_NK=" << mysqlpp::quote << _pmid;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    assert(str_res.size() == 1);
    _author = str_res[0]["Author"].c_str();
    _title = str_res[0]["ArticleTitle"].c_str();
    _journal = str_res[0]["ISOAbbreviation"].c_str();
    _pub_date = str_res[0]["PublicationDate"].c_str();
    
    return ret;
}
