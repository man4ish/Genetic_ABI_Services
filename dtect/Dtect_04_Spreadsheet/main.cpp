/* 
 * File:   main.cpp
 * Author: wuntyng
 *
 * Created on June 22, 2012, 4:06 PM
 */

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <locale.h>
#include <wchar.h>
#include <libxml++/nodes/node.h>
#include <set>
#include <bits/stl_pair.h>
#include "atom_helper.h"
#include "util/string_utils.h"
#include "client/service.h"
#include "GSSDb.h"
#include "GSSConfig.h"
#include "TransDb.h"

using namespace std;
using gdata::util::StringUtils;
using gdata::AtomHelper;

static size_t rowParser(char *_source, const char _delimiter, vector<char*> &_fields_vec)
{
    _fields_vec.clear();
    _fields_vec.push_back(_source);
    
    while (*_source != '\0')
    {
        if (*_source == _delimiter)
        {
            *_source = '\0';
            _source++;
            _fields_vec.push_back(_source);
            continue;
        }
        _source++;
    }
    return _fields_vec.size();
}

static bool get_last_change(mysqlpp::sql_datetime &_last_change, const string _str)
{
    if (_str.empty())
        return false;
    char t_date[32] = {0};
    strcpy(t_date, _str.c_str());
    if (t_date[0] == '\0') return false;
    if (strlen(t_date) == 0) return false;
    
    vector<char*> date_fields;
    size_t f_cnt = rowParser(t_date, '-', date_fields);
    if (f_cnt != 3) return false;

    _last_change.year((unsigned short)(atoi(date_fields[0])));
    _last_change.month((unsigned char)(atoi(date_fields[1])));
    _last_change.day((unsigned char)(atoi(date_fields[2])));

    vector<char*> time_fields;
    f_cnt = rowParser(date_fields[2], ':', time_fields);
    if (f_cnt != 3) return false;
    char *hour = time_fields[0];
    while (isalpha(*hour) == false)
        hour++;
    hour++;
    if (hour[0] == '\0') return false;
    _last_change.hour((unsigned char)(atoi(hour) + 8));
    _last_change.minute((unsigned char)(atoi(time_fields[1])));
    _last_change.second((unsigned char)(atoi(time_fields[2])));
    
    return true;
}

static bool filter_special_character(string &_target, const string _s_str)
{
    bool erase = false;
    size_t pos;
    while ((pos = _target.find(_s_str)) != string::npos)
    {
        _target.erase(pos, _s_str.length());
        erase = true;
    }
    return erase;
}

static bool parse_chip_id(mysqlpp::sql_char &_chip_type, mysqlpp::sql_char &_chip_ver, const string _str)
{
    _chip_type.clear();
    _chip_ver.clear();
    if (_str.empty()) return false;
    char title[64] = {0};
    strcpy(title, _str.c_str());
    vector<char*> title_fields;
    size_t c_cnt = rowParser(title, '_', title_fields);
    if (c_cnt != 3) return false;
    _chip_type = title_fields[1];
    _chip_ver = title_fields[2];
    if (_chip_type.empty()) return false;
    if (_chip_ver.empty()) return false;
    return true;
}

static bool parse_content(map<string, string> &_cont_map, const string _str)
{
    if (_str.empty()) return false;
    vector<char*> fields;
    char *t_str = new char[_str.length() + 1];
    strcpy(t_str, _str.c_str());
    if (t_str[0] == '\0') return false;
    if (strlen(t_str) == 0) return false;
    size_t f_cnt = rowParser(t_str, ',', fields);
    for (size_t i = 0; i < f_cnt; i++)
    {
        vector<char*> sub_fields;
        size_t s_cnt = rowParser(fields[i], ':', sub_fields);
        if (s_cnt != 2) return false;
        
        char *lang = sub_fields[0];
        while (isspace(*lang)) ++lang;
        char *value = sub_fields[1];
        while (isspace(*value)) ++value;
        
        _cont_map[lang] = value;
    }
    
    delete [] t_str;
    return true;
}

static bool parse_column_header(set<string> &_col_hdr_map, const string _str)
{
    if (_str.empty()) return false;
    AtomHelper h_atom;
    h_atom.Parse(_str);
    xmlpp::NodeSet h_entries = h_atom.Entries();
    for (size_t i = 0; i < h_entries.size(); ++i)
        if (_col_hdr_map.insert(h_atom.Content(h_entries[i])).second == false)
            return false;
    return true;
}

