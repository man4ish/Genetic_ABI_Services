/*
 * File:   DtectMapper.h
 * Author: jameswong
 *
 * Created on February 7, 2013, 5:27 PM
 */

#ifndef DTECTMAPPER_H
#define	DTECTMAPPER_H

#include <string>
#include "QSDb.h"
#include "QSDb_Func.h"
#include "GSSDb.h"
#include "VarDb.h"
#include "OpenDb.h"
#include "GSSConfig.h"
#include "LimsDb.h"
#include "DtectReport.h"
#include "GssXmlWriter.h"
#include "QSGenPop.h"
#include "GSSLogger.h"
#include "TransConfig.h"

#define MAX_LENGTH 2048

using namespace std;

class DtectMapper
{
private:
    string sample_id;
    string testcode;
    string chip_type;
    string chip_type_lc;    // lowercase
    string chip_ver;
    string open_array_id;
    char status;

    string partner_code; //syhwah 2013-04-18
    string patient_id;
    string patient_name;
    string gender;
    mysqlpp::sql_date dob;
    string patient_add;
    string phone;
    string ethnic;
    string physician_name;
    string physician_add;
    mysqlpp::sql_date sample_collection_date;
    string specimen_description;
    mysqlpp::sql_date order_rcvd_date;
    string InterpretResult; 
    set<string> lang_code_set;
    string qrcode_png_filename;
    string xml_output_filename;
    string pdf_output_filename;

    map<string, QSGenPop> gbl_pop_map;

    DtectReport dtect_report;

    LimsDb lims_db;
    GSSDb gss_db;
    QSDb qs_db;
    VarDb var_db;
    OpenDb open_db;
    
    GSSConfig gss_config;
    GSSLogger *gss_logger;
    TransConfig trans_conf;

private:
    static bool is_valid_call(
        const char *_vf_call);

    static bool is_valid_call(
        const char *_vf_call,
        string &_call_str,
        const char _vic_allele,
        const char _fam_allele);

    static bool is_valid_nucleotide(
        const char _nuc);

    bool map_gbl_pop();

    bool output_to_xml(
        const string& _lang_code);

    bool get_sample_details_from_lims();

    bool generate_qr_code();

    bool construct_output_xml_filename(
        const string& _lang_code);
    /*
    bool process_ext_content(
        GssXmlWriter& _gss_xml_writer,
        const string& _lang_code,
        const string& _dis_drug_id,
        const char* _section);

    bool process_ext_content_multi(
        GssXmlWriter& _gss_xml_writer,
        const string& _lang_code,
        const string& _dis_drug_id,
        const char* _section);
    */

    bool process_G3_content(
        GssXmlWriter& _gss_xml_writer,
        const string& _lang_code,
        const string& _dis_drug_id,
        const char* _section);

    bool generate_pdf(const string& _lang_code) const; //syhwah 2013-03-15 :- added _lang_code
    
    void reset();

    static void to_upper(char *_desc, const char *_src)
    {
        assert(_desc != NULL);
        assert(_src != NULL);
        char *ptr = _desc;
        size_t len = strlen(_src);
        for (size_t i = 0; i < len; ++i)
        {
            *ptr = toupper(_src[i]);
            ptr++;
        }
        *ptr = '\0';
    }

    static void to_lower(char *_desc, const char *_src)
    {
        assert(_desc != NULL);
        assert(_src != NULL);
        char *ptr = _desc;
        size_t len = strlen(_src);
        for (size_t i = 0; i < len; ++i)
        {
            *ptr = tolower(_src[i]);
            ptr++;
        }
        *ptr = '\0';
    }
    
    long hash_assay_id(const char* _assay_id);

public:
    DtectMapper();

    ~DtectMapper();

    bool map_sample(
        const char *_sample_id,
        const char *_testcode,
        const char *_open_array_id);
};

#endif	/* DTECTMAPPER_H */

