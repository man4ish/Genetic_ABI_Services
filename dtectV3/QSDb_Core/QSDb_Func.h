/* 
 * File:   QSDb_Func.h
 * Author: jameswong
 *
 * Created on June 5, 2012, 8:13 AM
 */

#ifndef QSDB_FUNC_H
#define	QSDB_FUNC_H

#include <vector>
#include <sys/stat.h>
#include <errno.h>

using namespace std;

static inline size_t filesize(FILE *f)
{
    struct stat st;
    if (fstat(fileno(f), &st) == -1)
        return 0;
    return (size_t)st.st_size;
}

static inline char complement(char src)
{
    char dst;
    switch (src)
    {
        case 'a':
        case 'A':
            dst = 'T';
            break;
        case 'c':
        case 'C':
            dst = 'G';
            break;
        case 'g':
        case 'G':
            dst = 'C';
            break;
        case 't':
        case 'T':
            dst = 'A';
            break;
        default:
            break;
    }
    return dst;
}

static inline size_t chomp(char *_string)
{
    size_t len = strlen(_string);
    while (_string[len - 1] == '\n')
        _string[--len] = '\0';
    while (_string[len - 1] == '\r')
        _string[--len] = '\0';
    return len;
}

static inline void word_clean_up(char *_string)
{
    assert(_string != NULL);
    size_t len = strlen(_string);
    size_t read_idx = 0;
    size_t write_idx = 0;
    
    while (read_idx < len)
    {
        if (isalnum(_string[read_idx]))
            _string[write_idx++] = _string[read_idx];
        read_idx++;
    }
    _string[write_idx] = '\0';
}

inline size_t rowParser(char *_source, const char *_delimiter, char **_fields, size_t _max)
{
    size_t index = 0;
    char *pch = strtok(_source, _delimiter);
    while ((pch != NULL) && (index < _max))
    {
        _fields[index++] = pch;
        pch = strtok(NULL, _delimiter);
    }
    return index;
}

inline size_t rowParser(char *_source, const char _delimiter, vector<char*> &_fields_vec)
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

inline size_t rowParser(char *_source, const char *_delimiter, vector<char*> &_fields_vec)
{
    _fields_vec.clear();
    char *pch = strtok(_source, _delimiter);
    while (pch != NULL)
    {
        _fields_vec.push_back(pch);
        pch = strtok(NULL, _delimiter);
    }
    return _fields_vec.size();
}

inline size_t rowParser(char *_source, vector<char*> &_fields_vec)
{
    _fields_vec.clear();
    if (*_source == '\0')
        return 0;
    if (isalnum(*_source))
        _fields_vec.push_back(_source);
    
    while (*_source != '\0')
    {
        if (!isalnum(*_source))
        {
            while ((*_source != '\0') && (!isalnum(*_source)))
            {
                *_source = '\0';
                _source++;
            }
            if (strlen(_source) > 0)
                _fields_vec.push_back(_source);
            continue;
        }
        _source++;
    }
    return _fields_vec.size();
}

inline size_t rowParser(char *_source, const char _delimiter, char **_fields, size_t _max, bool _print = false)
{
    size_t index = 0;
    _fields[index++] = _source;
    
    while ((*_source != '\0') && (index < _max))
    {
        if (*_source == _delimiter)
        {
            *_source = '\0';
            _source++;
            _fields[index++] = _source;
            continue;
        }
        _source++;
    }
    return index;
}

inline size_t rowTokenizer(char *_source, const char _escape, vector<char*> &_fields_vec)
{
    _fields_vec.clear();
    if (*_source == '\0')
        return 0;
    
    bool has_first = false;
    char *start = NULL;
    
    while (*_source != '\0')
    {
        if (*_source == _escape)
        {
            *_source = '\0';
            if (has_first == false)
            {
                start = _source + 1;
                has_first = true;
            }
            else
            {
                if (strlen(start) > 0)
                {
                    _fields_vec.push_back(start);
                    start = NULL;
                }
                has_first = !has_first;
            }
        }
        _source++;
    }
    return _fields_vec.size();
}

inline void trim_space(char *_str)
{
    if (_str != NULL)
    {
        char *start = _str;
        
        while ((*_str != '\0') && (isspace(*_str)))
            _str++;
        
        size_t len = strlen(_str);
        while ((len > 0) && (isspace(_str[len - 1])))
        {
            _str[--len] = '\0';
            if (len == 0)
                break;
        }
        for (size_t i = 0; i < len; i++)
            start[i] = _str[i];
        start[len] = '\0';
    }
}

inline void trim_non_alnum(char *_str)
{
    if (_str != NULL)
    {
        char *start = _str;
        while ((*_str != '\0') && (!isalnum(*_str)))
            _str++;
        
        size_t len = strlen(_str);
        while ((len > 0) && (!isalnum(_str[len - 1])))
        {
            _str[--len] = '\0';
            if (len == 0)
                break;
        }
        for (size_t i = 0; i < len; i++)
            start[i] = _str[i];
        start[len] = '\0';
    }
}

