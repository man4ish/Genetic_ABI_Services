/*
 * File:   DtectMapper.cpp
 * Author: jameswong
 *
 * Created on February 7, 2013, 5:27 PM
 */

#include <assert.h>
#include <mysql++/result.h>
#include "DtectMapper.h"
#include "GblPopAveSnp.h"
#include "GblPopSimPatient.h"
#include "DtectSort.h"
#include "TransDb.h"
#include "GssXmlWriter.h"
#include "VarDb.h"

DtectMapper::DtectMapper(): lims_db("LIMS_DB"), gss_db("GSSDb"), qs_db("QSDb"), var_db("VarDb")
{
    this->gbl_pop_map.clear();
    
    bool ret;
    ret = this->gss_config.reinit();
    assert(ret == true);
    
    ret = this->lims_db.init(
            this->gss_config.get_dbhost(this->lims_db.get_dbname()),
            this->gss_config.get_dbuser(this->lims_db.get_dbname()),
            this->gss_config.get_dbpswd(this->lims_db.get_dbname()));
    assert(ret == true);
    
    ret = this->gss_db.init(
            this->gss_config.get_dbhost(this->gss_db.get_dbname()),
            this->gss_config.get_dbuser(this->gss_db.get_dbname()),
            this->gss_config.get_dbpswd(this->gss_db.get_dbname()));
    assert(ret == true);
    
    ret = this->qs_db.init(
            this->gss_config.get_dbhost(this->qs_db.get_dbname()),
            this->gss_config.get_dbuser(this->qs_db.get_dbname()),
            this->gss_config.get_dbpswd(this->qs_db.get_dbname()));
    assert(ret == true);

    ret = this->open_db.init(
            this->gss_config.get_dbhost("QSDb"),
            this->gss_config.get_dbuser("QSDb"),
            this->gss_config.get_dbpswd("QSDb"));
    assert(ret == true);
    
    ret = this->var_db.init( //syhwah 2013-03-19
            this->gss_config.get_dbhost(this->var_db.get_dbname()),
            this->gss_config.get_dbuser(this->var_db.get_dbname()),
            this->gss_config.get_dbpswd(this->var_db.get_dbname()));
    assert(ret == true);
    
    this->gss_logger = GSSLogger::get_instance();
    assert(this->gss_logger != NULL);
}

DtectMapper::~DtectMapper()
{
}

void DtectMapper::reset()
{
    this->sample_id.clear();
    this->chip_type.clear();
    this->chip_type_lc.clear();
    this->chip_ver.clear();
    this->open_array_id.clear();

    this->patient_id.assign("123456-78-9012");
    this->patient_name.assign("anonymous");
    this->gender.assign("-");
    this->dob.convert("2012-01-01");
    this->patient_add.assign("-");
    this->phone.assign("-");
    this->ethnic.assign("-");
    this->physician_name.assign("-");
    this->physician_add.assign("-");
    this->sample_collection_date.convert("2012-01-01");
    this->specimen_description.assign("-");
    this->order_rcvd_date.convert("2012-01-01");
    this->lang_code_set.insert("en");
    this->qrcode_png_filename.clear();
    this->xml_output_filename.clear();
    this->pdf_output_filename.clear();
    
    this->dtect_report.reset();
}

