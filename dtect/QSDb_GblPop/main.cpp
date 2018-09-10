/* 
 * File:   main.cpp
 * Author: wuntyng
 *
 * Created on July 4, 2012, 11:13 AM
 */

#include <cstdlib>
#include <stdio.h>
#include "QSDb.h"
#include "QSDb_Func.h"
#include "GSSDb.h"
#include "GSSConfig.h"
#include "GblPopDownloader.h"

using namespace std;

int main(int argc, char** argv) 
{
    bool ret = false;
    GSSConfig xmlConfig;
    QSDb qs_db("QSDb");
    ret = qs_db.init(
        xmlConfig.get_dbhost(qs_db.get_dbname()),
        xmlConfig.get_dbuser(qs_db.get_dbname()),
        xmlConfig.get_dbpswd(qs_db.get_dbname()));
    assert(ret == true);

    GSSDb gss_db("GSSDb");
    ret = gss_db.init(
        xmlConfig.get_dbhost(gss_db.get_dbname()),
        xmlConfig.get_dbuser(gss_db.get_dbname()),
        xmlConfig.get_dbpswd(gss_db.get_dbname()));
    assert(ret == true);
    
    mysqlpp::StoreQueryResult res;
    gss_db.get_rsid(res);
    
    for(size_t i = 0; i < res.size(); ++i)
    {
        unsigned rsid = atol(res[i]["RsId"].c_str());
        string assay_id = res[i]["AssayIdNK"].c_str();
        char vic_allele = res[i]["VicAllele"].c_str()[0];
        char fam_allele = res[i]["FamAllele"].c_str()[0];
        
        GblPopDownloader pop;
        while(pop.size() == 0)
        {
            pop.download_xml(rsid);
            
            if(pop.size() == 0)
                sleep(2);
        }

        string population;
        string genotype;
        string vf_allele;
        unsigned sample_count = 0;

        map<PopGeno*, unsigned, PopGeno_ltr> pop_map;
        map<PopGeno*, unsigned, PopGeno_ltr>::iterator pop_it;

        while (pop.get_record(population, genotype, sample_count))
        {
            char allele_one = genotype.c_str()[0];
            char allele_two = genotype.c_str()[2];
            if((is_valid_nucleotide(allele_one) == false) || (is_valid_nucleotide(allele_two) == false))
                continue;

            if (((vic_allele == allele_one) && (fam_allele == allele_two)) ||
                ((vic_allele == allele_two) && (fam_allele == allele_one)))
                vf_allele = "VIC/FAM";
            else if((vic_allele == genotype.c_str()[0]) && (vic_allele == genotype.c_str()[2]))
                vf_allele = "VIC/VIC";
            else if((fam_allele == genotype.c_str()[0]) && (fam_allele == genotype.c_str()[2]))
                vf_allele = "FAM/FAM";
            else
            {
                if (allele_one == allele_two)
                {
                    // VIC/VIC OR FAM/FAM
                    if (complement(allele_one) == vic_allele)
                        vf_allele = "VIC/VIC";
                    else if (complement(allele_one) == fam_allele)
                        vf_allele = "FAM/FAM";
                    else { assert(false); }
                }
                else
                {
                    assert((complement(allele_one) == vic_allele) ||
                           (complement(allele_one) == fam_allele));

                    assert((complement(allele_two) == vic_allele) ||
                           (complement(allele_two) == fam_allele));

                    vf_allele = "VIC/FAM";
                }
            }

            PopGeno t_obj;
            t_obj.population = population;
            t_obj.genotype = vf_allele;
            pop_it = pop_map.find(&t_obj);
            if (pop_it == pop_map.end())
                pop_it = pop_map.insert(pair<PopGeno*, unsigned>(new PopGeno(&t_obj), 0)).first;
            assert(pop_it != pop_map.end());
            pop_it->second += sample_count;
        }

        for (pop_it = pop_map.begin(); pop_it != pop_map.end(); ++pop_it)
        {
            ret = qs_db.insert_population(assay_id, pop_it->first->genotype, pop_it->first->population, pop_it->second);
            assert(ret == true);
        }
    }

    return 0;
}