inline bool is_valid_field(const char *_field, bool _trim_non_alnum = true)
{
    if (_field == NULL)
        return false;
    if (_field[0] == '\0')
        return false;
    if (strlen(_field) == 0)
        return false;
    
    char *temp = new char [strlen(_field) + 1];
    char *ptr = temp;
    strcpy(temp, _field);
    trim_space(temp);
    if (_trim_non_alnum == true)
        trim_non_alnum(temp);
    
    bool ret = true;
    if (strlen(temp) == 0)
        ret = false;
    delete [] ptr;
    return ret;
}

inline bool is_valid_nucleotide(char _nuc)
{
    switch(_nuc)
    {
        case 'a':
        case 'A':
        case 'c':
        case 'C':
        case 'g':
        case 'G':
        case 't':
        case 'T':
            return true;
        default:
            return false;
    }
}

inline bool filter_thousand_separator(char *_str)
{
    if (_str == NULL)
        return false;
    size_t len = strlen(_str);
    size_t w_idx = 0;
    for (size_t r_idx = 0; r_idx < len; r_idx++)
    {
        if ((isdigit(_str[r_idx]) == true) || (_str[r_idx] == '.') || (_str[r_idx] == ','))
        {   
        }
        else
            return false;
    }
    for (size_t r_idx = 0; r_idx < len; r_idx++)
    {
        if (_str[r_idx] == ',')
            continue;
        _str[w_idx++] = _str[r_idx];
    }
    _str[w_idx] = '\0';
    return true;
}

inline void replace_char(char *_str, char _tgt, char _des)
{
    if (_str != NULL)
    {
        size_t len = strlen(_str);
        for (size_t i = 0; i < len; i++)
        {
            if (_str[i] == _tgt)
                _str[i] = _des;
        }
    }
}

inline void swap_alleles(char &_allele_one, char &_allele_two)
{
    if (_allele_one > _allele_two)
    {
        char tmp = _allele_one;
        _allele_one = _allele_two;
        _allele_two = tmp;
    }
}

inline void trim_file_ext(char *_str)
{
    if (_str != NULL)
    {
        char *buf = _str;
        size_t len = strlen(_str);
        _str += (len - 1);
        while ((*_str != '.') && (_str != buf))
            _str--;
        if (*_str == '.')
            *_str = '\0';
    }
}

inline void base_name(char *_str)
{
    if (_str != NULL)
    {
        char *buf = _str;
        size_t len = strlen(_str);
        _str += (len - 1);
        while ((*_str != '/') && (_str != buf))
            _str--;
        if (_str != buf) { assert(*_str == '/'); }
        _str++;
        while (*_str != '\0')
        {
            *buf = *_str;
            buf++;
            _str++;
        }
        *buf = '\0';
    }
}

inline const char *get_month_txt(const unsigned char _month)
{
    assert((_month >= 1) && (_month <= 12));
    switch (_month)
    {
        case 1:
            return "Jan";
        case 2:
            return "Feb";
        case 3:
            return "Mar";
        case 4:
            return "Apr";
        case 5:
            return "May";
        case 6:
            return "Jun";
        case 7:
            return "Jul";
        case 8:
            return "Aug";
        case 9:
            return "Sep";
        case 10:
            return "Oct";
        case 11:
            return "Nov";
        case 12:
            return "Dec";
        default:
            assert(false);
    }
}

static double diff(timespec start, timespec end)
{
    timespec temp;
    if (end.tv_nsec < start.tv_nsec)
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    char tstr[64] = {0};
    sprintf(tstr, "%lu.%ld", temp.tv_sec, temp.tv_nsec);
    return double(atof(tstr));
}

static bool is_valid_testcode(const char *_testcode)
{
    if (strlen(_testcode) != 5)
        return false;
    for (unsigned i = 0; i < 3; i++)
        if ((isupper(_testcode[i]) == false) || (isalpha(_testcode[i]) == false))
            return false;
    for (unsigned i = 3; i < 5; i++)
        if (isdigit(_testcode[i]) == false)
            return false;
    return true;
}

static bool is_valid_sample_id(const char *_sample_id)
{
    if (_sample_id == NULL)
        return false;
    if (strlen(_sample_id) != 6)
        return false;
    for (size_t i = 0; i < 6; i++)
        if (isdigit(_sample_id[i]) == false)
            return false;
    return true;
}

static bool is_valid_date(const char *_date)
{
    if (_date == NULL)
        return false;
    if (strlen(_date) != 10)
        return false;
    
    for (size_t i = 0; i < 10; i++)
    {
        if ((i == 4) || (i == 7))
        {
            if (_date[i] != '-')
                return false;
        }
        else
        {
            if (isdigit(_date[i]) == false)
                return false;
        }
    }
    return true;
}

static bool is_lims_sample(const char *_sample_id)
{
    if (_sample_id == NULL)
        return false;
    if (strlen(_sample_id) != 23)
        return false;
    char t_buf[32] = {0};
    strcpy(t_buf, _sample_id);
    vector<char*> sample_fields;
    size_t f_cnt = rowParser(t_buf, '/', sample_fields);
    if (f_cnt != 3)
        return false;
    
    if (is_valid_testcode(sample_fields[0]) == false)
        return false;
    if (is_valid_sample_id(sample_fields[1]) == false)
        return false;
    if (is_valid_date(sample_fields[2]) == false)
        return false;
        
    return true;
}
#endif	/* QSDB_FUNC_H */