bool DtectMapper::map_sample(
    const char* _sample_id,
    const char* _testcode,
    const char* _open_array_id)
{
    assert(_sample_id != NULL);
    assert(_testcode != NULL);
    assert(_open_array_id != NULL);
    
    // every invocation of this function is a new sample. So, call reset.
    this->reset();

    this->sample_id.assign(_sample_id);
    this->open_array_id.assign(_open_array_id);
    this->testcode.assign(_testcode);

    this->chip_type.assign(_testcode, 3);

    this->chip_type_lc.clear();
    size_t len = this->chip_type.length();
    for (size_t i = 0; i < len; i++)
        this->chip_type_lc += tolower(this->chip_type.c_str()[i]);

    this->chip_ver.assign(_testcode + 3, 2);
    this->status = 'O';

    bool ret = this->gss_config.reinit();
    assert(ret == true);

    if (this->gbl_pop_map.find(this->testcode) == this->gbl_pop_map.end())
    {
        ret = this->map_gbl_pop();
        assert(ret == true);
    }

    time_t start;
    time(&start);
    
    ret = this->qs_db.reconnect();
    assert(ret == true);

    ret = this->gss_db.reconnect();
    assert(ret == true);
    
    ret = this->open_db.reconnect();
    assert(ret == true);
    
    // map sample from QSDb against GSSDb
    mysqlpp::StoreQueryResult open_res;
    ret = this->open_db.map_sample_from_qsdb_against_gssdb(
            this->sample_id,
            this->open_array_id,
            this->chip_type,
            this->chip_ver,
            open_res);
    assert(ret == true);
    
    map<string, set<string> >val_assay_map;
    set<string> inv_assay_set;
    set<string> amb_assay_set;
    for (size_t i = 0; i < open_res.size(); ++i)
    {
        string call_str;
        if (this->is_valid_call(
                open_res[i]["VicFamCall"].c_str(),
                call_str,
                open_res[i]["VicAllele"].c_str()[0],
                open_res[i]["FamAllele"].c_str()[0]))
            val_assay_map[open_res[i]["AssayId"].c_str()].insert(call_str);
        else
            inv_assay_set.insert(open_res[i]["AssayId"].c_str());
    }

    // if an invalid assay if also found in valid_map, erase it from invalid_map
    for (set<string>::iterator it = inv_assay_set.begin(); it != inv_assay_set.end();)
    {
        set<string>::iterator cit = it++;
        if (val_assay_map.find(*cit) == val_assay_map.end()) continue;
        inv_assay_set.erase(cit);
    }

    // every valid assay should have only one call. Otherwise, erase it and set it as ambiguous
    for (map<string, set<string> >::iterator it = val_assay_map.begin(); it != val_assay_map.end();)
    {
        map<string, set<string> >::iterator cit = it++;
        assert(cit->second.empty() == false);
        if (cit->second.size() == 1) continue;
        amb_assay_set.insert(cit->first);
        val_assay_map.erase(cit);
    }

    // for every valid assay, obtain the associated disease/drug and insert into dis_drug_set
    set<string> dis_drug_set;
    set<string> val_assay_set;
    for (map<string, set<string> >::iterator it = val_assay_map.begin(); it != val_assay_map.end(); ++it)
        val_assay_set.insert(it->first);
    ret = this->gss_db.search_dis_drug(dis_drug_set, this->chip_type, this->chip_ver, &val_assay_set);
    assert(ret == true);

    // obtain the expected number of disease/drugs (based on design) for this chip
    size_t chip_dis_drug_count;
    ret = this->gss_db.get_dis_drug_cnt(chip_dis_drug_count, this->chip_type, this->chip_ver);
    assert(ret == true);

    // validation
    int unique_assay_cnt = 0;
    ret = this->gss_db.get_assay_cnt(unique_assay_cnt, this->chip_type, this->chip_ver);
    assert(ret == true);
    //assert(val_assay_map.size() + inv_assay_set.size() + amb_assay_set.size() == unique_assay_cnt); //commented by syhwah - 2013-04-04

    int assay_cnt = val_assay_map.size() + inv_assay_set.size() + amb_assay_set.size(); //syhwah - 2013-04-04

    int omitted_calls_cnt = 0;
    if (assay_cnt != unique_assay_cnt){
        assert(unique_assay_cnt > assay_cnt);
        omitted_calls_cnt = unique_assay_cnt - assay_cnt;
        unique_assay_cnt -= omitted_calls_cnt;
    }
    
    assert(dis_drug_set.size() <= chip_dis_drug_count);

    // Filter option one: If any ONE of the expected diseases/drugs not mapped, abort
    // Filter option two: If the valid assays called is lower than the absolute threshold, abort
    // Filter option three: If the valid assays called is lower than a pre-fixed percentage, abort
    if ((this->gss_config.get_check_missed_disease()) && (dis_drug_set.size() < chip_dis_drug_count))
        this->status = 'N';
    else if ((this->gss_config.get_max_invalid_calls() != 0) && (int(inv_assay_set.size() + amb_assay_set.size()) > this->gss_config.get_max_invalid_calls()))
    {
        if (amb_assay_set.empty() == false)
            this->status = 'A';     // Ambiguous calls for replicated samples
        else
            this->status = 'L';     // Low call rate
    }
    else if (double(val_assay_map.size())/double(unique_assay_cnt) * 100.0 < this->gss_config.get_min_pct_valid_calls())
        this->status = 'L';         // Low call rate
    else
        this->status = 'O';         // OKAY

    // update QSDb
    ret = this->qs_db.update_status(this->sample_id, this->chip_type, this->chip_ver, this->open_array_id,
            this->status, val_assay_map.size(), inv_assay_set.size(), amb_assay_set.size(),omitted_calls_cnt);
    assert(ret == true);
    
    ret = this->lims_db.reconnect();
    assert(ret == true);
    ret = this->lims_db.set_status_in_sample_test(this->sample_id, this->testcode, this->status);
    assert(ret == true);
    ret = this->lims_db.set_status_in_sample_test_run(this->sample_id, this->open_array_id, this->testcode, this->status);
    assert(ret == true);

    // Only proceed if status is 'O' (Okay)
    if (this->status != 'O') { return true; }


    // get ALL contents from GSSDb for this chip
    mysqlpp::StoreQueryResult all_res;
    ret = this->gss_db.get_all(this->chip_type, this->chip_ver, all_res);
    assert(ret == true);
    
    // all_res is a two-dimensional vector. parse it and assign to a map-of-set for rapid lookup
    // of assay id. One assay id may have multiple rows
    map<string, set<size_t> > assay_row_map;
    for (size_t i = 0; i < all_res.size(); ++i)
        assay_row_map[all_res[i]["AssayIdNK"].c_str()].insert(i);
    
    // match valid assays
    for (map<string, set<string> >::const_iterator val_assay_cit = val_assay_map.begin(); val_assay_cit != val_assay_map.end(); ++val_assay_cit)
    {
        map<string, set<size_t> >::const_iterator assay_row_it = assay_row_map.find(val_assay_cit->first);
        if (assay_row_it == assay_row_map.end())
            continue;
        for (set<size_t>::const_iterator row_it = assay_row_it->second.begin(); row_it != assay_row_it->second.end(); ++row_it)
        {
            size_t i = (*row_it);
            assert(val_assay_cit->second.size() == 1);
            char allele_one = (*val_assay_cit->second.begin()).c_str()[0];
            char allele_two = (*val_assay_cit->second.begin()).c_str()[2];
            assert(this->is_valid_nucleotide(allele_one));
            assert(this->is_valid_nucleotide(allele_two));

            if ((all_res[i]["AlleleOne"].c_str()[0] != allele_one) || (all_res[i]["AlleleTwo"].c_str()[0] != allele_two)) continue;

            // identify which relative risk base on zygosity
            char risk_allele = all_res[i]["RiskAllele"].c_str()[0];
            double relative_risk = 1.0;
            double Incidence = 1.0;  //syhwah 2013-04-02
  
            if (this->is_valid_nucleotide(risk_allele))
            {
                if (allele_one == allele_two){   // VIC/VIC or FAM/FAM                
                    if (allele_one == risk_allele)
                        relative_risk = double(atof(all_res[i]["RR_Factor"].c_str()));
                    else
                        relative_risk = double(atof(all_res[i]["NN_Factor"].c_str()));
                }
                else{                  
                    relative_risk = double(atof(all_res[i]["RN_Factor"].c_str()));
                } 
                
                if (relative_risk == 0.00) { relative_risk = 1.0; } //syhwah 2013-03-13

                Incidence = double(atof(all_res[i]["Incidence"].c_str())); //syhwah 2013-04-02              
            }

            this->dtect_report.insert_result(
                    all_res[i]["Type"].c_str(),
                    all_res[i]["DisDrugID"].c_str(),
                    all_res[i]["Sex"].c_str()[0],
                    all_res[i]["EffectID"].c_str(),
                    atoi(all_res[i]["GenotypeCount"].c_str()),
                    double(atof(all_res[i]["Risk"].c_str())),
                    unsigned(atol(all_res[i]["RsId"].c_str())),
                    allele_one,
                    allele_two,
                    risk_allele,
                    relative_risk,
                    Incidence, //syhwah 2013-04-02
                    all_res[i]["Trait"].c_str()[0], //syhwah 2013-04-03 
                    size_t(atol(all_res[i]["PmId"].c_str())));
        }
    }
    
    // CAUTION: 'wrong' effects need to be filtered, they are those that have multiple genotypes associated
    this->dtect_report.filter_effect();
    this->dtect_report.rewind();

    // get sample details from LIMS
    ret = this->get_sample_details_from_lims();
    assert(ret == true);

    // generate qr code for this sample
    ret = this->generate_qr_code();
    assert(ret == true);

    // output report to xml (language specific)
    for (set<string>::const_iterator it = this->lang_code_set.begin(); it != this->lang_code_set.end(); ++it)
    {
        // construct xml output filename
        ret = this->construct_output_xml_filename(*it);
        assert(ret == true);

        // output results to language specific XML file
        ret = this->output_to_xml(*it);
        assert(ret == true);

        // generate pdf
        bool pdf_ok = false;
        if (this->gss_config.get_print_pdf() == true)
        {
            pdf_ok = this->generate_pdf(*it);
            
            // Do a stat() on PDF file to confirm it exists
            struct stat st;
            if (stat(this->pdf_output_filename.c_str(), &st) != 0)
                pdf_ok = false;
            
            if (pdf_ok == true)   // successfully generated report in PDF
            {
                // Update LIMS with report generation date DATE(NOW())
                ret = this->lims_db.set_report_gen_date_in_sample_test_run(
                        this->sample_id,
                        this->open_array_id,
                        this->testcode);
                assert(ret == true);
                
                ret = this->lims_db.set_report_gen_date_in_sample_test(
                        this->sample_id,
                        this->testcode);
                assert(ret == true);
                
                ret = this->lims_db.set_report_filename(
                        this->sample_id,
                        this->testcode,
                        *it,
                        this->pdf_output_filename);
                assert(ret == true);
            }
            else    // PDF report NOT generated
            {
            }
        }
        
        // log
        time_t now;
        time(&now);
        this->gss_logger->log(true, " SAMPLE %s-%s | St:%c | %luV,%luI,%luA,%luO (%5.1f%%) | %s | %s | %lu secs\n",
                this->sample_id.c_str(),
                this->open_array_id.c_str(),
                this->status,
                val_assay_map.size(),
                inv_assay_set.size(),
                amb_assay_set.size(),
                omitted_calls_cnt,
                double(val_assay_map.size())/double(unique_assay_cnt) * 100.0,
                this->xml_output_filename.c_str(),
                (pdf_ok) ? "PDF_OK" : "NO_PDF",
                now - start);
        
        // if PDF report was successfully generated, delete intermediate files
        // Feature is configurable in XmlConfig
        if ((pdf_ok == true) && (this->gss_config.get_delete_intermediate() == true))
        {
            if (remove(this->qrcode_png_filename.c_str()) != 0)
            {
                this->gss_logger->log(true, "ERROR: Failed to remove file '%s' [%s].\n",
                        this->qrcode_png_filename.c_str(),
                        strerror(errno));
            }
            else
                this->qrcode_png_filename.clear();
            
            if (remove(this->xml_output_filename.c_str()) != 0)
            {
                this->gss_logger->log(true, "ERROR: Failed to remove file '%s' [%s].\n",
                        this->xml_output_filename.c_str(),
                        strerror(errno));
            }
            else
                this->xml_output_filename.clear();
        }
    }
    return true;
}

