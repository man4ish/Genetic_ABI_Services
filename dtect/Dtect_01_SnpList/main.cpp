/* 
 * File:   main.cpp
 * Author: jameswong
 *
 * Created on May 21, 2012, 3:18 PM
 */

#include <cstdlib>
#include <stdio.h>
#include <limits>
#include "GSSDb.h"
#include "GSSDb_Func.h"
#include "GSSConfig.h"
#include "TransDb.h"

#define MAX_LENGTH 2048

extern char *__progname;

using namespace std;

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        fprintf(stdout, "%s [SnpList files]\n", __progname);
        exit(EXIT_SUCCESS);
    }
    GSSConfig gss_config;
    GSSDb gss_db("GSSDb");
    bool ret = gss_db.init(
        gss_config.get_dbhost(gss_db.get_dbname()),
        gss_config.get_dbuser(gss_db.get_dbname()),
        gss_config.get_dbpswd(gss_db.get_dbname()));
    assert(ret == true);
    
    TransDb trans_db("TRANS_DB");
    ret = trans_db.init(
            gss_config.get_dbhost(trans_db.get_dbname()),
            gss_config.get_dbuser(trans_db.get_dbname()),
            gss_config.get_dbpswd(trans_db.get_dbname()));
    assert(ret == true);
    
    for (int arg = 1; arg < argc; arg++)
    {
        cout << "Parsing file " << argv[arg] << endl;
        FILE *inFile = fopen(argv[arg], "r");
        assert(inFile != NULL);
        char qString[MAX_LENGTH] = {0};
        char *fret = fgets(qString, MAX_LENGTH, inFile);
        assert(fret != NULL);
        size_t row_count = 1;
        
        vector<char*> fields;

        while (fgets(qString, MAX_LENGTH, inFile))
        {
            row_count++;
            mysqlpp::sql_int index = 0;
            mysqlpp::sql_int_unsigned chip_key = 0;
            mysqlpp::sql_varchar assay_id;
            mysqlpp::sql_char chip_type;
            mysqlpp::sql_char chip_ver;
            mysqlpp::sql_enum vic_allele;
            mysqlpp::sql_enum fam_allele;
            mysqlpp::sql_int_unsigned rs_id;
            mysqlpp::sql_varchar disease_drug;
            mysqlpp::sql_enum type;
            vector<mysqlpp::sql_int_unsigned> pm_id_vec;
            mysqlpp::sql_char sex;
            mysqlpp::sql_char risk_allele;
            double rr_factor = numeric_limits<double>::min();
            double rn_factor = numeric_limits<double>::min();
            double nn_factor = numeric_limits<double>::min();
	    double Incidence = numeric_limits<double>::min();
            mysqlpp::sql_char trait;

            chomp(qString);
            rowParser(qString, '\t', fields);

            if (is_valid_field(fields[0]) == true)  // index
            {
                if (isdigit(fields[0][0]) == false)
                {
                    fprintf(stderr, "\nERROR: Invalid index field [%s]\n", fields[0]);
                    exit(EXIT_FAILURE);
                }
                index = atoi(fields[0]);
            }
            if (index == 0)
            {
                fprintf(stderr, "\nWARNING: Line %lu has index 0\n", row_count);
                continue;
            }
            
            if (is_valid_field(fields[1]) == true)  // disease/drug
                disease_drug.assign(fields[1]);

            if (is_valid_field(fields[3]) == true)  // type
                type.assign(fields[3]);

            if (is_valid_field(fields[4]) == true)  // rsid
            {
                if (strstr(fields[4], "rs") != fields[4])
                {
                    fprintf(stderr, "\nERROR: Invalid rsid field [%s]\n", fields[4]);
                    exit(EXIT_FAILURE);
                }
                rs_id = mysqlpp::sql_int_unsigned(atol(fields[4] + 2));
                assert(rs_id != 0);
            }

            if (is_valid_field(fields[5]) == true)  // chip type
                chip_type = fields[5];

            if (is_valid_field(fields[6]) == true)  // chip ver
                chip_ver = fields[6];

            if (is_valid_field(fields[7]) == true)  // assay
                assay_id.assign(fields[7]);

            if (is_valid_field(fields[8]) == true)  // pmid
            {
                vector<char*> pmid_fields;
                size_t pm_cnt = rowParser(fields[8], ',', pmid_fields);
                for (size_t i = 0; i < pm_cnt; i++)
                {
                    char temp[256] = {0};
                    strcpy(temp, pmid_fields[i]);
                    trim_space(temp);
                    pm_id_vec.push_back(mysqlpp::sql_int_unsigned(atol(temp)));
                }
            }
            
            if (is_valid_field(fields[9]) == true) // vic/fam allele
            {
                vic_allele = fields[9][0];
                fam_allele = fields[9][2];
            }
            
            if (is_valid_field(fields[14]) == true) // sex
            {
                if ((strcmp(fields[14], "Female") == 0))
                    sex = 'F';
                else if (strcmp(fields[14], "Male") == 0)
                    sex = 'M';
                else if (strcmp(fields[14], "Both") == 0)
                    sex = 'B';
                else { assert(false); }
            }
            
            if (is_valid_field(fields[15]) == true) // risk allele
            {
                assert(is_valid_nucleotide(fields[15][0]));
                risk_allele = fields[15][0];
            }
            if (is_valid_field(fields[16]) == true) // protective allele
            {
                assert(is_valid_nucleotide(fields[16][0]));
                assert(risk_allele.empty());
                risk_allele = fields[16][0];
            }
            
            if (is_valid_field(fields[36]) == true) // RR factor
                rr_factor = double(atof(fields[36]));
            if (is_valid_field(fields[37]) == true) // RN factor
                rn_factor = double(atof(fields[37]));
            if (is_valid_field(fields[38]) == true) // NN factor
                nn_factor = double(atof(fields[38]));

            if (is_valid_field(fields[39]) == true) // Incidence
                Incidence = double(atof(fields[39]));            

            if (is_valid_field(fields[40]) == true) // Trait - Y/N
                trait = fields[40][0];                  

            // done parsing one record, do some error checks
            if (disease_drug.empty())
            {
                fprintf(stderr, "\nERROR: No disease/drug name.\n");
                exit(-1);
            }
            if (disease_drug.length() >= 127)
            {
                fprintf(stderr, "\nERROR: Disease/drug '%s' exceeds the 127 characters limit.\n", disease_drug.c_str());
                exit(EXIT_FAILURE);
            }
            if (type.empty())
            {
                fprintf(stderr, "\nERROR: No disease/drug type.\n");
                exit(EXIT_FAILURE);
            }
            if ((type != "Disease") && (type != "Drug") && (type != "Trait"))
            {
                fprintf(stderr, "\nERROR: Invalid disease/drug type [%s]\n", type.c_str());
                exit(EXIT_FAILURE);
            }
            if (chip_type.empty())
            {
                fprintf(stderr, "\nERROR: No testcode (ChipType).\n");
                exit(EXIT_FAILURE);
            }
            if (chip_type.length() != 3)
            {
                fprintf(stderr, "\nERROR: Testcode (ChipType) is not of length 3 characters.\n");
                exit(EXIT_FAILURE);
            }
            for (size_t i = 0; i < chip_type.length(); i++)
            {
                if (!isupper(chip_type.c_str()[i]))
                {
                    fprintf(stderr, "\nERROR: Invalid/lowercase testcode [%s]\n", chip_type.c_str());
                    exit(EXIT_FAILURE);
                }
            }
            if (chip_ver.empty())
            {
                fprintf(stderr, "\nERROR: No testcode (ChipVer)\n");
                exit(EXIT_FAILURE);
            }
            if (chip_ver.length() != 2)
            {
                fprintf(stderr, "\nERROR: Testcode (ChipVer) is not of length 2 digits.\n");
                exit(EXIT_FAILURE);
            }
            for (size_t i = 0; i < chip_ver.length(); i++)
            {
                if (!isdigit(chip_ver.c_str()[i]))
                {
                    fprintf(stderr, "\nERROR: Invalid testcode [%s]\n", chip_ver.c_str());
                    exit(EXIT_FAILURE);
                }
            }
            if (assay_id.empty())
            {
                fprintf(stderr, "\nERROR: No assay id.\n");
                exit(EXIT_FAILURE);
            }
            if (assay_id.length() > 16)
            {
                fprintf(stderr, "\nERROR: Assay id '%s' exceeds the 16 characters limit.\n", assay_id.c_str());
                exit(EXIT_FAILURE);
            }
            if (pm_id_vec.empty())
            {
                fprintf(stderr, "\nERROR: No pmid for disease/drug '%s'.\n", disease_drug.c_str());
                exit(EXIT_FAILURE);
            }
            if (vic_allele.empty())
            {
                fprintf(stderr, "\nERROR: No VIC allele for disease/drug '%s'\n", disease_drug.c_str());
                exit(EXIT_FAILURE);
            }
            if(is_valid_nucleotide(vic_allele.c_str()[0]) == false)
            {
                fprintf(stderr, "\nERROR: Invalid VIC allele [%s] for disease/drug '%s'\n", vic_allele.c_str(), disease_drug.c_str());
                exit(EXIT_FAILURE);
            }
            if (fam_allele.empty())
            {
                fprintf(stderr, "\nERORR: No FAM allele for disease/drug '%s'\n", disease_drug.c_str());
                exit(EXIT_FAILURE);
            }
            if (is_valid_nucleotide(fam_allele.c_str()[0]) == false)
            {
                fprintf(stderr, "\nERROR: Invalid FAM allele [%s] for disease/drug '%s'\n", fam_allele.c_str(), disease_drug.c_str());
                exit(EXIT_FAILURE);
            }
            if (sex.empty())
            {
                fprintf(stderr, "\nERROR: 'Sex' is not specified for disease/drug '%s'\n", disease_drug.c_str());
                exit(EXIT_FAILURE);
            }
            
            // one row parsed
            char text_id[32] = {0};
            sprintf(text_id, "%s_%s_%02d", chip_type.c_str(), chip_ver.c_str(), index);
            
            ret = gss_db.insert_chip(chip_key, chip_type, chip_ver);
            assert(ret == true);
            assert(chip_key != mysqlpp::sql_tinyint_unsigned(0));
            
            mysqlpp::sql_int_unsigned assay_key = mysqlpp::sql_int_unsigned(0);
            ret = gss_db.insert_assay(assay_key, chip_key, rs_id, assay_id, vic_allele, fam_allele);
            assert(ret == true);
            assert(assay_key != mysqlpp::sql_int_unsigned(0));
            
            //ret = gss_db.insert_dis_drug(text_id, type, sex);
            ret = gss_db.insert_dis_drug(text_id, type, sex, Incidence, trait); //syhwah - 2013-04-03

            assert(ret == true);
            
            if (risk_allele.empty() == false)
            {
                ret = gss_db.insert_risk_factor(assay_key, text_id, risk_allele, rr_factor, rn_factor, nn_factor);
                assert(ret == true);
            }
            
            char doc_id[16] = {0};
            sprintf(doc_id, "GS-%s", chip_type.c_str());
            ret = trans_db.insert_content(doc_id, "en", "1", "DIS_DRUG_NAME", text_id, disease_drug.c_str());
            if (ret == false)
            assert(ret == true);
        }
        fclose(inFile);
        inFile = NULL;
    }
    return 0;
}
