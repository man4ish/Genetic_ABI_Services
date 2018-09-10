/* 
 * File:   VarDb.h
 * Author: jameswong
 *
 * Created on November 6, 2012, 4:06 PM
 */

#ifndef VARDB_H
#define	VARDB_H

#include "DbBase.h"

using namespace std;

class VarDb : public DbBase
{
public:
    VarDb(const char *_dbname,
          const char *_host,
          const char *_user,
          const char *_pswd);

    VarDb(const char *_dbname);   
    
    ~VarDb();

    bool init(const char* _host,const char* _user,const char* _pswd);

    bool reinit();
    
    bool get_gene_symbol(unsigned _rsid, string &_gene_symbol);
    
    bool get_citation(
        unsigned _pmid,
        string &_author,
        string &_title,
        string &_journal,
        string &_pub_date);
};

#endif	/* VARDB_H */

