/* 
 * File:   TransDb.cpp
 * Author: jameswong
 * 
 * Created on December 31, 2012, 4:10 PM
 */

#include <vector>

#include "TransDb.h"

TransDb::TransDb(const char* _dbname) : DbBase(_dbname)
{
}

TransDb::~TransDb()
{
    this->disconnect();
}

bool TransDb::init(
        const char* _dbhost,
        const char* _dbuser,
        const char* _dbpswd)
{
    bool ret = this->connect(_dbhost, _dbuser, _dbpswd, false);
    assert(ret == true);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SET NAMES utf8";
    ret = this->query_execute(qry);
    assert(ret == true);
    return ret;
}

bool TransDb::insert_text_version(
        const char* _doc_id,
        const char* _lang_code,
        int _text_ver_no,
        int lock_text)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO TextVersion "
            "SET "
            "TextVersion.DocID=" << mysqlpp::quote << _doc_id << ","
            "TextVersion.LanguageCode=" << mysqlpp::quote << _lang_code << ","
            "TextVersion.TextVerNo=" << mysqlpp::quote << _text_ver_no << ","
            "TextVersion.LockText=" << mysqlpp::quote << lock_text;
    return this->query_execute(qry);
}

bool TransDb::insert_document(
        const char* _doc_id,
        const char* _doc_desc,
        int _doc_type_idx)
{
    assert(_doc_id != NULL);
    assert(_doc_desc != NULL);
    assert(_doc_type_idx != 0);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO Document "
            "VALUES ("
            << mysqlpp::quote << _doc_id << ","
            << mysqlpp::quote << _doc_desc << ","
            << mysqlpp::quote << _doc_type_idx << ")";
    return this->query_execute(qry);
}

bool TransDb::insert_content(
        const char* _doc_id,
        const char* _lang_code,
        const char* _text_ver_no,
        const char* _text_type,
        const char* _text_id,
        const char* _content)
{
    assert(_doc_id != NULL);
    assert(_lang_code != NULL);
    assert(_text_ver_no != NULL);
    assert(_text_type != NULL);
    assert(_text_id != NULL);
    assert(_content != NULL);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "INSERT IGNORE "
            "INTO TextTranslate "
            "SET "
            "TextTranslate.DocID=" << mysqlpp::quote << _doc_id << ","
            "TextTranslate.LanguageCode=" << mysqlpp::quote << _lang_code << ","
            "TextTranslate.TextVerNo=" << mysqlpp::quote << _text_ver_no << ","
            "TextTranslate.TextType=" << mysqlpp::quote << _text_type << ","
            "TextTranslate.TextID=" << mysqlpp::quote << _text_id << ","
            "TextTranslate.Content=" << mysqlpp::quote << _content << ","
            "TextTranslate.Source=" << mysqlpp::quote << "DT" << ","
            "TextTranslate.TranslateText=" << mysqlpp::quote << "1" << ","
            "TextTranslate.EditDateTime=NOW()";
    return this->query_execute(qry);
}

bool TransDb::get_content(
        const char* _doc_id,
        const char* _lang_code,
        int _text_ver_no,
        const char* _text_type,
        const char* _text_id,
        string& _content)
{
    assert(_doc_id != NULL);
    assert(_lang_code != NULL);
    assert(_text_ver_no != 0);
    assert(_text_type != NULL);
    assert(_text_id != NULL);
    _content.clear();
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT "
            "TextTranslate.Content, "
            "TextTranslate.LanguageCode, "
            "TextTranslate.TranslateText "
            "FROM TextTranslate "
            "WHERE "
            "TextTranslate.DocID=" << mysqlpp::quote << _doc_id << " AND "
            "TextTranslate.TextVerNo=" << mysqlpp::quote << _text_ver_no << " AND "
            "TextTranslate.TextType=" << mysqlpp::quote << _text_type << " AND "
            "TextTranslate.TextID=" << mysqlpp::quote << _text_id;
/*
if (strcmp(_text_id,"female")== 0 || strcmp(_text_id,"male")== 0){
    cout << "------------------------------------------------------------" << endl;
    cout << qry << endl;
    cout << "------------------------------------------------------------" << endl;
}
*/
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    
    if (str_res.empty()) return false;
    
    for (size_t i = 0; i < str_res.size(); ++i)
    {
        if (str_res[i]["LanguageCode"] == _lang_code)
        {
            _content = str_res[i]["Content"].c_str();
            return true;
        }
    }
    for (size_t i = 0; i < str_res.size(); ++i)
    {
        if (atoi(str_res[i]["TranslateText"].c_str()) == 0)
        {
            _content = str_res[i]["Content"].c_str();
            return true;
        }
    }

    //Get the english text contents if all criteria are not met -syhwah 20130312
    for (size_t i = 0; i < str_res.size(); ++i)
    {
        if (str_res[i]["LanguageCode"] == "en")
        {
            _content = str_res[i]["Content"].c_str();
            return true;
        }
    }

    return false;
}