static bool parse_column_header(vector<string> &_col_hdr_vec, const string _str)
{
    if (_str.empty()) return false;
    AtomHelper h_atom;
    h_atom.Parse(_str);
    xmlpp::NodeSet h_entries = h_atom.Entries();
    for (size_t i = 0; i < h_entries.size(); ++i)
        _col_hdr_vec.push_back(h_atom.Content(h_entries[i]));
    return true;
}

static void latex_markup(string &_str)
{
    // >  : &gt;
    // <  : &lt;
    // %  : \%
    // '  : &apos;
    // bullet point
    // \newline (paragraph)
    bool has_begin = false;
    bool has_end = false;
    if (_str.empty() == false)
    {
        for (size_t i = 0; i < _str.length(); ++i)
        {
            if ((_str[i - 1] == '\n') && (_str[i] == '*') && (_str[i + 1] == ' '))
            {
                if(has_begin == false)
                {
                    _str.erase(i, 1);
                    _str.insert(i, "\\begin{itemize}\\item");
                    has_begin = true;
                    has_end = true;
                }
                else
                {
                    _str.erase(i, 1);
                    _str.insert(i, "\\item");
                }
            }
        }
        if(has_end == true)
            _str.append("\\end{itemize}");
    }
}

static bool parse_disease_drug(
        GSSDb &_gssdb,
        TransDb &_transdb,
        mysqlpp::sql_int_unsigned _chip_key,
        const string _chip_type,
        vector<string> &dis_drug_vec,
        set<string> &_col_hdr_set,
        string _str)
{
    if (_str.empty()) return false;
    if (_col_hdr_set.empty()) return false;
    AtomHelper ws;
    ws.Parse(_str);
    xmlpp::NodeSet ws_entries = ws.Entries();

    for (size_t i = 0; i < ws_entries.size(); ++i)
    {
        dis_drug_vec.push_back(ws.Title(ws_entries[i]));
        
        set<string>::const_iterator s_it;
        for (s_it = _col_hdr_set.begin(); s_it != _col_hdr_set.end(); ++s_it)
        {
            string str;
            str = ws.Lang(ws_entries[i], "en");
            assert(str.empty() == false);
            
            // Obtain text id from TransDb based on en text
            char doc_id[32] = {0};
            sprintf(doc_id, "GS-%s", _chip_type.c_str());
            set<string> text_id;
            bool ret = _transdb.get_text_id(doc_id, "en", "DIS_DRUG_NAME", str.c_str(), text_id);
            assert(ret == true);
            assert(text_id.size() == 1);
            
            if (*s_it == "en")
                continue;
            
            if ((str = ws.Lang(ws_entries[i], *s_it)).empty() == false)
            {
                fprintf(stderr, "\n%s  %s  %s", (*s_it).c_str(), (*text_id.begin()).c_str(), str.c_str());
                bool is_filter = filter_special_character(str, "\u200B");
                if (is_filter) { assert(*s_it != "en"); }
                
                ret = _transdb.insert_content(doc_id, (*s_it).c_str(), "1", "DIS_DRUG_NAME", (*text_id.begin()).c_str(), str.c_str());
                assert(ret == true);
            }
        }
    }
    return true;
}

static bool parse_description(
        GSSDb &_gssdb,
        TransDb &_transdb,
        mysqlpp::sql_int_unsigned _chip_key,
        const string chip_type,
        vector<string> &_dis_drug_vec,
        set<string> &_col_hdr_set,
        string _str)
{
    if (_str.empty()) return false;
    if (_dis_drug_vec.empty()) return false;
    AtomHelper ws;
    ws.Parse(_str);
    xmlpp::NodeSet ws_entries = ws.Entries();
    
    assert(_dis_drug_vec.size() == ws_entries.size());
    for (size_t i = 0; i < ws_entries.size(); i++)
    {
        char doc_id[32] = {0};
        sprintf(doc_id, "GS-%s", chip_type.c_str());
        set<string> text_id;
        bool ret = _transdb.get_text_id(doc_id, "en", "DIS_DRUG_NAME", _dis_drug_vec[i].c_str(), text_id);
        assert(ret == true);
        assert(text_id.size() == 1);
        
        set<string>::const_iterator s_it;
        for (s_it = _col_hdr_set.begin(); s_it != _col_hdr_set.end(); ++s_it)
        {
            string str;
            if ((str = ws.Lang(ws_entries[i], *s_it)).empty() == false)
            {
                bool is_filter = filter_special_character(str, "\u200B");
                if (is_filter) { assert(*s_it != "en"); }
                
                char DocID[64] = {0};
                sprintf(DocID, "GS-%s-DESC", chip_type.c_str());
                
                char temp_desc[32] = {0};
                strncpy(temp_desc, str.c_str(), 31);
                fprintf(stderr, "\n%s  %s  %s", (*s_it).c_str(), (*text_id.begin()).c_str(), temp_desc);
                
                ret = _transdb.insert_content(DocID, (*s_it).c_str(), "1", "DESC", (*text_id.begin()).c_str(), str.c_str());
                assert(ret == true);
            }
        }
    }
    return true;
}

