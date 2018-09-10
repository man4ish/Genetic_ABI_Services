/* 
 * File:   XmlConfig.h
 * Author: jameswong
 *
 * Created on June 18, 2012, 7:59 AM
 */

#ifndef XMLCONFIG_H
#define	XMLCONFIG_H

#include <stdexcept>
#include <string>
#include <map>
#include <set>

#include "XmlParse.h"

using namespace std;

struct Database
{
    string host;
    string user;
    string pswd;
};

class GSSConfig : public XmlParse
{
private:
    char *env_var;
    
    map<string, Database> database_map;
    map<string, Database>::iterator database_it;
    
    string qs_input_dir;
    string qs_output_dir;
    string qs_move_dir;
    
    unsigned int interval;
    string log_file;
    string latex_log_file;
    
    set<string> sample_id_prefix;
    set<string> filter_disease_names;
    
    size_t max_bar;
    double min_pct;
    bool cluster_target;
    
    bool use_lims;
    string testcode;
    
    bool print_pdf;
    bool delete_intermediate;
    
    bool check_missed_disease;
    int max_invalid_calls;
    double min_pct_valid_calls;
    
    string image_dir;
    set<string> lang_set;
    
    bool redirect_stdout;
    bool redirect_stderr;
    
    bool parse(
        const xercesc::DOMElement* _pa_elem,
        const xercesc::DOMElement* _ch_elem);
    
private:
    bool validate_input();
    
    bool is_all_digit(const char *_str) const;
    
public:
    GSSConfig();
    ~GSSConfig();
    bool init();
    void cleanup();
    bool reinit();
    const char *get_dbhost(const char *_dbname) const;
    const char *get_dbuser(const char *_dbname) const;
    const char *get_dbpswd(const char *_dbname) const;
    const char *get_qs_input_dir() const;
    const char *get_qs_output_dir() const;
    const char *get_qs_move_dir() const;
    unsigned int get_interval() const;
    const char *get_logfile() const;
    const char *get_latex_logfile() const;
    bool is_filter(const char *_sample_id) const;
    bool is_filtered_disdrug(const char *_disdrug) const;
    const set<string> *get_filter_disdrug() const;
    const size_t get_max_bar() const;
    const double get_min_pct() const;
    const bool get_cluster_target() const;
    bool get_use_lims() const;
    const char *get_testcode() const;
    bool get_print_pdf() const;
    bool get_delete_intermediate() const;
    bool get_check_missed_disease() const;
    int get_max_invalid_calls() const;
    double get_min_pct_valid_calls() const;
    const char *get_image_dir() const;
    void get_lang_code(set<string> &_lang_set) const;
    bool get_redirect_stdout() const;
    bool get_redirect_stderr() const;
};

#endif	/* XMLCONFIG_H */