bool DtectMapper::map_gbl_pop()
{
    // This function is called ONCE per testcode. Subsequent samples of the same testcode will use
    // previously mapped global population values. This is controlled by gbl_pop_map<string, QSGenPop>.
    assert(this->chip_type.empty() == false);
    assert(this->testcode.empty() == false);
    assert(this->gbl_pop_map.find(this->testcode) == this->gbl_pop_map.end());
    map<string, QSGenPop>::iterator gbl_pop_it = this->gbl_pop_map.insert(pair<string, QSGenPop>(this->testcode, QSGenPop())).first;
    assert(gbl_pop_it != this->gbl_pop_map.end());
    
    bool ret = this->gss_config.reinit();
    assert(ret == true);

    ret = this->qs_db.reconnect();
    assert(ret == true);

    ret = this->gss_db.reconnect();
    assert(ret == true);

    // get ALL contents from GSSDb for this chip
    mysqlpp::StoreQueryResult all_res;
    ret = this->gss_db.get_all(this->chip_type, this->chip_ver, all_res);
    assert(ret == true);
    assert(all_res.empty() == false);

    time_t start_time;
    time(&start_time);
    this->gss_logger->log(true, "Mapping global population for %s...", this->testcode.c_str());

    // all_res is a two-dimensional vector. Parse it and assign to a map-of-set for rapid lookup
    // of assay id. One assay id may have multiple rows. Set stores the indexes to the all_res vector
    map<string, set<size_t> > assay_row_map;
    for (size_t i = 0; i < all_res.size(); ++i)
        assay_row_map[all_res[i]["AssayIdNK"].c_str()].insert(i);


    // get the VIC and FAM alleles for all assays of this testcode
    mysqlpp::StoreQueryResult alleles_res;
    ret = this->gss_db.get_alleles(this->chip_type, this->chip_ver, alleles_res);
    assert(ret == true);
    assert(alleles_res.empty() == false);
    
    // alleles_res is a two_dimensional vector. Parse it and assign to a map-of_set for rapid lookup
    // of assay id. One assay id may have multiple rows. Set stores the indexes to the alleles_res vector
    map<string, set<size_t> > alleles_row_map;
    for (size_t i = 0; i < alleles_res.size(); ++i)
        alleles_row_map[alleles_res[i]["AssayIdNK"].c_str()].insert(i);
    
    // get all assays from QSDb's GlobalPopulation for this sample, testcode and openarray
    mysqlpp::StoreQueryResult gblpop_assay_res;
    ret = this->qs_db.get_gbl_assay(gblpop_assay_res);
    assert(ret == true);
    assert(gblpop_assay_res.empty() == false);

    GblPopAveSnp gbl_pop_ave_snp;
    size_t cnt = gblpop_assay_res.size();
    for (size_t i = 0; i < cnt; ++i)
    {
        if (assay_row_map.find(gblpop_assay_res[i]["AssayId"].c_str()) == assay_row_map.end())
            continue;
        
        gbl_pop_ave_snp.insert(
                gblpop_assay_res[i]["AssayId"].c_str(),
                gblpop_assay_res[i]["VicFamCall"].c_str(),
                size_t(atol(gblpop_assay_res[i]["PopulationSize"].c_str())));
    }

    // simulate 1000 'virtual patients'
    map<unsigned, GblPopSimPatient> sim_patient_map;
    map<unsigned, GblPopSimPatient>::iterator sim_patient_it;

    // hard-coded to 1000 virtual patients for now. Should do more for higher accuracy and distribution.
    for (unsigned vp_id = 1; vp_id <= 1000; ++vp_id)
    {
        sim_patient_it = sim_patient_map.insert(pair<unsigned, GblPopSimPatient>(vp_id, GblPopSimPatient())).first;
        gbl_pop_ave_snp.rewind();
        string assay_id;
        while (gbl_pop_ave_snp.get_assayid(assay_id) == true)
        {
            assert(assay_id.empty() == false);
            map<string, set<size_t> >::const_iterator alleles_row_it = alleles_row_map.find(assay_id);
            
            for (set<size_t>::const_iterator it = alleles_row_it->second.begin(); it != alleles_row_it->second.end(); ++it)
            {
                // Fixed seed per virtual patient per assayId (rsid)
                srand(vp_id * this->hash_assay_id(assay_id.c_str()));
                
                size_t a = *it;
                assert(alleles_res[a]["AssayIdNK"] == assay_id);

                string vic_fam_call = gbl_pop_ave_snp.get_random_vf_call(assay_id);
                char vic_allele = alleles_res[a]["VicAllele"].c_str()[0];
                char fam_allele = alleles_res[a]["FamAllele"].c_str()[0];
                assert(this->is_valid_nucleotide(vic_allele));
                assert(this->is_valid_nucleotide(fam_allele));

                string call_str;
                if (this->is_valid_call(vic_fam_call.c_str(), call_str, vic_allele, fam_allele))
                    sim_patient_it->second.insert(assay_id, call_str);
                else { assert(false); }
            }
        }
    }

    for (sim_patient_it = sim_patient_map.begin(); sim_patient_it != sim_patient_map.end(); ++sim_patient_it)
    {
        // Each virtual patient is given a 'report' just like a normal patient. It is based on the
        // report that the risk for each virtual patient is calculated.
        DtectReport sim_dtect_report;

        sim_patient_it->second.rewind();
        string assay_id, vf_call;
        
        while (sim_patient_it->second.get_record(assay_id, vf_call) == true)
        {
            map<string, set<size_t> >::const_iterator assay_row_it = assay_row_map.find(assay_id);
            if (assay_row_it == assay_row_map.end()) continue;
            for (set<size_t>::const_iterator it = assay_row_it->second.begin(); it != assay_row_it->second.end(); ++it)
            {
                size_t a = *it;
                assert(all_res[a]["AssayIdNK"] == assay_id);

                char allele_one = vf_call.c_str()[0];
                char allele_two = vf_call.c_str()[2];
                assert(this->is_valid_nucleotide(allele_one));
                assert(this->is_valid_nucleotide(allele_two));

                if ((all_res[a]["AlleleOne"].c_str()[0] != allele_one) || (all_res[a]["AlleleTwo"].c_str()[0] != allele_two)) continue;

                // NOTE: Relative risk is not used for global population for now. However, in future
                //  we may wanna dynamically compute relative risk for global population instead of 
                // a fixed utility
                //
                // identify which relative risk base on zygosity
                char risk_allele = all_res[a]["RiskAllele"].c_str()[0];
                double relative_risk = 1.0;
                double Incidence = 1.0;  //syhwah 2013-04-02
 
                if (this->is_valid_nucleotide(risk_allele))
                {
                    if (allele_one == allele_two) {  // VIC/VIC or FAM/FAM                    
                        if (allele_one == risk_allele)
                            relative_risk = double(atof(all_res[a]["RR_Factor"].c_str()));
                        else
                            relative_risk = double(atof(all_res[a]["NN_Factor"].c_str()));
                    }
                    else {
                        relative_risk = double(atof(all_res[a]["RN_Factor"].c_str()));
                    }

                    if (relative_risk == 0.00) { relative_risk = 1.0; } //syhwah 2013-03-13
                    Incidence = double(atof(all_res[a]["Incidence"].c_str())); //syhwah 2013-04-02
                }
                sim_dtect_report.insert_result(
                        all_res[a]["Type"].c_str(),
                        all_res[a]["DisDrugID"].c_str(),
                        all_res[a]["Sex"].c_str()[0],
                        all_res[a]["EffectID"].c_str(),
                        atoi(all_res[a]["GenotypeCount"].c_str()),
                        double(atof(all_res[a]["Risk"].c_str())),
                        unsigned(atol(all_res[a]["RsId"].c_str())),
                        allele_one,
                        allele_two,
                        risk_allele,
                        relative_risk,
                        Incidence, //syhwah 2013-04-02
                        all_res[a]["Trait"].c_str()[0], //syhwah 2013-04-03
                        size_t(atol(all_res[a]["PmId"].c_str())));
            }
        }
        
        // CAUTION: 'wrong' effects need to be filtered, they are those that have multiple genotypes associated
        sim_dtect_report.filter_effect();
        sim_dtect_report.rewind();
        string type, dis_drug_id;
        double total_risk;
        while (sim_dtect_report.get_result(type, dis_drug_id, total_risk) == true)
            gbl_pop_it->second.insert(type, dis_drug_id, total_risk);
    }
    time_t stop_time;
    time(&stop_time);
    this->gss_logger->log(false, " Done in %lu seconds.\n", stop_time - start_time);
    return true;
}

