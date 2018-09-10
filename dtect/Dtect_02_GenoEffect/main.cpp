/* 
 * File:   main.cpp
 * Author: jameswong
 *
 * Created on May 21, 2012, 3:18 PM
 */

#include <cstdlib>
#include <stdio.h>
#include "GSSDb.h"
#include "GSSDb_Func.h"
#include "GSSConfig.h"
#include "TransDb.h"
#include "GSSLogger.h"

#define MAX_LENGTH 2048

extern char *__progname;

using namespace std;

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        fprintf(stdout, "%s [GenoEffect files]\n", __progname);
        exit(EXIT_SUCCESS);
    }
    GSSConfig gss_config;
    GSSDb gss_db("GSSDb");
    bool ret = gss_db.init(
        gss_config.get_dbhost(gss_db.get_dbname()),
        gss_config.get_dbuser(gss_db.get_dbname()),
        gss_config.get_dbpswd(gss_db.get_dbname()));
    assert(ret == true);
    
    GSSLogger *gss_logger = GSSLogger::get_instance();
    gss_logger->open_file(
            gss_config.get_logfile(),
            gss_config.get_redirect_stdout(),
            gss_config.get_redirect_stderr());
    assert(gss_logger != NULL);
    
    TransDb trans_db("TRANS_DB");
    ret = trans_db.init(
            gss_config.get_dbhost(trans_db.get_dbname()),
            gss_config.get_dbuser(trans_db.get_dbname()),
            gss_config.get_dbpswd(trans_db.get_dbname()));
    assert(ret == true);
    
    for (int arg = 1; arg < argc; arg++)
    {
        cout << "Parsing file " << argv[arg] << endl;
        size_t line_cnt = 0;
        FILE *inFile = fopen(argv[arg], "r");
        assert(inFile != NULL);
        char qString[MAX_LENGTH] = {0};
        fgets(qString, MAX_LENGTH, inFile);
        line_cnt++;

        vector<char*> fields;

        while (fgets(qString, MAX_LENGTH, inFile))
        {
            line_cnt++;
            mysqlpp::sql_int index = {0};
            mysqlpp::sql_char chip_type;
            mysqlpp::sql_char chip_ver;
            vector<mysqlpp::sql_int_unsigned> rs_id_vec;
            mysqlpp::sql_varchar disease_drug;
            mysqlpp::sql_enum type;
            vector<mysqlpp::sql_int_unsigned> pm_id_vec;
            vector<mysqlpp::sql_enum> allele_one_vec;
            vector<mysqlpp::sql_enum> allele_two_vec;
            mysqlpp::sql_double risk;
            mysqlpp::sql_varchar effect;

            chomp(qString);
            rowParser(qString, '\t', fields);

            if (is_valid_field(fields[0]) == true)  // index
            {
                assert(isdigit(fields[0][0]));
                index = atoi(fields[0]);
            }
            if (index == 0)
            {
                fprintf(stderr, "\nWARNING: Row %lu has index zero\n", line_cnt);
                continue;
            }
            if (is_valid_field(fields[1]) == true)  // disease/drug
            {
                if (filter_non_print(fields[1]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:0 has non-printable character/s.\n", argv[arg], line_cnt);
                disease_drug.assign(fields[1]);
            }

            if (is_valid_field(fields[2]) == true)  // type
            {
                if (filter_non_print(fields[2]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:1 has non-printable character/s.\n", argv[arg], line_cnt);
                type.assign(fields[2]);
            }
            
            if (is_valid_field(fields[3]) == true)  // chip type
            {
                if (filter_non_print(fields[3]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:2 has non-printable character/s.\n", argv[arg], line_cnt);
                chip_type = fields[3];
                if (chip_type.length() != 3)
                {
                    fprintf(stderr, "\nERROR: Invalid testcode (ChipType) for disease/drug '%s'.\n", disease_drug.c_str());
                    exit(EXIT_FAILURE);
                }
            }

            if (is_valid_field(fields[4]) == true)  // chip ver
            {
                if (filter_non_print(fields[4]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:3 has non-printable character/s.\n", argv[arg], line_cnt);
                chip_ver = fields[4];
                if (chip_ver.length() != 2)
                {
                    fprintf(stderr, "\nERROR: Invalid testcode (ChipVer) for disease/drug '%s'.\n", disease_drug.c_str());
                    exit(EXIT_FAILURE);
                }
            }

            if (is_valid_field(fields[5]) == true)  // rsid
            {   
                if (filter_non_print(fields[5]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:4 has non-printable character/s.\n", argv[arg], line_cnt);
                vector<char*> rsid_fields;
                size_t rs_cnt = rowParser(fields[5], ',', rsid_fields);
                for (size_t i = 0; i < rs_cnt; i++)
                {
                    char temp[256] = {0};
                    strcpy(temp, rsid_fields[i]);
                    trim_space(temp);
                    trim_quotes(temp);
                    if (strstr(temp, "rs") != temp)
                        fprintf(stderr, "\n");
                    assert(strstr(temp, "rs") == temp);
                    rs_id_vec.push_back(mysqlpp::sql_int_unsigned(atol(temp + 2)));
                }
            }
            
            if (is_valid_field(fields[7]) == true)      // genotypes
            {
                if (filter_non_print(fields[7]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:6 has non-printable character/s.\n", argv[arg], line_cnt);
                vector<char*> geno_fields;
                size_t geno_cnt = rowParser(fields[7], ',', geno_fields);
                assert(geno_cnt == rs_id_vec.size());
                for (size_t i = 0; i < geno_cnt; i++)
                {
                    char temp[16] = {0};
                    strcpy(temp, geno_fields[i]);
                    trim_space(temp);
                    trim_quotes(temp);
                    assert((strchr(temp, ';') != NULL) || (strchr(temp, ':') != NULL));
                    
                    if (is_valid_nucleotide(temp[0]) == false)
                    {
                        fprintf(stderr, "\nERROR: Invalid VIC allele for disease/drug '%s'.\n", disease_drug.c_str());
                        exit(EXIT_FAILURE);
                    }
                    mysqlpp::sql_enum tmp_one;
                    tmp_one = temp[0];
                    allele_one_vec.push_back(tmp_one);
                    
                    if (is_valid_nucleotide(temp[2]) == false)
                    {
                        fprintf(stderr, "\nERROR: Invalid FAM allele for disease/drug '%s'.\n", disease_drug.c_str());
                        exit(EXIT_FAILURE);
                    }
                    mysqlpp::sql_enum tmp_two;
                    tmp_two = temp[2];
                    allele_two_vec.push_back(tmp_two);
                }
                if (allele_one_vec.empty() || allele_two_vec.empty())
                {
                    fprintf(stderr, "\nERROR: Missing VIC/FAM alleles for disease/drug '%s'.\n", disease_drug.c_str());
                    exit(EXIT_FAILURE);
                }
                if (allele_one_vec.size() != allele_two_vec.size())
                {
                    fprintf(stderr, "\nERROR: Mismatch VIC/FAM alleles for disease/drug '%s'.\n", disease_drug.c_str());
                    exit(EXIT_FAILURE);
                }
            }
            
            if (is_valid_field(fields[8]) == true)      // genotype effects
            {
                if (filter_non_print(fields[8]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:7 has non-printable character/s.\n", argv[arg], line_cnt);
                
                effect.assign(fields[8]);
                if (effect.length() > 128)
                {
                    fprintf(stderr, "\nERROR: Genotypic effect is longer than 128 characters.\n");
                    exit(EXIT_FAILURE);
                }
            }
            
            if (is_valid_field(fields[9]) == true)      // pmid
            {
                if (filter_non_print(fields[9]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:8 has non-printable character/s.\n", argv[arg], line_cnt);
                vector<char*> pmid_fields;
                size_t pm_cnt = rowParser(fields[9], ',', pmid_fields);
                for (size_t i = 0; i < pm_cnt; i++)
                {
                    char temp[32] = {0};
                    strcpy(temp, pmid_fields[i]);
                    trim_space(temp);
                    trim_quotes(temp);
                    mysqlpp::sql_int_unsigned pmid = atol(temp);
                    assert(pmid != 0);
                    pm_id_vec.push_back(pmid);
                }
                if (pm_id_vec.empty())
                {
                    fprintf(stderr, "\nERROR: No pmid for disease/drug '%s'.\n", disease_drug.c_str());
                    exit(EXIT_FAILURE);
                }
            }
            
            if (is_valid_field(fields[10], false) == true)      // risk
            {
                if (filter_non_print(fields[10]) == true)
                    gss_logger->log(true, "%s Line:%lu Column:9 has non-printable character/s.\n", argv[arg], line_cnt);
                if (strstr(fields[10], "Neutral") != NULL)
                    risk = 0.0;
                else if (strstr(fields[10], "Good") != NULL)
                    risk = 1.0;
                else if (strstr(fields[10], "Bad") != NULL)
                    risk = -1.0;
                else if (strstr(fields[10], "No") != NULL)
                    risk = 0.0;
                else if (strstr(fields[10], "Yes") != NULL)
                    risk = -1.0;
                else
                {
                    fprintf(stderr, "\nERROR: Invalid risk '%s'.\n", fields[10]);
                    exit(EXIT_FAILURE);
                }
            }
            
            if ((rs_id_vec.size() != allele_one_vec.size()) || (rs_id_vec.size() != allele_two_vec.size()))
            {
                fprintf(stderr, "\nERROR: Mismatch rsid and VIC/FAM alleles for disease/drug '%s'.\n", disease_drug.c_str());
                exit(EXIT_FAILURE);
            }
            size_t genotype_count = rs_id_vec.size();
            
            
            
            // one row parsed
            mysqlpp::sql_int_unsigned chip_key = 0;
            ret = gss_db.get_chip_key(chip_key, chip_type, chip_ver);
            assert(ret == true);
            assert(chip_key != 0);
            
            set<mysqlpp::sql_int_unsigned> genotype_key_set;
            set<mysqlpp::sql_int_unsigned>::iterator genotype_key_curr_it;
            set<mysqlpp::sql_int_unsigned>::iterator genotype_key_next_it;
            string genotype_text;
            
            for (size_t i = 0; i < genotype_count; i++)
            {
                mysqlpp::sql_int_unsigned genotype_key = 0;
                mysqlpp::sql_int_unsigned assay_key = 0;
                
                ret = gss_db.get_assay_key(assay_key, chip_key, rs_id_vec[i]);
                assert(ret == true);
                assert(assay_key != 0);
                
                ret = gss_db.insert_genotype(genotype_key, assay_key, allele_one_vec[i], allele_two_vec[i]);
                if (ret == false)
                {
                    fprintf(stderr, "\nERROR: %s rs%u [line %lu]\n", argv[arg], rs_id_vec[i], line_cnt);
                    exit(EXIT_FAILURE);
                }
                assert(genotype_key != 0);
                
                genotype_key_set.insert(genotype_key);
                
                char genotype_t_text[32] = {0};
                sprintf(genotype_t_text, "%u_%s%s", rs_id_vec[i], allele_one_vec[i].c_str(), allele_two_vec[i].c_str());
                genotype_text.append(genotype_t_text);
                
                char genotype_hash[32] = {0};
                sprintf(genotype_hash, "%u-%s%s", rs_id_vec[i], allele_one_vec[i].c_str(), allele_two_vec[i].c_str());
                
                if (i != genotype_count - 1)
                    genotype_text.append("_");
            }
            
            char effect_id[64] = {0};
            sprintf(effect_id, "%s_%s_%02d_%s",
                    chip_type.c_str(),
                    chip_ver.c_str(),
                    index,
                    genotype_text.c_str());
            
            char dis_drug_id[64] = {0};
            sprintf(dis_drug_id, "%s_%s_%02d",
                    chip_type.c_str(),
                    chip_ver.c_str(),
                    index);
            
            ret = gss_db.assert_dis_drug_id(dis_drug_id, type);
            assert(ret == true);
            
            ret = gss_db.insert_effect(effect_id, dis_drug_id, genotype_count, risk);
            assert(ret == true);
            
            char doc_id[16] = {0};
            sprintf(doc_id, "GS-%s", chip_type.c_str());
            
            ret = trans_db.insert_content(doc_id, "en", "1", "GENOTYPIC_EFFECT", effect_id, effect.c_str());
            assert(ret == true);
            
            genotype_key_curr_it = genotype_key_set.begin();
            genotype_key_next_it = genotype_key_set.begin();
            ++genotype_key_next_it;
            
            while (genotype_key_curr_it != genotype_key_set.end())
            {
                mysqlpp::sql_int_unsigned genolink_key = 0;
                if (genotype_key_next_it == genotype_key_set.end())
                {
                    ret = gss_db.insert_genolink(genolink_key, *genotype_key_curr_it, *(genotype_key_set.begin()));
                    assert(ret == true);
                }
                else
                {
                    ret = gss_db.insert_genolink(genolink_key, *genotype_key_curr_it, *genotype_key_next_it);
                    assert(ret == true);
                }
                assert(genolink_key != 0);
                
                mysqlpp::sql_int_unsigned geno_effect_key = 0;
                ret = gss_db.insert_geno_effect(geno_effect_key, genolink_key, effect_id);
                assert(ret == true);
                assert(geno_effect_key != 0);
                
                size_t pm_id_cnt = pm_id_vec.size();
                for (size_t pm = 0; pm < pm_id_cnt; pm++)
                {
                    ret = gss_db.insert_citation(geno_effect_key, pm_id_vec[pm]);
                    assert(ret == true);
                }
                
                if (genotype_key_next_it == genotype_key_set.end())
                    break;
                ++genotype_key_curr_it;
                ++genotype_key_next_it;
            }
        }
        fclose(inFile);
        inFile = NULL;
    }
    return 0;
}

