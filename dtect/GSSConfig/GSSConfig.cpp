/* 
 * File:   XmlConfig.cpp
 * Author: jameswong
 * 
 * Created on June 18, 2012, 7:59 AM
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <set>
#include <dirent.h>
#include <limits>
#include "GSSConfig.h"

GSSConfig::GSSConfig()
{
    this->init();
}

GSSConfig::~GSSConfig()
{
    this->cleanup();
}

bool GSSConfig::parse(
        const xercesc::DOMElement* _pa_elem,
        const xercesc::DOMElement* _ch_elem)
{
    if ((_ch_elem == NULL) || (_pa_elem == NULL))
        throw(runtime_error("Invalid DOM element"));

    try
    {
        const XMLCh *elem_tag = _ch_elem->getTagName();
        if (elem_tag == NULL)
            throw(runtime_error("Invalid element tag"));

        char *elem_tag_ch = this->transcode(elem_tag);
        assert(elem_tag_ch != NULL);
        
        if (strcmp(elem_tag_ch, "root") == 0)
        {   
        }
        else if (strcmp(elem_tag_ch, "database") == 0)
        {
        }
        else if (strcmp(elem_tag_ch, "input") == 0)
        {
        }
        else if (strcmp(elem_tag_ch, "output") == 0)
        {
        }
        else if (strcmp(elem_tag_ch, "dbname") == 0)
        {
            char *dbname = this->transcode(_ch_elem->getTextContent());
            this->database_it = this->database_map.find(dbname);
            if (this->database_it == this->database_map.end())
                this->database_it = this->database_map.insert(pair<string, Database>(dbname, Database())).first;
        }
        else if (strcmp(elem_tag_ch, "dbhost") == 0)
        {
            assert(this->database_it != this->database_map.end());
            this->database_it->second.host = this->transcode(_ch_elem->getTextContent());
        }
        else if (strcmp(elem_tag_ch, "dblogin") == 0)
        {
            assert(this->database_it != this->database_map.end());
            this->database_it->second.user = this->transcode(_ch_elem->getTextContent());
        }
        else if (strcmp(elem_tag_ch, "dbpasswd") == 0)
        {
            assert(this->database_it != this->database_map.end());
            this->database_it->second.pswd = this->transcode(_ch_elem->getTextContent());
        }
        else if (strcmp(elem_tag_ch, "qsinput") == 0)
        {
            this->qs_input_dir = this->transcode(_ch_elem->getTextContent());
            if (this->qs_input_dir.empty() == true)
                throw(runtime_error("qsinput not provided"));
        }
        else if (strcmp(elem_tag_ch, "qsoutput") == 0)
        {
            this->qs_output_dir = this->transcode(_ch_elem->getTextContent());
            if (this->qs_output_dir.empty() == true)
                throw(runtime_error("qsoutput not provided"));
        }
        else if (strcmp(elem_tag_ch, "qsmove") == 0)
        {
            this->qs_move_dir = this->transcode(_ch_elem->getTextContent());
            if (this->qs_move_dir.empty() == true)
                throw(runtime_error("qsmove not provided"));
        }
        else if (strcmp(elem_tag_ch, "interval") == 0)
        {
            this->interval = unsigned(atol(this->transcode(_ch_elem->getTextContent())));
            if (this->interval == 0)
                throw(runtime_error("'interval' should be more than 3 seconds"));
        }
        else if (strcmp(elem_tag_ch, "logfile") == 0)
        {
            this->log_file = this->transcode(_ch_elem->getTextContent());
            if (this->log_file.empty() == true)
                throw(runtime_error("logfile not provided"));
        }
        else if (strcmp(elem_tag_ch, "latex_logfile") == 0)
        {
            this->latex_log_file = this->transcode(_ch_elem->getTextContent());
            if (this->latex_log_file.empty() == true)
                throw(runtime_error("latex logfile not provided"));
        }
        else if (strcmp(elem_tag_ch, "filter") == 0)
        {
            if (this->sample_id_prefix.insert(this->transcode(_ch_elem->getTextContent())).second == false)
                throw(runtime_error("filter error"));
        }
        else if (strcmp(elem_tag_ch, "disdrug_filter") == 0)
        {
            char *disdrug = this->transcode(_ch_elem->getTextContent());
            if (disdrug != NULL)
                this->filter_disease_names.insert(disdrug);
        }
        else if (strcmp(elem_tag_ch, "max_bar") == 0)
        {
            this->max_bar = size_t(atol(this->transcode(_ch_elem->getTextContent())));
        }
        else if (strcmp(elem_tag_ch, "min_pct") == 0)
        {
            this->min_pct = double(atof(this->transcode(_ch_elem->getTextContent())));
            if ((this->min_pct < 0.0) || (this->min_pct > 100.0))
                throw(runtime_error("min_pct is out-of-range"));
        }
        else if (strcmp(elem_tag_ch, "cluster_target") == 0)
        {
            if (strcmp(this->transcode(_ch_elem->getTextContent()), "OFF") == 0)
                this->cluster_target = false;
            else if (strcmp(this->transcode(_ch_elem->getTextContent()), "ON") == 0)
                this->cluster_target = true;
            else
                throw(runtime_error("cluster_target not provided"));
        }
        else if (strcmp(elem_tag_ch, "lims") == 0)
        {
            if (strcmp(this->transcode(_ch_elem->getTextContent()), "ON") == 0)
                this->use_lims = true;
            else if (strcmp(this->transcode(_ch_elem->getTextContent()), "OFF") == 0)
                this->use_lims = false;
            else
                throw(runtime_error("invalid value of <lims></lims> provided"));
        }
        else if (strcmp(elem_tag_ch, "testcode") == 0)
        {
            this->testcode = this->transcode(_ch_elem->getTextContent());
            if (this->testcode.empty() == true)
                throw(runtime_error("testcode not provided"));
        }
        else if (strcmp(elem_tag_ch, "pdf") == 0)
        {
            if (strcmp(this->transcode(_ch_elem->getTextContent()), "OFF") == 0)
                this->print_pdf = false;
            else if (strcmp(this->transcode(_ch_elem->getTextContent()), "ON") == 0)
                this->print_pdf = true;
            else
                throw(runtime_error("pdf not provided"));
        }
        else if (strcmp(elem_tag_ch, "delete_intermediate") == 0)
        {
            if (strcmp(this->transcode(_ch_elem->getTextContent()), "OFF") == 0)
                this->delete_intermediate = false;
            else if (strcmp(this->transcode(_ch_elem->getTextContent()), "ON") == 0)
                this->delete_intermediate = true;
            else
                throw(runtime_error("pdf not provided"));
        }
        else if (strcmp(elem_tag_ch, "missed_disease") == 0)
        {
            if (strcmp(this->transcode(_ch_elem->getTextContent()), "OFF") == 0)
                this->check_missed_disease = false;
            else if (strcmp(this->transcode(_ch_elem->getTextContent()), "ON") == 0)
                this->check_missed_disease = true;
            else
                throw(runtime_error("invalid value for <missed_disease>"));
        }
        else if (strcmp(elem_tag_ch, "max_invalid") == 0)
        {
            this->max_invalid_calls = atoi(this->transcode(_ch_elem->getTextContent()));
        }
        else if (strcmp(elem_tag_ch, "min_valid") == 0)
        {
            this->min_pct_valid_calls = atof(this->transcode(_ch_elem->getTextContent()));
        }
        else if (strcmp(elem_tag_ch, "image_dir") == 0)
        {
            this->image_dir.assign(this->transcode(_ch_elem->getTextContent()));
            size_t loc, pos = 0;
            while ((loc = this->image_dir.find("//", pos)) != string::npos)
            {
                this->image_dir.replace(loc, 2, "/");
                pos = loc;
            }
            if (this->image_dir.find_last_of("/") != this->image_dir.length() - 1)
                this->image_dir.append("/");
        }
        else if (strcmp(elem_tag_ch, "lang_code") == 0)
        {
            this->lang_set.insert(this->transcode(_ch_elem->getTextContent()));
        }
        else if (strcmp(elem_tag_ch, "redirect_stdout") == 0)
        {
            if (strcmp(this->transcode(_ch_elem->getTextContent()), "ON") == 0)
                this->redirect_stdout = true;
            else if (strcmp(this->transcode(_ch_elem->getTextContent()), "OFF") == 0)
                this->redirect_stdout = false;
            else
                throw(runtime_error("invalid value for <redirect_stdout>"));
        }
        else if (strcmp(elem_tag_ch, "redirect_stderr") == 0)
        {
            if (strcmp(this->transcode(_ch_elem->getTextContent()), "ON") == 0)
                this->redirect_stderr = true;
            else if (strcmp(this->transcode(_ch_elem->getTextContent()), "OFF") == 0)
                this->redirect_stderr = false;
            else
                throw(runtime_error("invalid value for <redirect_stderr>"));
        }
        else
        {
            char error_message[1024] = {0};
            sprintf(error_message, "invalid tag <%s>", elem_tag_ch);
            throw(runtime_error(error_message));
            return false;
        }
    }
    catch(DOMException& _e)
    {
        char* message = this->transcode(_e.getMessage());
        fprintf(stderr, "\nXMLException caught: %s.\n", message);
        return false;
    }
    catch(XMLException& _e)
    {
        char* message = this->transcode(_e.getMessage());
        fprintf(stderr, "\nXMLException caught: %s.\n", message);
        return false;
    }
    catch(...)
    {
        fprintf(stderr, "\nUnknown exception caught.\n");
        return false;
    }
    return true;
}

bool GSSConfig::init()
{
    this->interval = 3;
    this->log_file.clear();
    this->latex_log_file.clear();
    this->max_bar = 5;
    this->min_pct = 3.0;
    this->cluster_target = true;
    this->testcode.clear();
    this->print_pdf = true;
    this->delete_intermediate = true;
    this->image_dir.clear();
    this->lang_set.insert("en");
    this->max_invalid_calls = 0;
    this->min_pct_valid_calls = numeric_limits<double>::min();
    this->redirect_stdout = false;
    this->redirect_stderr = false;
    
    this->env_var = getenv("GSSConfig");
    if (this->env_var == NULL)
    {
        fprintf(stderr, "\nERROR: GSSConfig environment variable not set.\n");
        exit(EXIT_FAILURE);
    }
    
    bool ret = this->parse_file(this->env_var);
    assert(ret == true);
    
    if (this->validate_input() == false)
        exit(EXIT_FAILURE);
    
    this->database_it = this->database_map.begin();
    return true;
}

void GSSConfig::cleanup()
{
    this->sample_id_prefix.clear();
    this->filter_disease_names.clear();
}

bool GSSConfig::reinit()
{
    this->cleanup();
    return this->init();
}

bool GSSConfig::validate_input()
{
    if (this->log_file.empty() == false)
    {
        FILE *test_file = fopen(this->log_file.c_str(), "a");
        if (test_file == NULL)
        {
            fprintf(stderr, "\nERROR: Invalid 'logfile' provided.\n");
            return false;
        }
        fclose(test_file);
    }
    if (this->latex_log_file.empty() == false)
    {
        FILE *test_file = fopen(this->latex_log_file.c_str(), "a");
        if (test_file == NULL)
        {
            fprintf(stderr, "\nERROR: Invalid 'latex_logfile' provided.\n");
            return false;
        }
        fclose(test_file);
    }
    if (this->image_dir.empty() == false)
    {
        DIR *dir = opendir(this->image_dir.c_str());
        if (dir == NULL)
        {
            fprintf(stderr, "\nERROR: Invalid directory '%s' provided.\n", this->image_dir.c_str());
            return false;
        }
        if (closedir(dir) == -1)
        {
            fprintf(stderr, "\nERROR: closedir\n");
            return false;
        }
    }
    return true;
}

bool GSSConfig::is_all_digit(const char* _str) const
{
    assert(_str != NULL);
    size_t len = strlen(_str);
    for (size_t i = 0; i < len; i++)
        if (isdigit(_str[i]) == false)
            return false;
    return true;
}

const char *GSSConfig::get_dbhost(const char* _dbname) const
{
    assert(_dbname != NULL);
    if (this->database_map.find(_dbname) == this->database_map.end())
    {
        fprintf(stderr, "\nERROR: %s is not a valid database in GSSConfig.xml.\n", _dbname);
        exit(EXIT_FAILURE);
    }
    return this->database_map.find(_dbname)->second.host.c_str();
}

const char *GSSConfig::get_dbuser(const char* _dbname) const
{
    assert(_dbname != NULL);
    if (this->database_map.find(_dbname) == this->database_map.end())
    {
        fprintf(stderr, "\nERROR: %s is not a valid database in GSSConfig.xml.\n", _dbname);
        exit(EXIT_FAILURE);
    }
    return this->database_map.find(_dbname)->second.user.c_str();
}

const char *GSSConfig::get_dbpswd(const char* _dbname) const
{
    assert(_dbname != NULL);
    if (this->database_map.find(_dbname) == this->database_map.end())
    {
        fprintf(stderr, "\nERROR: %s is not a valid database in GSSConfig.xml.\n", _dbname);
        exit(EXIT_FAILURE);
    }
    return this->database_map.find(_dbname)->second.pswd.c_str();
}

const char *GSSConfig::get_qs_input_dir() const
{
    return this->qs_input_dir.c_str();
}

const char *GSSConfig::get_qs_output_dir() const
{
    return this->qs_output_dir.c_str();
}

const char *GSSConfig::get_qs_move_dir() const
{
    return this->qs_move_dir.c_str();
}

unsigned int GSSConfig::get_interval() const
{
    return this->interval;
}

const char *GSSConfig::get_logfile() const
{
    return this->log_file.c_str();
}

const char *GSSConfig::get_latex_logfile() const
{
    return this->latex_log_file.c_str();
}

bool GSSConfig::is_filter(const char* _sample_id) const
{
    assert(_sample_id != NULL);
    set<string>::const_iterator const_it;
    for (const_it = this->sample_id_prefix.begin();
            const_it != this->sample_id_prefix.end();
            ++const_it)
    {
        if (strncmp(_sample_id, (*const_it).c_str(), (*const_it).length()) == 0)
            return true;
    }
    return false;
}

bool GSSConfig::is_filtered_disdrug(const char* _disdrug) const
{
    assert(_disdrug != NULL);
    if (this->filter_disease_names.find(_disdrug) != this->filter_disease_names.end())
        return true;
    return false;
}

const set<string> *GSSConfig::get_filter_disdrug() const
{
    return const_cast<const set<string>* >(&this->filter_disease_names);
}

const size_t GSSConfig::get_max_bar() const
{
    return this->max_bar;
}

const double GSSConfig::get_min_pct() const
{
    return this->min_pct;
}

const bool GSSConfig::get_cluster_target() const
{
    return this->cluster_target;
}

bool GSSConfig::get_use_lims() const
{
    return this->use_lims;
}

const char *GSSConfig::get_testcode() const
{
    return this->testcode.c_str();
}

bool GSSConfig::get_print_pdf() const
{
    return this->print_pdf;
}

bool GSSConfig::get_delete_intermediate() const
{
    return this->delete_intermediate;
}

bool GSSConfig::get_check_missed_disease() const
{
    return this->check_missed_disease;
}

int GSSConfig::get_max_invalid_calls() const
{
    return this->max_invalid_calls;
}

double GSSConfig::get_min_pct_valid_calls() const
{
    return this->min_pct_valid_calls;
}

const char *GSSConfig::get_image_dir() const
{
    return this->image_dir.c_str();
}

void GSSConfig::get_lang_code(set<string>& _lang_set) const
{
    _lang_set.clear();
    _lang_set = this->lang_set;
}

bool GSSConfig::get_redirect_stdout() const
{
    return this->redirect_stdout;
}

bool GSSConfig::get_redirect_stderr() const
{
    return this->redirect_stderr;
}