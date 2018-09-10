/* 
 * File:   GssXmlWriter.h
 * Author: jameswong
 *
 * Created on October 30, 2012, 5:05 PM
 */

#ifndef GSSXMLWRITER_H
#define	GSSXMLWRITER_H

#include "XmlWrite.h"
#include <set>

class GssXmlWriter : public XmlWrite
{
private:
    DOMElement *diseases_elem;
    DOMElement *drugs_elem;
    
    string type;
    DOMElement *disdrug_elem;
    DOMElement *segments_elem;
    
    DOMElement *summ_elem;
    DOMElement *summ_results_elem;
    
    DOMElement *geno_effects_elem;
    DOMElement *geno_effect_elem;
    DOMElement *genotypes_elem;
    
    DOMElement *references_elem;
    DOMElement *images_elem;
    //DOMElement *ext_contents_elem;

    DOMElement *G3Contents_elem;
    
public:
    GssXmlWriter();
    ~GssXmlWriter();
    
    bool initialise();
    
    bool write(
            const char *_filename);
    
    bool insert_metadata(
            const char *_lang_code,
            const char *_content_date);
    
    bool insert_images(
            const char *_main_logo,
            const char *_product_logo,
            const char *_company_logo,
            const char *_background,
            const char *_chip_logo,
            const char *_arrow,
            const char *_disease_logo,
            const char *_dtect_cover,
            const char *_bg_cover,
            const char *_section1_image,
            const char *_section2_image,
            const char *_section3_image,
            const char *_letter_head);
    
    bool insert_qr_code(
            const char *_qr);
    
    bool insert_general_information(
            const char *_key_patient_name,
            const char *_patient_name,
            const char *_key_patient_id,
            const char *_patient_id,
            const char *_key_patient_add,
            const char *_patient_add,
            const char* _key_patient_gender,  
            const char* _patient_gender,      
            const char* _key_patient_dob,     
            const char* _patient_dob,         
            const char* _patient_age,
            const char *_key_sample_id,
            const char *_sample_id,
            const char *_key_primary_sample_type,
            const char *_primary_sample_type,
            const char *_key_sample_collection_date,
            const char *_sample_collection_date,
            const char *_key_sample_receipt_date,
            const char *_sample_receipt_date,
            const char *_key_report_date,
            const char *_report_date,
            const char *_key_reference_no,
            const char *_reference_no,
            const char *_key_doctor_name,
            const char *_doctor_name,
            const char *_key_doctor_add,
            const char *_doctor_add,
            const char* _cover_title,
            const char* _chip_type, 
            const char* _partner_code, 
            const char* _label_your_drug_profile); 
    
    bool insert_company(
            const char *_name,
            const char *_address,
            const char *_contact_key,
            const char *_contact,
            const char *_url);
    
    bool insert_letter(
            const char *_letter_name,
            const char *_letter_content,
            const char *_letter_signature,
            const char *forward_letter);
    
    bool insert_limitation(
            const char* _limitation_title,
            const char* _limitation_text);
    
    bool insert_term(
            const char *_term_title,
            const char *_term_contents);

    bool insert_summ_increased_risk(
            const char *_summ_incr_risk_title,
            const char *_summ_incr_risk_col1,
            const char *_summ_incr_risk_col2,
            const char *_summ_incr_risk_col3,
            const char *_summ_incr_risk_col4,
            const char *_summ_incr_risk_col5,
            const char *_summ_incr_risk_col6,
            const char *_summ_incr_risk_note1,
            const char *_summ_incr_risk_note2,
            const char *_summ_incr_risk_note3,
            const char *_summ_incr_risk_dr_note1,
            const char *_summ_incr_risk_dr_note2,
            const char *_summ_incr_risk_dr_note3);

    bool insert_summ_non_increased_risk(
            const char *_summ_non_incr_risk_title,
            const char *_summ_non_incr_risk_col1,
            const char *_summ_non_incr_risk_col2,
            const char *_summ_non_incr_risk_col3,
            const char *_summ_non_incr_risk_col4,
            const char *_summ_non_incr_risk_col5,
            const char *_summ_non_incr_risk_col6,
            const char *_summ_non_incr_risk_caption,
            const char *_summ_non_incr_risk_note);

   bool insert_genetic_tbl_titles(
            const char *_genetic_col1, 
            const char *_genetic_col2, 
            const char *_genetic_col3,
            const char *_genetic_row1, 
            const char *_genetic_row2);
        
   bool insert_graph_writeup(
            const char *_writeup);