bool DtectMapper::get_sample_details_from_lims()
{
    bool ret = this->gss_config.reinit();
    assert(ret == true);

    ret = this->lims_db.reconnect();
    assert(ret == true);

    ret = this->lims_db.get_patient_details(
            this->sample_id,
            this->patient_id,
            this->patient_name,
            this->gender,
            this->dob,
            this->patient_add,
            this->phone,
            this->ethnic);
    assert(ret == true);

    ret = this->lims_db.get_physician_details(
            this->sample_id,
            this->physician_name,
            this->physician_add);
    assert(ret == true);

    ret = this->lims_db.get_sample_collection_date(
            this->sample_id,
            this->sample_collection_date);
    assert(ret == true);

    ret = this->lims_db.get_specimen_description(
            this->sample_id,
            this->specimen_description);
    assert(ret == true);

    ret = this->lims_db.get_received_date(
            this->sample_id,
            this->order_rcvd_date);

/*
    ret = this->lims_db.get_received_date_and_InterpretResult(
            this->sample_id,
            this->order_rcvd_date,
            this->InterpretResult);
*/
    assert(ret == true);

    ret = this->lims_db.get_lang_code(
            this->sample_id,
            this->lang_code_set);
    assert(ret == true);

    ret = this->lims_db.get_partner_code( //syhwah 2013-04-18
            this->sample_id,
            this->partner_code);

    assert(ret == true);

    return ret;
}

bool DtectMapper::generate_qr_code()
{
    bool ret = this->gss_config.reinit();
    assert(ret == true);

    char qr_png_filename[MAX_LENGTH] = {0};
    sprintf(qr_png_filename, "%s/%s%s_%s_%04u%02u%02u_%s_%04d%02d%02d%02d%02d%02d.png",
            this->gss_config.get_qs_output_dir(),
            this->chip_type.c_str(),
            this->chip_ver.c_str(),
            this->sample_id.c_str(),
            this->order_rcvd_date.year(),
            this->order_rcvd_date.month(),
            this->order_rcvd_date.day(),
            this->open_array_id.c_str(),
            this->dtect_report.get_date_year(),
            this->dtect_report.get_date_month(),
            this->dtect_report.get_date_day(),
            this->dtect_report.get_date_hour(),
            this->dtect_report.get_date_min(),
            this->dtect_report.get_date_sec());
    size_t len = strlen(qr_png_filename);
    for (size_t i = strlen(this->gss_config.get_qs_output_dir()) + 1; i < len; ++i)
    {
        if ((qr_png_filename[i] == '/') || (isspace(qr_png_filename[i])))
            qr_png_filename[i] = '_';
    }

    char qrcode_string[MAX_LENGTH] = {0};
    if (this->patient_name == "anonymous")
        sprintf(qrcode_string, "www.mgrc.com.my");
    else
        sprintf(qrcode_string, "%s%s/%s/%02d-%02d-%04d",
            this->chip_type.c_str(),
            this->chip_ver.c_str(),
            this->sample_id.c_str(),
            this->order_rcvd_date.day(),
            this->order_rcvd_date.month(),
            this->order_rcvd_date.year());

    char qrencode_cmd_line[MAX_LENGTH] = {0};
    sprintf(qrencode_cmd_line, "qrencode -o %s -s 9 -m 1 \"%s\"",
            qr_png_filename,
            qrcode_string);
    int sys_ret = system(qrencode_cmd_line);
    if (sys_ret != 0)
    {
        this->gss_logger->log(true, "qrencode returned %d, expecting 0\n", sys_ret);
        exit(EXIT_FAILURE);
    }
    this->qrcode_png_filename.assign(qr_png_filename);
    return true;
}

bool DtectMapper::construct_output_xml_filename(
        const string& _lang_code)
{
    bool ret = this->gss_config.reinit();
    assert(ret == true);

    char xml_filename[MAX_LENGTH] = {0};
    sprintf(xml_filename, "%s/%s%s_%s_%04u%02u%02u_%s_%04d%02d%02d%02d%02d%02d_%s.xml",
            this->gss_config.get_qs_output_dir(),
            this->chip_type.c_str(),
            this->chip_ver.c_str(),
            this->sample_id.c_str(),
            this->order_rcvd_date.year(),
            this->order_rcvd_date.month(),
            this->order_rcvd_date.day(),
            this->open_array_id.c_str(),
            this->dtect_report.get_date_year(),
            this->dtect_report.get_date_month(),
            this->dtect_report.get_date_day(),
            this->dtect_report.get_date_hour(),
            this->dtect_report.get_date_min(),
            this->dtect_report.get_date_sec(),
            _lang_code.c_str());
    size_t len = strlen(xml_filename);
    for (size_t i = strlen(this->gss_config.get_qs_output_dir()) + 1; i < len; ++i)
    {
        if ((xml_filename[i] == '/') || (isspace(xml_filename[i])))
            xml_filename[i] = '_';
    }
    this->xml_output_filename.assign(xml_filename);
    
    // Construct pdf filename for stat() to confirm existence of PDF file
    char pdf_filename[MAX_LENGTH] = {0};
    sprintf(pdf_filename, "%s/%s%s_%s_%04u%02u%02u_%s_%04d%02d%02d%02d%02d%02d_%s.pdf",
            this->gss_config.get_qs_output_dir(),
            this->chip_type.c_str(),
            this->chip_ver.c_str(),
            this->sample_id.c_str(),
            this->order_rcvd_date.year(),
            this->order_rcvd_date.month(),
            this->order_rcvd_date.day(),
            this->open_array_id.c_str(),
            this->dtect_report.get_date_year(),
            this->dtect_report.get_date_month(),
            this->dtect_report.get_date_day(),
            this->dtect_report.get_date_hour(),
            this->dtect_report.get_date_min(),
            this->dtect_report.get_date_sec(),
            _lang_code.c_str());
    len = strlen(pdf_filename);
    for (size_t i = strlen(this->gss_config.get_qs_output_dir()) + 1; i < len; ++i)
    {
        if ((pdf_filename[i] == '/') || (isspace(pdf_filename[i])))
            pdf_filename[i] = '_';
    }
    this->pdf_output_filename.assign(pdf_filename);
    
    return true;
}