static bool parse_effect(
        GSSDb &_gssdb,
        TransDb &_transdb,
        mysqlpp::sql_int_unsigned _chip_sk,
        const string _chip_type,
        set<string> &_col_hdr_set,
        string _str)
{
    if (_str.empty()) return false;
    if (_col_hdr_set.empty()) return false;
    if (_col_hdr_set.find("en") == _col_hdr_set.end()) return false;
    AtomHelper ws;
    ws.Parse(_str);
    xmlpp::NodeSet ws_entries = ws.Entries();
    for (size_t i = 0; i < ws_entries.size(); i++)
    {
        set<string>::const_iterator s_it;
        map<string, string> lang_map;
        map<string, string>::iterator lang_it;
        for (s_it = _col_hdr_set.begin(); s_it != _col_hdr_set.end(); ++s_it)
        {
            string str;
            if ((str = ws.Lang(ws_entries[i], *s_it)).empty() == false)
                lang_map[*s_it] = str;
        }
        assert(lang_map.find("en") != lang_map.end());
        
        char doc_id[32] = {0};
        sprintf(doc_id, "GS-%s", _chip_type.c_str());
        set<string> text_id;
        bool ret = _transdb.get_text_id(doc_id, "en", "GENOTYPIC_EFFECT", lang_map.find("en")->second.c_str(), text_id);
        if (ret == false)
        {
            fprintf(stderr, "\n%s\n", lang_map.find("en")->second.c_str());
            continue;
        }
        assert(ret == true);
        assert(text_id.empty() == false);
        
        for (lang_it = lang_map.begin(); lang_it != lang_map.end(); ++lang_it)
        {
            if (lang_it->second.empty())
                continue;
            if (lang_it->first == "en")
                continue;
            if (lang_it->second.length() < 4)
                continue;

            bool is_filter = filter_special_character(lang_it->second, "\u200B");
            if (is_filter) { assert(lang_it->first != "en"); }
            
            for (set<string>::iterator t_it = text_id.begin(); t_it != text_id.end(); ++t_it)
            {
                fprintf(stderr, "\n%s  %s  %s", lang_it->first.c_str(), (*t_it).c_str(), lang_it->second.c_str());
                
                ret = _transdb.insert_content(doc_id, lang_it->first.c_str(), "1", "GENOTYPIC_EFFECT", (*t_it).c_str(), lang_it->second.c_str());
                assert(ret == true);
//                if (_transdb.has_text_id((*text_id_it).c_str(), lang_it->first.c_str()) == false)
//                {
//                    fprintf(stderr, "%s\n", (*text_id_it).c_str());
//                    fprintf(stderr, "\n%s\n%s\n", lang_it->first.c_str(), lang_it->second.c_str());
//                    
//                    char docId[32] = {0};
//                    sprintf(docId, "GS-%s", _chip_type.c_str());
//                    bool ret = _transdb.insert_content(docId, lang_it->first.c_str(), "1", "GENOTYPIC_EFFECT", (*text_id_it).c_str(), lang_it->second.c_str());
//                    if (ret == false)
//                        fprintf(stderr, "\n");
//                    assert(ret == true);
//                }
            }
        }
    }
    return true;
}

//static bool parse_static(
//        GSSDb &_gssdb,
//        vector<string> &_col_hdr_vec,
//        string _str)
//{
//    if (_str.empty()) return false;
//    if (_col_hdr_vec.empty()) return false;
//    AtomHelper ws;
//    ws.Parse(_str);
//    xmlpp::NodeSet ws_entries = ws.Entries();
//    for (size_t i = 0; i < ws_entries.size(); ++i)
//    {
//        mysqlpp::sql_varchar last_author = ws.Author(ws_entries[i]);
//        if (last_author.empty()) return false;
//        mysqlpp::sql_varchar author_email = ws.AuthorEmail(ws_entries[i]);
//        if (author_email.empty()) return false;
//        mysqlpp::sql_datetime last_change;
//        if (get_last_change(last_change, ws.Updated(ws_entries[i])) == false) return false;
//        
//        size_t vec_size = _col_hdr_vec.size();
//        
//        mysqlpp::sql_varchar key;
//        for (size_t v = 0; v < vec_size; v++)
//        {
//            string str;
//            if ((str = ws.Lang(ws_entries[i], _col_hdr_vec[v].c_str())).empty() == false)
//            {
//                if (strcmp(_col_hdr_vec[v].c_str(), "key") == 0)
//                {
//                    key = str;
//                    continue;
//                }
//                
//                assert(key.empty() == false);
//                bool is_filter = filter_special_character(str, "\u200B");
//                if (is_filter) { assert(_col_hdr_vec[v] != "en"); }
//                bool ret = _gssdb.insert_static(_col_hdr_vec[v], key, str, last_change, last_author, author_email);
//                assert(ret == true);
//            }
//        }
//    }
//    return true;
//}