bool TransDb::get_content_multi( //syhwah 2013-03-18
        bool _check_empty,
        const char *_doc_id,
        const char *_lang_code,
        int _text_ver_no,
        const char *_text_type,
        const char *_text_id,
        set<string> &_content_set,
        string &_source)
{
    assert(_doc_id != NULL);
    assert(_lang_code != NULL);
    assert(_text_ver_no != 0);
    assert(_text_type != NULL);
    assert(_text_id != NULL);

    char *tmp_text_id = new char[strlen(_text_id)+2];

    sprintf(tmp_text_id,"%s%%",_text_id); 

    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT "
            "TextTranslate.Content, "
            "TextTranslate.LanguageCode, "
            "TextTranslate.TranslateText "
            "FROM TextTranslate "
            "WHERE "
            "TextTranslate.DocID=" << mysqlpp::quote << _doc_id << " AND "
            "TextTranslate.TextVerNo=" << mysqlpp::quote << _text_ver_no << " AND "
            "TextTranslate.TextType=" << mysqlpp::quote << _text_type << " AND "
            "TextTranslate.TextID like " << mysqlpp::quote << tmp_text_id;

    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);

    if (ret == false) { return false; }

    if (str_res.empty()){
        if (_check_empty) return false;
        return true;
    }

    for (size_t i = 0; i < str_res.size(); ++i){
        if (str_res[i]["LanguageCode"] == _lang_code){
            _content_set.insert(str_res[i]["Content"].c_str());
        }
    }

    if (_content_set.size() > 0 ) { return true; }
    
    for (size_t i = 0; i < str_res.size(); ++i){
        if (atoi(str_res[i]["TranslateText"].c_str()) == 0){
           _content_set.insert(str_res[i]["Content"].c_str());
        }
    }


    if (_content_set.size() > 0 ) { return true; }
 
    //Get the english text contents if all criteria are not met -syhwah 20130312    
    for (size_t i = 0; i < str_res.size(); ++i){
        if (str_res[i]["LanguageCode"] == "en"){
            _content_set.insert(str_res[i]["Content"].c_str());            
        }        
    }
    
    if (_content_set.size() > 0 ) { return true; } else { return false; }
}

bool TransDb::get_content(
        bool _check_empty,
        const char* _doc_id,
        const char* _lang_code,
        int _text_ver_no,
        const char* _text_type,
        const char* _text_id,
        string& _content,
        string& _source)
{
    assert(_doc_id != NULL);
    assert(_lang_code != NULL);
    assert(_text_ver_no != 0);
    assert(_text_type != NULL);
    assert(_text_id != 0);
    _content.clear();
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT "
            "TextTranslate.Content, "
            "TextTranslate.Source, "
            "TextTranslate.LanguageCode,"
            "TextTranslate.TranslateText "
            "FROM TextTranslate "
            "WHERE "
            "TextTranslate.DocID=" << mysqlpp::quote << _doc_id << " AND "
            "TextTranslate.TextVerNo=" << mysqlpp::quote << _text_ver_no << " AND "
            "TextTranslate.TextType=" << mysqlpp::quote << _text_type << " AND "
            "TextTranslate.TextID=" << mysqlpp::quote << _text_id;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    if (str_res.empty())
    {
        if (_check_empty) return false;
        return true;
    }
    
    bool match_found = false;
    for (size_t i = 0; i < str_res.size(); ++i)
    {
        if (_source.empty() == false)
        {
            if ((str_res[i]["LanguageCode"] == _lang_code) && (str_res[i]["Source"] == _source))
            {
                assert(match_found == false);
                match_found = true;
                _content = str_res[i]["Content"].c_str();
            }
        }
        else
        {
            if (str_res[i]["LanguageCode"] == _lang_code)
            {
                assert(match_found == false);
                match_found = true;
                _content = str_res[i]["Content"].c_str();
                _source = str_res[i]["Source"].c_str();
            }
        }
    }
    return true;
}