bool DtectMapper::output_to_xml(
        const string& _lang_code)
{
    assert(_lang_code.empty() == false);
    assert(this->status == 'O');
    this->dtect_report.rewind();

    bool ret = this->gss_config.reinit();
    assert(ret == true);

    TransDb trans_db("TRANS_DB");
    ret = trans_db.init(
            this->gss_config.get_dbhost(trans_db.get_dbname()),
            this->gss_config.get_dbuser(trans_db.get_dbname()),
            this->gss_config.get_dbpswd(trans_db.get_dbname()));
    assert(ret == true);

    string content_date;
    ret = trans_db.get_latest_date(content_date, _lang_code.c_str());
    assert(ret == true);

    GssXmlWriter gss_xml_writer;
    bool xml_ret;

    xml_ret = gss_xml_writer.initialise();
    assert(xml_ret == true);
    xml_ret = gss_xml_writer.insert_metadata(_lang_code.c_str(), content_date.c_str());
    assert(xml_ret == true);

    // images
    char main_logo[2048];
    sprintf(main_logo, "%slogo/logo_dtect_%s.pdf", this->gss_config.get_image_dir(), _lang_code.c_str());
    char product_logo[2048];
    sprintf(product_logo, "%slogo/logo_%s_%s.pdf", this->gss_config.get_image_dir(), this->chip_type_lc.c_str(), _lang_code.c_str());
    char company_logo[2048];
    sprintf(company_logo, "%slogo_mgrc.png", this->gss_config.get_image_dir());
    char background[2048];
    sprintf(background, "%sbackground/bg_%s.jpg", this->gss_config.get_image_dir(), this->chip_type_lc.c_str());
    char chip_logo[2048];
    sprintf(chip_logo, "%stitle/title_%s_%s.pdf", this->gss_config.get_image_dir(), this->chip_type_lc.c_str(), _lang_code.c_str());
    char arrow[2048];
    sprintf(arrow, "%spointer/pointer_%s.jpg", this->gss_config.get_image_dir(), _lang_code.c_str());
    char disease_logo[2048];
    sprintf(disease_logo, "%slogo_profile_disease.png", this->gss_config.get_image_dir());
    char cover_logo[2048];
    sprintf(cover_logo, "%slogo/logo_dtect_cover.jpg", this->gss_config.get_image_dir());
    char cover_background[2048];
    sprintf(cover_background, "%sbackground/bg_white_blue.jpg", this->gss_config.get_image_dir());

    char section1_image[2048];

    if (this->chip_type == "CRD")//syhwah 2013-04-22
       sprintf(section1_image, "%sSection/Section1_CRD_%s.pdf", this->gss_config.get_image_dir(),_lang_code.c_str()); 
    else
       sprintf(section1_image, "%sSection/Section1_%s.pdf", this->gss_config.get_image_dir(),_lang_code.c_str());

    char section2_image[2048];
    sprintf(section2_image, "%sSection/Section2_%s.pdf", this->gss_config.get_image_dir(),_lang_code.c_str());
    char section3_image[2048];
    sprintf(section3_image, "%sSection/Section3_%s.pdf", this->gss_config.get_image_dir(),_lang_code.c_str());

    char letter_head[2048];
    sprintf(letter_head, "%sletterhead/letterhead.pdf", this->gss_config.get_image_dir());

    xml_ret = gss_xml_writer.insert_images(
            main_logo,
            product_logo,
            company_logo,
            background,
            chip_logo,
            arrow,
            disease_logo,
            cover_logo,
            cover_background,
            section1_image,
            section2_image,
            section3_image,
            letter_head);

    assert(xml_ret == true);

    //-----------------------------------------------------------------

    string key_patient_name, key_patient_id, key_patient_add, key_sample_id, 
           key_patient_gender, key_patient_dob, cover_TextID, cover_title,// syhwah 2013-03-13
           key_primary_sample_type, key_sample_collection_date, key_sample_receipt_date,
           key_report_date, key_reference_no, key_doctor_name, key_doctor_add,
           label_your_drug_profile; //syhwah 2013-04-21

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "patient_name", key_patient_name);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "patient_id", key_patient_id);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "patient_add", key_patient_add);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "patient_gender", key_patient_gender);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "patient_dob", key_patient_dob);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "sample_id", key_sample_id);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "primary_sample_type", key_primary_sample_type);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "sampling_date", key_sample_collection_date);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "sample_received_date", key_sample_receipt_date);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "report_date", key_report_date);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "reference_no", key_reference_no);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "doctor_name", key_doctor_name);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "doctor_address", key_doctor_add);
    assert(ret == true);

    cover_TextID = "cover_title_";
    cover_TextID.append(this->chip_type); 
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", cover_TextID.c_str(), cover_title);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "index_your_drug_profile", label_your_drug_profile);
    assert(ret == true);

    string report_date;
    this->dtect_report.get_date(report_date);

    xml_ret = gss_xml_writer.insert_general_information(
            key_patient_name.c_str(),
            this->patient_name.c_str(),
            key_patient_id.c_str(),
            this->patient_id.c_str(),
            key_patient_add.c_str(),
            this->patient_add.c_str(),
            key_patient_gender.c_str(),
            this->gender.c_str(),
            key_patient_dob.c_str(),
            this->dob.str().c_str(),       
            key_sample_id.c_str(),
            this->sample_id.c_str(),
            key_primary_sample_type.c_str(),
            this->specimen_description.c_str(),
            key_sample_collection_date.c_str(),
            this->sample_collection_date.str().c_str(),
            key_sample_receipt_date.c_str(),
            this->order_rcvd_date.str().c_str(),
            key_report_date.c_str(),
            report_date.c_str(),
            key_reference_no.c_str(),
            string(this->sample_id + " " + this->chip_type + this->chip_ver).c_str(),
            key_doctor_name.c_str(),
            this->physician_name.c_str(),
            key_doctor_add.c_str(),
            this->physician_add.c_str(),
            cover_title.c_str(), 
            this->chip_type.c_str(), 
            this->partner_code.c_str(), 
            label_your_drug_profile.c_str()); 

    assert(xml_ret == true);

    //-----------------------------------------------------------------

    string comp_name, comp_add, comp_contact, comp_url;
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "comp_name", comp_name);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "comp_add", comp_add);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "comp_contact", comp_contact);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "comp_url", comp_url);
    assert(ret == true);

    xml_ret = gss_xml_writer.insert_company(
            comp_name.c_str(),
            comp_add.c_str(),
            "Tel:",
            comp_contact.c_str(),
            comp_url.c_str());
    assert(xml_ret == true);

    //-----------------------------------------------------------------

    string letter_name, letter_content, letter_signature, forward_letter;
    
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "letter_name", letter_name);
    assert(ret == true);
    if(strcmp(this->chip_type.c_str(), "CRD") == 0)
    {
        ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "letter_content_cardio", letter_content);
        assert(ret == true);
    }
    else
    {
        ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "letter_content", letter_content);
        assert(ret == true);
    }
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "letter_signature", letter_signature);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "forward_letter", forward_letter);
    assert(ret == true);
    
    xml_ret = gss_xml_writer.insert_letter(
            letter_name.c_str(),
            letter_content.c_str(),
            letter_signature.c_str(),
            forward_letter.c_str());

    assert(xml_ret == true);
    
    //-----------------------------------------------------------------
    
    string limitation_title, limitation_text;
    
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "limitation_screening", limitation_title);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "limitation_content", limitation_text);
    assert(ret == true);
    
    xml_ret = gss_xml_writer.insert_limitation(
            limitation_title.c_str(),
            limitation_text.c_str());
    assert(xml_ret == true);

    //-----------------------------------------------------------------
    
    string term_title, term_contents;
    
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "title_terms", term_title);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "contents_terms", term_contents);
    assert(ret == true);
    
    xml_ret = gss_xml_writer.insert_term(term_title.c_str(),term_contents.c_str());
    assert(xml_ret == true);

    //-----------------------------------------------------------------
    
    xml_ret = gss_xml_writer.insert_summary();    
    assert(xml_ret == true);
    
    //-----------------------------------------------------------------
    
    string summ_incr_risk_title, summ_incr_risk_col1, summ_incr_risk_col2, summ_incr_risk_col3, summ_incr_risk_col4, summ_incr_risk_col5, summ_incr_risk_col6;

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "summ_increased_risk_title", summ_incr_risk_title);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_increased_risk_col1", summ_incr_risk_col1);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_increased_risk_col2", summ_incr_risk_col2);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_increased_risk_col3", summ_incr_risk_col3);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_increased_risk_col4", summ_incr_risk_col4);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_increased_risk_col5", summ_incr_risk_col5);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_increased_risk_col6", summ_incr_risk_col6);
    assert(ret == true);

    xml_ret = gss_xml_writer.insert_summ_increased_risk(
            summ_incr_risk_title.c_str(), summ_incr_risk_col1.c_str(), summ_incr_risk_col2.c_str(), 
            summ_incr_risk_col3.c_str(), summ_incr_risk_col4.c_str(), summ_incr_risk_col5.c_str(), 
            summ_incr_risk_col6.c_str());
    assert(xml_ret == true);
   
    //-----------------------------------------------------------------
    
    string summ_non_incr_risk_title, summ_non_incr_risk_col1, summ_non_incr_risk_col2, summ_non_incr_risk_col3, 
           summ_non_incr_risk_col4, summ_non_incr_risk_col5, summ_non_incr_risk_col6, summ_non_incr_risk_caption;

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "summ_non_increased_risk_title", summ_non_incr_risk_title);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_non_increased_risk_col1", summ_non_incr_risk_col1);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_non_increased_risk_col2", summ_non_incr_risk_col2);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_non_increased_risk_col3", summ_non_incr_risk_col3);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_non_increased_risk_col4", summ_non_incr_risk_col4);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_non_increased_risk_col5", summ_non_incr_risk_col5);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_non_increased_risk_col6", summ_non_incr_risk_col6);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_non_increased_risk_caption", summ_non_incr_risk_caption);
    assert(ret == true);

    xml_ret = gss_xml_writer.insert_summ_non_increased_risk(
            summ_non_incr_risk_title.c_str(), summ_non_incr_risk_col1.c_str(), summ_non_incr_risk_col2.c_str(),   
            summ_non_incr_risk_col3.c_str(), summ_non_incr_risk_col4.c_str(), summ_non_incr_risk_col5.c_str(), 
            summ_non_incr_risk_col6.c_str(), summ_non_incr_risk_caption.c_str());
    assert(xml_ret == true);    

    //-----------------------------------------------------------------
    
    string summ_drug_title, summ_drug_col1, summ_drug_col2, summ_drug_col3, summ_drug_col4;

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "summ_drug_title", summ_drug_title);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_drug_col1", summ_drug_col1);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_drug_col2", summ_drug_col2);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_drug_col3", summ_drug_col3);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "summ_drug_col4", summ_drug_col4);
    assert(ret == true);

    xml_ret = gss_xml_writer.insert_summ_drug(
            summ_drug_title.c_str(), summ_drug_col1.c_str(), summ_drug_col2.c_str(),summ_drug_col3.c_str(), summ_drug_col4.c_str());
    assert(xml_ret == true);

    //-----------------------------------------------------------------
    
    string lifestyle_title, lifestyle_contents1, lifestyle_contents2;
    char textid[1024];

    sprintf(textid,"%s_lifestyle_title",this->chip_type.c_str()); 
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", textid, lifestyle_title);
    assert(ret == true);  
    sprintf(textid,"%s_lifestyle_contents1",this->chip_type.c_str());
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", textid, lifestyle_contents1);
    assert(ret == true);
    sprintf(textid,"%s_lifestyle_contents2",this->chip_type.c_str());
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", textid, lifestyle_contents2);
    assert(ret == true);
    
    xml_ret = gss_xml_writer.insert_general_lifestyle(lifestyle_title.c_str(),lifestyle_contents1.c_str(), lifestyle_contents2.c_str());
    assert(xml_ret == true);

    //-----------------------------------------------------------------
    
    string interpretation_title, interpretation_contents1, interpretation_contents2;

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "interpretation_title", interpretation_title);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "interpretation_contents_1", interpretation_contents1);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "interpretation_contents_2", interpretation_contents2);
    assert(ret == true);
    
    xml_ret = gss_xml_writer.insert_interpretation(interpretation_title.c_str(),interpretation_contents1.c_str(),interpretation_contents2.c_str());
    assert(xml_ret == true);   
    
    //-----------------------------------------------------------------
    string riskscores_title, riskscores_contents1, riskscores_formula1_1, riskscores_formula1_2, riskscores_formula1_3,
           riskscores_contents2, riskscores_subtitle3, riskscores_contents3, riskscores_subtitle4, riskscores_contents4, 
           riskscores_subtitle5, riskscores_contents5, riskscores_contents6, riskscores_formula6, riskscores_contents7;

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "riskscores_title", riskscores_title);
    assert(ret == true); 
   
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_contents_1", riskscores_contents1);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_formula1_1", riskscores_formula1_1);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_formula1_2", riskscores_formula1_2);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_formula1_3", riskscores_formula1_3);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_contents_2", riskscores_contents2);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_subtitle3", riskscores_subtitle3);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_contents_3", riskscores_contents3);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_subtitle4", riskscores_subtitle4);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_contents_4", riskscores_contents4);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_subtitle5", riskscores_subtitle5);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_contents_5", riskscores_contents5);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_contents_6", riskscores_contents6);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_formula6", riskscores_formula6);
    assert(ret == true);

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "riskscores_contents_7", riskscores_contents7);
    assert(ret == true);
                              
    xml_ret = gss_xml_writer.insert_riskscores_def(riskscores_title.c_str(), riskscores_contents1.c_str(), riskscores_formula1_1.c_str(), riskscores_formula1_2.c_str(),
                                                   riskscores_formula1_3.c_str(), riskscores_contents2.c_str(), riskscores_subtitle3.c_str(), riskscores_contents3.c_str(),
                                                   riskscores_subtitle4.c_str(), riskscores_contents4.c_str(), riskscores_subtitle5.c_str(), riskscores_contents5.c_str(),
                                                   riskscores_contents6.c_str(), riskscores_formula6.c_str(), riskscores_contents7.c_str());
    assert(xml_ret == true);

    //----------------------------------------------------------------- 

    string section1, section2, section3, section1_header, section2_header, section3_header;
    
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "section_1_header", section1_header);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "section_1_title", section1);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "section_2_header", section2_header);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "section_2_title", section2);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "section_3_header", section3_header);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "TITLE", "section_3_title", section3);
    assert(ret == true);
    
    xml_ret = gss_xml_writer.insert_section(
            section1_header.c_str(),
            section1.c_str(),
            section2_header.c_str(),
            section2.c_str(),
            section3_header.c_str(),
            section3.c_str());

    assert(xml_ret == true);

    //-----------------------------------------------------------------
    
    string genetic_col1, genetic_col2, genetic_row1, genetic_row2;

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genetic_col1", genetic_col1);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genetic_col2", genetic_col2);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genetic_row1", genetic_row1);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genetic_row2", genetic_row2);
    assert(ret == true);

    xml_ret = gss_xml_writer.insert_genetic_tbl_titles(genetic_col1.c_str(), genetic_col2.c_str(), genetic_row1.c_str(), genetic_row2.c_str());

    assert(xml_ret == true);

    //-----------------------------------------------------------------

    string disease_table_title, drug_table_title, hdr_gene, hdr_snp, hdr_alleles, hdr_effect, hdr_ref, hdr_risk_allele, hdr_relative_risk, table_caption, no_variant;

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "table_title", disease_table_title);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "drug_table_title", drug_table_title);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genotype_table_hdr_gene", hdr_gene);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genotype_table_hdr_snp", hdr_snp);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genotype_table_hdr_genotype", hdr_alleles);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genotype_table_hdr_effect", hdr_effect);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genotype_table_reference", hdr_ref);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genotype_table_hdr_risk_allele", hdr_risk_allele);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "genotype_table_hdr_relative_risk", hdr_relative_risk);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "table_caption", table_caption);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "no_variant", no_variant);
    assert(ret == true);

    xml_ret = gss_xml_writer.insert_markers_tbl_titles(        
                disease_table_title.c_str(),
                drug_table_title.c_str(),
                hdr_gene.c_str(),
                hdr_snp.c_str(),
                hdr_alleles.c_str(),
                hdr_effect.c_str(),
                hdr_ref.c_str(),
                hdr_risk_allele.c_str(),
                hdr_relative_risk.c_str(),
                table_caption.c_str(),
                no_variant.c_str());

    assert(xml_ret == true);

    //-----------------------------------------------------------------

    string count_risk_markers, no_risk_markers, num_of_risk_markers, num_of_pharmacogenetic_markers, ref_url, no_citation, doc_sheet,tear_off_instruction;

    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "count_risk_markers", count_risk_markers);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "no_risk_markers", no_risk_markers);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "num_of_risk_markers", num_of_risk_markers);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "num_of_pharmacogenetic_markers", num_of_pharmacogenetic_markers);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "no_citation", no_citation);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "ref_url", ref_url);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "doctor_sheet", doc_sheet);
    assert(ret == true);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "tear_off_instruction", tear_off_instruction);
    assert(ret == true);


    xml_ret = gss_xml_writer.insert_titles(
                count_risk_markers.c_str(),
                no_risk_markers.c_str(),
                num_of_risk_markers.c_str(),
                num_of_pharmacogenetic_markers.c_str(),
                hdr_ref.c_str(),
                no_citation.c_str(),
                ref_url.c_str(),
                doc_sheet.c_str(),
                tear_off_instruction.c_str());

    assert(xml_ret == true);

    //-----------------------------------------------------------------

    map<string, QSGenPop>::const_iterator gbl_pop_cit = this->gbl_pop_map.find(this->testcode);
    assert(gbl_pop_cit != this->gbl_pop_map.end());
    
    DtectSort dtect_sort;

    // Compute and bin the distribution of risk markers in global population
    PopDistribution gbl_pop_dis;
    string type, dis_drug_id, dis_drug_name;
    double total_risk;
    while (this->dtect_report.get_result(type, dis_drug_id, total_risk) == true)
    {
        gbl_pop_cit->second.compute_n_bin(
                this->gss_config.get_max_bar(),
                this->gss_config.get_min_pct(),
                this->gss_config.get_cluster_target(),
                type,
                dis_drug_id,
                total_risk,
                gbl_pop_dis);

        // get the actual name of diseases/drugs in ENGLISH for sorting purpose
        ret = trans_db.get_content(
                this->trans_conf.get_doc_id(this->testcode, "dt"),
                "en",
                this->trans_conf.get_version(this->testcode),
                "DIS_DRUG_NAME",
                dis_drug_id.c_str(),
                dis_drug_name);
        assert(ret == true);
        dtect_sort.insert_name(type, dis_drug_name, dis_drug_id);
    }

    //-----------------------------------------------------------------

    while (dtect_sort.get_name(type, dis_drug_name, dis_drug_id) == true)
    {
        double relative_risk, Incidence; 
        int positive_markers, neutral_markers, negative_markers;
        char sex = 'B', trait = 'N';

        ret = this->dtect_report.find_result(type, dis_drug_id, sex, Incidence, trait, 
                                             total_risk, relative_risk, positive_markers, 
                                             neutral_markers, negative_markers);
        assert(ret == true);
        int total_marker = positive_markers + neutral_markers + negative_markers;

        if (this->gss_config.is_filtered_disdrug(dis_drug_id.c_str()) == true)
            continue;
        
        ret = trans_db.get_content(
                this->trans_conf.get_doc_id(this->testcode, "dt"),
                _lang_code.c_str(),
                this->trans_conf.get_version(this->testcode),
                "DIS_DRUG_NAME",
                dis_drug_id.c_str(),
                dis_drug_name);
        assert(ret == true);
       
        double relative_genetic_risk = Incidence * relative_risk;
        char acIncidence[1024];

        switch (sex){ 
          case 'M':
          case 'F':
          {
               string strSex,strSex_TranslatedText;
               strSex = (sex=='M')? "male":"female";

               ret = trans_db.get_content(
               "GS-LABEL",
               _lang_code.c_str(),
               this->trans_conf.get_version(this->testcode),
               "LABEL",
               strSex.c_str(),
               strSex_TranslatedText);
               assert(ret == true);

              sprintf(acIncidence,"%.2f (%s)",Incidence, strSex_TranslatedText.c_str());
              break;
          }
          case 'B': { sprintf(acIncidence,"%.2f",Incidence);  break; }
        }
  
        xml_ret = gss_xml_writer.insert_summary_results(
                type.c_str(),
                dis_drug_name.c_str(),
                dis_drug_id.c_str(),
                gbl_pop_cit->second.average(dis_drug_id),
                total_risk,
                relative_risk,
                relative_genetic_risk,
                acIncidence,  
                false, trait);
        assert(xml_ret == true);

    //-----------------------------------------------------------------

        string linked_disease = "empty", linked_dis_drug_id="";
        
        if(strcmp(this->chip_type.c_str(), "ONC") == 0)
        {
            if(strcmp(dis_drug_id.c_str(), "ONC_01_07") == 0) linked_dis_drug_id = "ONC_01_08";      //linked_disease = "Esophageal Squamous Cell Carcinoma (ESCC)";                 
            else if(strcmp(dis_drug_id.c_str(), "ONC_01_18") == 0) linked_dis_drug_id = "ONC_01_19"; //linked_disease = "Lung Adenocarcinoma";                      
            else if(strcmp(dis_drug_id.c_str(), "ONC_01_22") == 0) linked_dis_drug_id = "ONC_01_20"; //linked_disease = "Follicular Lymphoma";  
        }
        else if(strcmp(this->chip_type.c_str(), "PDI") == 0)
        {
            if(strcmp(dis_drug_name.c_str(), "PDI_01_10") == 0) linked_dis_drug_id = "ONC_01_08";    //linked_disease = "Invasive Pneumococcal Disease (IPD)"; 
        }

        if (!linked_dis_drug_id.empty()){
            ret = trans_db.get_content(
            this->trans_conf.get_doc_id(this->testcode, "dt"),
            _lang_code.c_str(),
            this->trans_conf.get_version(this->testcode),
            "DIS_DRUG_NAME",
            linked_dis_drug_id.c_str(),
            linked_disease);

            assert(ret == true);
        }
        
        xml_ret = gss_xml_writer.insert_dis_drug_risk(
                type.c_str(),
                dis_drug_name.c_str(),
                dis_drug_id.c_str(),
                linked_disease.c_str(),
                gbl_pop_dis.get_risk(dis_drug_id),
                total_marker,
                trait);

        assert(xml_ret == true);

    //-----------------------------------------------------------------

        unsigned min_marker = 0, max_marker = 0;
        double percent = 0.0;
        while (gbl_pop_dis.get_segment(dis_drug_id, min_marker, max_marker, percent))
        {
            char pct_txt[8] = {0};
            sprintf(pct_txt, "%.1f%%", percent);
            char mark_txt[16] = {0};
            if (min_marker == max_marker)
                sprintf(mark_txt, "%u", min_marker);
            else
                sprintf(mark_txt, "%u-%u", min_marker, max_marker);
            string seg_legend;
            if (type == "Trait")
                ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "plot_legend_trait", seg_legend);
            else
                ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "plot_legend_risk", seg_legend);
            assert(ret == true);

            size_t space = 6;
            size_t pop_var = seg_legend.find("${pop}");
            if (pop_var == string::npos)
            {
                pop_var = seg_legend.find("$ {pop}");
                space = 7;
            }
            assert(pop_var != string::npos);

            seg_legend.replace(pop_var, space, pct_txt);

            size_t mark_var = seg_legend.find("${marker}");
            assert(mark_var != string::npos);

            seg_legend.replace(mark_var, 9, mark_txt);

            xml_ret = gss_xml_writer.insert_segment(percent, min_marker, max_marker, seg_legend.c_str());
            assert(xml_ret == true);
        }

    //-----------------------------------------------------------------

        string no_marker_text, positive_text, neutral_text, negative_text, undefined_text, total_text;
        if (type == "Disease")
        {
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_disease_no_marker", no_marker_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_disease_protective", positive_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_disease_neutral", neutral_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_disease_risk_marker", negative_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_disease_und", undefined_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_total", total_text);
            assert(ret == true);
        }
        else if (type == "Trait")
        {
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_disease_no_marker", no_marker_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_trait_marker", positive_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_trait_marker", neutral_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_trait_marker", negative_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_trait_marker", undefined_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_trait_marker", total_text);
            assert(ret == true);
        }
        else if (type == "Drug")
        {
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_drug_no_marker", no_marker_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_drug_marker", positive_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_drug_marker", neutral_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_drug_marker", negative_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_drug_marker", undefined_text);
            assert(ret == true);
            ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "profile_drug_marker", total_text);
            assert(ret == true);
        }

        if (positive_markers)
        {
            char positive_marker_text[8] = {0};
            sprintf(positive_marker_text, "%d", positive_markers);
            positive_text.replace(positive_text.find("${marker}"), 9, positive_marker_text);
        }
        else
            positive_text = no_marker_text;

        if (neutral_markers)
        {
            char neutral_marker_text[8] = {0};
            sprintf(neutral_marker_text, "%d", neutral_markers);
            neutral_text.replace(neutral_text.find("${marker}"), 9, neutral_marker_text);
        }
        else
            neutral_text = no_marker_text;

        if (negative_markers)
        {
            char negative_marker_text[8] = {0};
            sprintf(negative_marker_text, "%d", negative_markers);
            negative_text.replace(negative_text.find("${marker}"), 9, negative_marker_text);
        }
        else
            negative_text = no_marker_text;

        undefined_text = no_marker_text;

        if (negative_markers)
        {
            char total_marker_text[8] = {0};
            sprintf(total_marker_text, "%d", positive_markers + negative_markers);
            total_text.replace(total_text.find("${marker}"), 9, total_marker_text);
        }
        else
            total_text = no_marker_text;

        if (type == "Drug")
        {
            positive_text.replace(positive_text.find("${drug}"), 7, dis_drug_name);
            neutral_text.replace(neutral_text.find("${drug}"), 7, dis_drug_name);
            negative_text.replace(negative_text.find("${drug}"), 7, dis_drug_name);
            undefined_text.replace(undefined_text.find("${drug}"), 7, dis_drug_name);
            total_text.replace(total_text.find("${drug}"), 7, dis_drug_name);
        }

        xml_ret = gss_xml_writer.insert_profiles(
                positive_markers,
                positive_text.c_str(),
                neutral_markers,
                neutral_text.c_str(),
                negative_markers,
                negative_text.c_str(),
                0,
                undefined_text.c_str(),
                positive_markers + negative_markers,
                total_text.c_str(),
                relative_risk);
        assert(xml_ret == true);

    //-----------------------------------------------------------------
        
        xml_ret = gss_xml_writer.insert_geno_effects_tag();
        assert(xml_ret == true);

    //-----------------------------------------------------------------
    
        set<size_t> citation_set;
 
        ret = this->var_db.reinit(); //reconnect(); //syhwah 2013-05-20
        assert(ret == true);

        // get effects, genotypes, risk and citations for this particular disease/drug
        this->dtect_report.rewind(type, dis_drug_id);
        string effect_id;
        double risk = 0.0;
        while (this->dtect_report.get_effect_genotype_citations(effect_id, risk))
        {
            // one effect obtained
            xml_ret = gss_xml_writer.insert_genoeffect(risk);
            assert(xml_ret == true);

            // get genotypes for this effect
            unsigned rsid;
            string alleles;
            string risk_allele;
            double relative_risk;
            while (this->dtect_report.get_genotype_by_effect(effect_id, rsid, alleles, risk_allele, relative_risk))
            {
                // one genotype obtained
                string gene_symbol;
                ret = this->var_db.get_gene_symbol(rsid, gene_symbol);
                assert(ret == true);
                if (gene_symbol.empty())
                    gene_symbol.assign("-");

                xml_ret = gss_xml_writer.insert_genotype(
                        gene_symbol.c_str(),
                        rsid,
                        alleles.c_str(),
                        risk_allele.c_str(),
                        relative_risk);
                assert(xml_ret == true);

                alleles.clear();
                risk_allele.clear();
            }

            string references;
            set<size_t> pmid_set;
            ret = this->dtect_report.get_pmid_by_effect(effect_id, pmid_set);
            assert(ret == true);
            for (set<size_t>::const_iterator cit = pmid_set.begin(); cit != pmid_set.end(); ++cit)
            {
                // keep a list of unique pmid for references (with author, title, etc)
                citation_set.insert(*cit);

                char t[64] = {0};
                if (cit == pmid_set.begin())
                    sprintf(t, "%lu", *cit);
                else
                    sprintf(t, ",%lu", *cit);
                references.append(t);
            }
            string report_effect;
            ret = trans_db.get_content(
                    this->trans_conf.get_doc_id(this->testcode, "dt"),
                    _lang_code.c_str(),
                    this->trans_conf.get_version(this->testcode),
                    "GENOTYPIC_EFFECT",
                    effect_id.c_str(),
                    report_effect);
            assert(ret == true);

            xml_ret = gss_xml_writer.insert_effect_ref(report_effect.c_str(), references.c_str());
            assert(xml_ret == true);
        }

        string no_variant;
        ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "no_variant", no_variant);
        assert(ret == true);

        xml_ret = gss_xml_writer.append_geno_effects();
        assert(xml_ret == true);

    //-----------------------------------------------------------------

        xml_ret = this->process_G3_content(gss_xml_writer, _lang_code, dis_drug_id, "Description");
        assert(xml_ret == true);

        if ((type == "Disease") || (type == "Trait"))
        {
             xml_ret = this->process_G3_content(gss_xml_writer, _lang_code, dis_drug_id, "Symptoms");
             assert(xml_ret == true);
             xml_ret = this->process_G3_content(gss_xml_writer, _lang_code, dis_drug_id, "Prevention");
             assert(xml_ret == true);
        }

        xml_ret = gss_xml_writer.append_G3_content();
        assert(xml_ret == true);

    //-----------------------------------------------------------------    

        xml_ret = gss_xml_writer.insert_references_tag();
        assert(xml_ret == true);

        for (set<size_t>::const_iterator cit = citation_set.begin(); cit != citation_set.end(); ++cit)
        {
            string author, title, journal, pubdate;
            ret = this->var_db.get_citation(unsigned(*cit), author, title, journal, pubdate);
            assert(ret == true);

            xml_ret = gss_xml_writer.insert_reference(
                    (*cit),
                    author.c_str(),
                    title.c_str(),
                    journal.c_str(),
                    pubdate.c_str());
            assert(xml_ret == true);
        }

        xml_ret = gss_xml_writer.append_references();
        assert(xml_ret == true);

    //-----------------------------------------------------------------

        if ((type == "Disease") || (type == "Trait"))
        {
            xml_ret = gss_xml_writer.append_disease();
            assert(xml_ret == true);
        }
        else if (type == "Drug")
        {
            xml_ret = gss_xml_writer.append_drug();
            assert(xml_ret == true);
        }
        else { assert(false); }
    }
    xml_ret = gss_xml_writer.insert_qr_code(this->qrcode_png_filename.c_str());
    assert(xml_ret == true);

    //-----------------------------------------------------------------
    
    string notes;
    
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version("label"), "LABEL", "last_notes", notes);
    assert(ret == true);
    xml_ret = gss_xml_writer.insert_notes(notes.c_str());
    assert(xml_ret == true);
                       
    //-----------------------------------------------------------------
    
    xml_ret = gss_xml_writer.write(this->xml_output_filename.c_str());
    assert(xml_ret == true);
    
    return true;
}