   bool insert_summ_drug(
            const char *_summ_drug_title, 
            const char *_summ_drug_col1, 
            const char *_summ_drug_col2,
            const char *_summ_drug_col3, 
            const char *_summ_drug_col4);

    bool insert_general_lifestyle(
            const char *_lifestyle_title,
            const char *_lifestyle_contents1,
            const char *_lifestyle_contents2);
 
    bool insert_interpretation(
            const char* _interpretation_title, 
            const char* _interpretation_contents_1,
            const char* _interpretation_contents_2);

    bool insert_riskscores_def(
            const char *_title,
            const char *_patient_subtitle1,
            const char *_patient_contents1,
            const char *_patient_formula1_1,
            const char *_patient_formula1_2,
            const char *_patient_formula1_3,
            const char *_patient_contents2,
            const char *_patient_subtitle3,
            const char *_patient_contents3,
            const char *_patient_formula3,
            const char *_patient_contents4,
            const char *_doctor_subtitle1,
            const char *_doctor_contents1,
            const char *_doctor_formula1_1,
            const char *_doctor_formula1_2,
            const char *_doctor_formula1_3,
            const char *_doctor_contents2,
            const char *_doctor_subtitle3,
            const char *_doctor_contents3,
            const char *_doctor_subtitle4,
            const char *_doctor_contents4,
            const char *_doctor_subtitle5,
            const char *_doctor_contents5,
            const char *_doctor_subtitle6,
            const char *_doctor_contents6,
            const char *_doctor_formula6,
            const char *_doctor_contents7);
 
    bool insert_section(
            const char *_section_1_header,
            const char *_section_1_title,
            const char *_section_2_header,
            const char *_section_2_title,
            const char *_section_3_header,
            const char *_section_3_title);

    bool insert_markers_tbl_titles(
            const char* _disease_table_title,
            const char* _drug_table_title,
            const char* _hdr_gene,
            const char* _hdr_snp,
            const char* _hdr_alleles,
            const char*_hdr_effect,
            const char* _hdr_ref,
            const char* _hdr_risk_allele,
            const char* _hdr_relative_risk,
            const char* _table_caption,
            const char* _no_variant);
   
    bool insert_titles(
            const char* _count_risk_markers,
            const char* _no_risk_markers,
            const char *_num_of_risk_markers,
            const char* _num_of_pharmacogenetic_markers,
            const char* _references,
            const char* _nocitation,
            const char* _ref_url,
            const char* _doc_sheet,
            const char* _tear_off_instruction);
 
    bool insert_summary();
    
    bool insert_summary_results(
            const char* _type,
            const char* _dis_drug,
            const char* _dis_drug_id,
            double _gbl_pop,
            double _risk,
            double _rel_risk,
            double _rel_genetic_risk,
            const char *_Incidence,
            bool _und,
            const char _trait);
    
    bool insert_dis_drug_risk(
            const char *_type,
            const char *_dis_drug,
            const char *_dis_drug_id,
            const char *_linked_disease,
            double _your_risk,
            int _total_marker,
            const char _trait);
    
    bool insert_profiles(
            int _positive,
            const char *_positive_text,
            int _neutral,
            const char *_neutral_text,
            int _negative,
            const char *_negative_text,
            int _undefined,
            const char *_undefined_text,
            int _total,
            const char *_total_text,
            double _rel_risk);
    
    bool insert_segment(
            double _pop_percent,
            unsigned _min_marker,
            unsigned _max_marker,
            const char *_text);
    
    bool append_disease();
    
    bool append_drug();
    
    bool insert_genoeffect();
    
    bool insert_genoeffect(
            double _risk);

    bool insert_geno_effects_tag();
    
    bool insert_genotype(
            const char* _gene,
            unsigned _rsid,
            const char* _alleles,
            const char* _risk_allele,
            double _rel_risk);
    
    bool insert_effect_ref(
            const char *_effect,
            const char *_reference);
    
    bool insert_novariant(
            const char *_novariant);
    
    bool append_geno_effects();
    
    bool insert_description(
            const char *_title,
            const char *_text);
    
    bool insert_references_tag();
    
    bool insert_reference(
            size_t _pmid,
            const char *_author,
            const char *_title,
            const char *_journal,
            const char *_pubdate);
    
    bool append_references();

    bool insert_G3_content(
	    const char* _tag,
            const char* _title,
            const char* _content);

    bool append_G3_content();

    bool insert_notes(const char *_notes);
};

#endif	/* GSSXMLWRITER_H */
