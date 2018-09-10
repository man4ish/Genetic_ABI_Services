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
    DOMElement *ext_contents_elem;
    
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
            const char* _key_patient_gender,  //syhwah 2013-03-13
            const char* _patient_gender,      //syhwah 2013-03-13
            const char* _key_patient_dob,     //syhwah 2013-03-13
            const char* _patient_dob,         //syhwah 2013-03-13
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
            const char* _InterpretResult,
            const char* _cover_title, //syhwah 2013-03-13 : _InterpretResult
            const char* _chip_type, //wuntyng 2013-03-27
            const char* _partner_code, //syhwah 2013-03-14
            const char* _label_your_drug_profile); //syhwah 2013-03-21
    
    bool insert_titles(
            const char *_box_plot_caption,
            const char *_box_plot_title,
            const char *_box_plot_subtitle,
            const char *_drug_title,
            const char *_legend_increased_risk,
            const char *_legend_normal_risk,
            const char *_legend_reduced_risk,
            const char *_legend_your_risk,
            const char *_table_caption,
            const char *_typical,
            const char *_slightly,
            const char *_increased,
            const char *_table_title,
            const char *_drug_table_title,
            const char *_overall_relative_risk,
            const char *_overall_risk_marker,
            const char *_overall_typical_risk);
    
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
            const char *_term_content_1,
            const char *_term_content_2,
            const char *_term_content_3,
            const char *_term_content_4,
            const char *_term_content_5);
    
    bool insert_section(
            const char *_section_1_header,
            const char *_section_1_title,
            const char *_section_2_header,
            const char *_section_2_title,
            const char *_section_3_header,
            const char *_section_3_title);
    
    bool insert_summary(
            const char *_your_genetic_summary,
            const char *_your_comparative_risk,
            const char *_box_plot_interpretation_upper,
            const char *_box_plot_interpretation_1_1,
            const char *_box_plot_interpretation_1_2,
            const char *_box_plot_interpretation_2_1,
            const char *_box_plot_interpretation_2_2,
            const char *_box_plot_interpretation_3_1,
            const char *_box_plot_interpretation_3_2,
            const char *_box_plot_interpretation_lower,
            const char *_box_plot_interpretation_footnote,
            const char *_title,
            const char *_pop_zero_note,
            const char *_pat_zero_note,
            const char *_und_note,
            const char *_risk_footnote1,
            const char *_risk_footnote2,
            const char *_risk_footnote3,
            const char *_pat_legend,
            const char *_pop_legend,
            const char *_disclaimer,
            const char *_axis,
            const char * _table_risk_description,
            const char *_genetic_summary_title,
            const char *_genetic_h_1,
            const char *_genetic_h_2,
            const char *_genetic_h_3,
            const char *_genetic_h_4,
            const char *_genetic_h_5,
            const char *_summary_no_risk,
            const char *_genetic_caption,
            const char *_genetic_content,
            const char *_drug_summary_title,
            const char *_drug_h_1,
            const char *_drug_h_2,
            const char *_drug_h_3,
            const char *_drug_h_4,
            const char *_drug_caption);
    
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
    
    bool append_disease(
            const char *_title,
            const char *_risk_title,
            const char *_path_risk_logo,
            const char *_path_question_logo,
            const char *_path_book_logo);
    
    bool append_drug(
            const char *_profile_title,
            const char *_path_drug_logo,
            const char *_path_question_logo,
            const char *_path_book_logo);
    
    bool insert_geno_effects_hdr(
            const char *_title,
            const char *_hdr_gene,
            const char *_hdr_snp,
            const char *_hdr_alleles,
            const char *_hdr_effect,
            const char *_hdr_ref,
            const char *_hdr_risk_allele,
            const char *_hdr_relative_risk,
            const char *_footnote);
    
    bool insert_genoeffect();
    
    bool insert_genoeffect(
            double _risk);
    
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
    
    bool insert_references(
            const char *_title);
    
    bool insert_reference(
            size_t _pmid,
            const char *_author,
            const char *_title,
            const char *_journal,
            const char *_pubdate);
    
    bool insert_nocitation(
            const char *_nocitation);
    
    bool append_references();
    
    bool insert_ext_content(
            const char *_logo,
            const char *_tag,
            const char *_title,
            const char *_image,
            const char *_caption,
            const char *_content);
    

    bool insert_ext_content_multi(
            const char *_logo,
            const char *_tag,
            const char *_title,
            const char *_img_dir,
            set<string> _img_set,              
            set<string> _cap_set,
            const char *_content);

    bool append_ext_content();
    
    bool insert_notes(
            const char *_notes);
};

#endif	/* GSSXMLWRITER_H */