bool DtectMapper::process_G3_content(
        GssXmlWriter& _gss_xml_writer,
        const string& _lang_code,
        const string& _dis_drug_id,
        const char* _section)
{
    bool ret = this->gss_config.reinit();
    assert(ret == true);

    TransDb trans_db("TRANS_DB");
    ret = trans_db.init(
            this->gss_config.get_dbhost(trans_db.get_dbname()),
            this->gss_config.get_dbuser(trans_db.get_dbname()),
            this->gss_config.get_dbpswd(trans_db.get_dbname()));
    assert(ret == true);

    if (_section == NULL) return false;

    char section_id_upper[1024] = {0};
    char section_id_lower[1024] = {0};
    this->to_upper(section_id_upper, _section);
    this->to_lower(section_id_lower, _section);

    char text_type[1024] = {0};
    char text_id[1024] = {0};

    string title, text, source;

    sprintf(text_id, "title_%s", section_id_lower);
    ret = trans_db.get_content(this->trans_conf.get_doc_id("label"), _lang_code.c_str(), this->trans_conf.get_version(this->testcode), "TITLE", text_id, title);
    assert(ret == true);

    sprintf(text_type, "%s", section_id_upper);
    ret = trans_db.get_content(false, this->trans_conf.get_doc_id(this->testcode, "g3"), _lang_code.c_str(), this->trans_conf.get_version(this->testcode), text_type, _dis_drug_id.c_str(), text, source);
    assert(ret == true);

     return _gss_xml_writer.insert_G3_content(_section, title.c_str(), text.c_str());
}