bool TransDb::get_latest_date(
        string &_content_date,
        const char* _lang_code)
{
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT "
            "MAX(DATE(TextTranslate.EditDateTime)) "
            "FROM TextTranslate ";
    if (_lang_code != NULL)
        qry << "WHERE TextTranslate.LanguageCode=" << mysqlpp::quote << _lang_code;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    if (str_res.size() != 1) return false;
    _content_date.assign(str_res[0][0].c_str());
    return ret;
}

bool TransDb::update_text_id(
        const char* _current,
        const char* _new)
{
    assert(_current != NULL);
    assert(_new != NULL);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "UPDATE TextTranslate "
            "SET TextTranslate.TextID=" << mysqlpp::quote << _new << " "
            "WHERE "
            "TextTranslate.TextID=" << mysqlpp::quote << _current;
    return this->query_execute(qry);
}

bool TransDb::has_text_id(
        const char* _text_id,
        const char* _lang_code)
{
    assert(_text_id != NULL);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT * "
            "FROM TextTranslate "
            "WHERE TextTranslate.TextID=" << mysqlpp::quote << _text_id;
    
    if (_lang_code != NULL)
        qry << " AND TextTranslate.LanguageCode=" << mysqlpp::quote << _lang_code;
    
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    assert(ret == true);
    if (str_res.empty())
        return false;
//    this->print(stderr, &str_res);
    return true;
}

bool TransDb::has_content(
        const char* _content,
        set<string>& _text_id_set)
{
    assert(_content != NULL);
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT * "
            "FROM TextTranslate "
            "WHERE TextTranslate.Content=" << mysqlpp::quote << _content;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    if (str_res.empty()) return false;
    
//    this->print(stderr, &str_res);
    
    for (size_t i = 0; i < str_res.size(); ++i)
    {
        if (strstr(str_res[i]["DocID"].c_str(), "GS") != NULL)
            _text_id_set.insert(str_res[i]["TextID"].c_str());
    }
    
    return true;
}

bool TransDb::get_text_id(
        const char* _doc_id,
        const char* _lang_code,
        const char* _text_type,
        const char* _content,
        set<string> &_text_id)
{
    assert(_doc_id != NULL);
    assert(_lang_code != NULL);
    assert(_text_type != NULL);
    assert(_content != NULL);
    
    mysqlpp::Query qry = this->db_conn.query();
    qry << "SELECT DISTINCT TextTranslate.TextID "
            "FROM TextTranslate "
            "WHERE "
            "TextTranslate.DocID=" << mysqlpp::quote << _doc_id << " AND "
            "TextTranslate.LanguageCode=" << mysqlpp::quote << _lang_code << " AND "
            "TextTranslate.TextType=" << mysqlpp::quote << _text_type << " AND "
            "TextTranslate.Content=" << mysqlpp::quote << _content;
    mysqlpp::StoreQueryResult str_res;
    bool ret = this->query_store(qry, str_res);
    if (ret == false) return false;
    if (str_res.empty()) return false;
    
    for (size_t i = 0; i < str_res.size(); i++)
    {
        if ((strstr(str_res[i][0].c_str(), "Disease") != NULL) ||
            (strstr(str_res[i][0].c_str(), "Drug") != NULL) ||
            (strstr(str_res[i][0].c_str(), "Trait") != NULL) ||
            (strstr(str_res[i][0].c_str(), "EFF") != NULL))
            continue;
        _text_id.insert(str_res[i][0].c_str());
    }
    return ret;
}