int main(int argc, char *argv[])
{
    GSSConfig xml_config;
    GSSDb gss_db("GSSDb");
    gss_db.init(
            xml_config.get_dbhost(gss_db.get_dbname()),
            xml_config.get_dbuser(gss_db.get_dbname()),
            xml_config.get_dbpswd(gss_db.get_dbname()));
    
    TransDb trans_db("TRANS_DB");
    trans_db.init(
            xml_config.get_dbhost(trans_db.get_dbname()),
            xml_config.get_dbuser(trans_db.get_dbname()),
            xml_config.get_dbpswd(trans_db.get_dbname()));
    
    string get_resp;
    AtomHelper atom;
    xmlpp::NodeSet entries;
    
    
    // Login to Google
    gdata::client::Service service("wise", "mgrc-gsstrans-0.1");
    service.ClientLogin("gss@neuramatix.com", "neuramatix");
    
    
    // GET list of spreadsheet
    get_resp = service.HttpRequest("GET", "https://spreadsheet.google.com/feeds/spreadsheets/private/full");
    atom.Parse(get_resp);
    entries = atom.Entries();
    
    
    cerr << "Available spreadsheets:" << endl;
    for (size_t i = 0; i < entries.size(); i++)
    {
        if (strcmp(atom.Title(entries[i]).c_str(), "DTect_static") == 0)
        {
            get_resp = service.HttpRequest("GET", atom.ContentSrc(entries[i]));
            AtomHelper ws_atom;
            ws_atom.Parse(get_resp);
            xmlpp::NodeSet spreadsheet_entries = ws_atom.Entries();
            assert(spreadsheet_entries.size() == 1);
            for (size_t static_i = 0; static_i < spreadsheet_entries.size(); ++static_i)
            {
                fprintf(stderr, "%s\n", ws_atom.Id(spreadsheet_entries[static_i]).c_str());
                fprintf(stderr, "%s\n", ws_atom.Updated(spreadsheet_entries[static_i]).c_str());
                fprintf(stderr, "%s\n", ws_atom.ContentSrc(spreadsheet_entries[static_i]).c_str());
                fprintf(stderr, "%s\n", ws_atom.CellFeed(spreadsheet_entries[static_i]).c_str());
                
                vector<string> col_hdr_vec;
                bool ret = parse_column_header(col_hdr_vec, service.HttpRequest("GET", ws_atom.CellFeed(spreadsheet_entries[static_i]) + "?min-row=1&max-row=1"));
                assert(ret == true);
                assert(col_hdr_vec.empty() == false);
                
                string ws_resp = service.HttpRequest("GET", ws_atom.ContentSrc(spreadsheet_entries[static_i]));
//                ret = parse_static(gss_db, col_hdr_vec, ws_resp);
//                assert(ret == true);
            }
            continue;
        }
        if (strstr(atom.Title(entries[i]).c_str(), "DTect") == NULL)
            continue;
        
        // Parse for CHIP TYPE and CHIP VER and obtain ChipSK from GSSDb
        mysqlpp::sql_char chip_type;
        mysqlpp::sql_char chip_ver;
        mysqlpp::sql_int_unsigned chip_sk = 0;
        bool ret = parse_chip_id(chip_type, chip_ver, atom.Title(entries[i]));
        assert(ret == true);
        
        ret = gss_db.get_chip_key(chip_sk, chip_type, chip_ver);
        assert(ret == true);
        assert(chip_sk != 0);
        
        fprintf(stderr, "\n");
        fprintf(stderr, "%-5lu %s\n", i + 1, atom.Title(entries[i]).c_str());
        fprintf(stderr, "      %s\n", atom.Id(entries[i]).c_str());
        fprintf(stderr, "      %s\n", atom.Updated(entries[i]).c_str());
        fprintf(stderr, "      %s\n", atom.ContentSrc(entries[i]).c_str());
        fprintf(stderr, "\n");
    
    
        // GET list of worksheets
        get_resp = service.HttpRequest("GET", atom.ContentSrc(entries[i]));

        AtomHelper ws_atom;
        ws_atom.Parse(get_resp);
        
        xmlpp::NodeSet spreadsheet_entries = ws_atom.Entries();
        fprintf(stderr, "        Available worksheets:\n");
        
        int process_seq = 0;
        vector<string> dis_drug_vec;
        while (process_seq < 3)
        {
            size_t idx = 0;
            string ws_resp;
            set<string> col_hdr_set;
            switch(process_seq)
            {
                case 0:
                    while (ws_atom.Title(spreadsheet_entries[idx]) != "Diseases_Drugs")
                        idx++;
                    fprintf(stderr, "\n");
                    fprintf(stderr, "     %-5lu %s\n", idx + 1, ws_atom.Title(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.Id(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.Updated(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.ContentSrc(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.CellFeed(spreadsheet_entries[idx]).c_str());
                    
                    ret = parse_column_header(col_hdr_set, service.HttpRequest("GET", ws_atom.CellFeed(spreadsheet_entries[idx]) + "?min-row=1&max-row=1"));
                    assert(ret == true);
                    assert(col_hdr_set.empty() == false);
                    
                    ws_resp = service.HttpRequest("GET", ws_atom.ContentSrc(spreadsheet_entries[idx]));
                    ret = parse_disease_drug(gss_db, trans_db, chip_sk, chip_type, dis_drug_vec, col_hdr_set, ws_resp);
                    assert(ret == true);
                    
                    process_seq++;
                    break;
                case 1:
                    if (dis_drug_vec.empty())
                    {
                        process_seq = 10;
                        break;
                    }
                    while (ws_atom.Title(spreadsheet_entries[idx]) != "Descriptions")
                        idx++;
                    fprintf(stderr, "\n");
                    fprintf(stderr, "     %-5lu %s\n", idx + 1, ws_atom.Title(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.Id(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.Updated(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.ContentSrc(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.CellFeed(spreadsheet_entries[idx]).c_str());
                    
                    ret = parse_column_header(col_hdr_set, service.HttpRequest("GET", ws_atom.CellFeed(spreadsheet_entries[idx]) + "?min-row=1&max-row=1"));
                    assert(ret == true);
                    assert(col_hdr_set.empty() == false);
                    
                    ws_resp = service.HttpRequest("GET", ws_atom.ContentSrc(spreadsheet_entries[idx]));
                    ret = parse_description(gss_db, trans_db, chip_sk, chip_type, dis_drug_vec, col_hdr_set, ws_resp);
                    assert(ret == true);
                    process_seq++;
                    break;
                case 2:
                    while (ws_atom.Title(spreadsheet_entries[idx]) != "Effects")
                        idx++;
                    ret = parse_column_header(col_hdr_set, service.HttpRequest("GET", ws_atom.CellFeed(spreadsheet_entries[idx]) + "?min-row=1&max-row=1"));
                    assert(ret == true);
                    assert(col_hdr_set.empty() == false);
                    
                    fprintf(stderr, "\n");
                    fprintf(stderr, "     %-5lu %s\n", idx + 1, ws_atom.Title(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.Id(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.Updated(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.ContentSrc(spreadsheet_entries[idx]).c_str());
                    fprintf(stderr, "           %s\n", ws_atom.CellFeed(spreadsheet_entries[idx]).c_str());
                    
                    ws_resp = service.HttpRequest("GET", ws_atom.ContentSrc(spreadsheet_entries[idx]));
                    ret = parse_effect(gss_db, trans_db, chip_sk, chip_type, col_hdr_set, ws_resp);
                    assert(ret == true);
                    
                    process_seq++;
                    break;
            }
        }
//        for (size_t i = 0; i < spreadsheet_entries.size(); ++i)
//        {
//            fprintf(stderr, "     %-5lu %s\n", i + 1, ws_atom.Title(spreadsheet_entries[i]).c_str());
//            fprintf(stderr, "           %s\n", ws_atom.Id(spreadsheet_entries[i]).c_str());
//            fprintf(stderr, "           %s\n", ws_atom.Updated(spreadsheet_entries[i]).c_str());
//            fprintf(stderr, "\n");
//            
//            vector<string> dis_drug_vec;
//            if (ws_atom.Title(spreadsheet_entries[i]) == "Diseases_Drugs")
//            {
//                // GET worksheet contents
//                string ws_resp = service.HttpRequest("GET", ws_atom.ContentSrc(spreadsheet_entries[i]));
//                ret = parse_disease_drug(gss_db, chip_sk, dis_drug_vec, ws_resp);
//                assert(ret == true);
//            }
//        }
    }
    return 0;
}