bool DtectMapper::generate_pdf(const string& _lang_code) const //syhwah 2013-03-15 :- added _lang_code
{
    assert(this->xml_output_filename.empty() == false);

    char pdf_gen_command[MAX_LENGTH] = {0};
    const char *latex_log_file = this->gss_config.get_latex_logfile();


    char acLaTeX[1024],acParam[1024];

    strcpy(acParam," "); 
    strcpy(acLaTeX,this->trans_conf.get_latex());

    for (int i=0; acLaTeX[i] != 0; i++){
        if (acLaTeX[i] == ',') { 
           sprintf(&acParam[1],"%s ",&acLaTeX[i+1]);
           acLaTeX[i] = 0; break;  
        } 
    } 
 
    if (latex_log_file != NULL){
        sprintf(pdf_gen_command, "%s %s%s >> %s 2>> %s",
                acLaTeX,        
                this->xml_output_filename.c_str(),
                acParam,        
                latex_log_file,
                latex_log_file);
    } else {
        sprintf(pdf_gen_command, "%s %s%s",
                acLaTeX,       
                this->xml_output_filename.c_str(),
                acParam);
    }
 
    // generate PDF using XML
    int pdf_ret = -1;
    pdf_ret = system(pdf_gen_command);
    if (pdf_ret == 0)
        return true;
    return false;
}

long DtectMapper::hash_assay_id(const char* _assay_id)
{
    assert(_assay_id != NULL);
    size_t len = strlen(_assay_id);
    long hash = 0;
    for (size_t i = 0; i < len; ++i)
        hash += long(_assay_id[i]) * 3;
    return hash;
}
