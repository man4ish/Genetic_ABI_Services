/* 
 * File:   TransDb.h
 * Author: jameswong
 *
 * Created on December 31, 2012, 4:10 PM
 */

#ifndef TRANSDB_H
#define	TRANSDB_H

#include "DbBase.h"

using namespace std;

class TransDb : public DbBase
{
private:
    
public:
    TransDb(const char *_dbname);
    ~TransDb();
    
    bool init(
        const char *_dbhost,
        const char *_dbuser,
        const char *_dbpswd);
    
    bool insert_text_version(
        const char *_doc_id,
        const char *_lang_code,
        int _text_ver_no,
        int lock_text);
    
    bool insert_document(
        const char *_doc_id,
        const char *_doc_desc,
        int _doc_type_idx);
    
    bool insert_content(
        const char *_doc_id,
        const char *_lang_code,
        const char *_text_ver_no,
        const char *_text_type,
        const char *_text_id,
        const char *_content);
    
    bool get_content(
        const char *_doc_id,
        const char *_lang_code,
        int _text_ver_no,
        const char *_text_type,
        const char *_text_id,
        string &_content);
    
    bool get_content(
        bool _check_empty,
        const char *_doc_id,
        const char *_lang_code,
        int _text_ver_no,
        const char *_text_type,
        const char *_text_id,
        string &_content,
        string &_source);

    bool get_content_multi( //syhwah 2013-03-18
        bool _check_empty,
        const char *_doc_id,
        const char *_lang_code,
        int _text_ver_no,
        const char *_text_type,
        const char *_text_id,
        set<string> &_content_set,
        string &_source);
    
    bool get_latest_date(
        string &_content_date,
        const char *_lang_code = NULL);
    
    bool update_text_id(
        const char *_current,
        const char *_new);
    
    bool has_text_id(
        const char *_text_id,
        const char *_lang_code = NULL);
    
    bool has_content(
        const char *_content,
        set<string> &_text_id_set);
    
    bool get_text_id(
        const char *_doc_id,
        const char *_lang_code,
        const char *_text_type,
        const char *_content,
        set<string> &_text_id);
};

#endif	/* TRANSDB_H */

