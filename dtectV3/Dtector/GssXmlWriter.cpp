/* 
 * File:   GssXmlWriter.cpp
 * Author: Manish
 * 
 * Created on October 30, 2012, 5:05 PM
 */

#include <stdio.h>
#include <bitset>
#include <ios>
#include "GssXmlWriter.h"

GssXmlWriter::GssXmlWriter()
{
    this->diseases_elem = NULL;
    this->drugs_elem = NULL;
    
    this->disdrug_elem = NULL;
    this->type.clear();
    //this->ext_contents_elem = NULL;

    this->G3Contents_elem = NULL;
    
    this->segments_elem = NULL;
    this->summ_elem = NULL;
    this->summ_results_elem = NULL;
    this->geno_effects_elem = NULL;
    this->geno_effect_elem = NULL;
    this->genotypes_elem = NULL;
    this->references_elem = NULL;
    this->images_elem = NULL;
}

GssXmlWriter::~GssXmlWriter()
{
}

bool GssXmlWriter::initialise()
{
    return this->init();
}

bool GssXmlWriter::write(
        const char* _filename)
{
    bool ret = this->output(_filename);
    if (ret == false) return false;
    
    this->diseases_elem = NULL;
    this->drugs_elem = NULL;
    this->disdrug_elem = NULL;
    this->type.clear();
    return true;
}

bool GssXmlWriter::insert_metadata(
        const char* _lang_code,
        const char* _content_date)
{
    DOMElement *meta_elem = this->createElement("Metadata");
    if (meta_elem == NULL) return false;
    
    DOMElement *lang_elem = this->createElement("Language", _lang_code);
    if (lang_elem == NULL) return false;
    if (meta_elem->appendChild(lang_elem) == NULL) return false;
    
    DOMElement *date_elem = this->createElement("ContentDate", _content_date);
    if (date_elem == NULL) return false;
    if (meta_elem->appendChild(date_elem) == NULL) return false;
    
    if (this->appendToRoot(meta_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_images(
        const char* _main_logo,
        const char* _product_logo,
        const char* _company_logo,
        const char* _background,
        const char* _chip_logo,
        const char* _arrow,
        const char* _disease_logo,
        const char *_dtect_cover,
        const char *_bg_cover,
        const char *_section1_image,
        const char *_section2_image,
        const char *_section3_image,
        const char *_letter_head)
{
    this->images_elem = this->createElement("Images");
    if (this->appendToRoot(this->images_elem) == NULL) return false;
    
    DOMElement *main_logo_elem = this->createElement("MainLogo", _main_logo);
    if (main_logo_elem == NULL) return false;
    if (this->images_elem->appendChild(main_logo_elem) == NULL) return false;
    
    DOMElement *product_logo_elem = this->createElement("ProductLogo", _product_logo);
    if (product_logo_elem == NULL) return false;
    if (this->images_elem->appendChild(product_logo_elem) == NULL) return false;
    
    DOMElement *company_logo_elem = this->createElement("CompanyLogo", _company_logo);
    if (company_logo_elem == NULL) return false;
    if (this->images_elem->appendChild(company_logo_elem) == NULL) return false;
    
    DOMElement *background_elem = this->createElement("Background", _background);
    if (background_elem == NULL) return false;
    if (this->images_elem->appendChild(background_elem) == NULL) return false;
    
    DOMElement *chip_logo_elem = this->createElement("ChipLogo", _chip_logo);
    if (_chip_logo == NULL) return false;
    if (this->images_elem->appendChild(chip_logo_elem) == NULL) return false;
    
    DOMElement *arrow_elem = this->createElement("Arrow", _arrow);
    if (arrow_elem == NULL) return false;
    if (this->images_elem->appendChild(arrow_elem) == NULL) return false;
    
    DOMElement *disease_logo_elem = this->createElement("DiseaseLogo", _disease_logo);
    if (disease_logo_elem == NULL) return false;
    if (this->images_elem->appendChild(disease_logo_elem) == NULL) return false;
    
    DOMElement *cover_logo_elem = this->createElement("CoverLogo", _dtect_cover);
    if (cover_logo_elem == NULL) return false;
    if (this->images_elem->appendChild(cover_logo_elem) == NULL) return false;
    
    DOMElement *bg_cover_elem = this->createElement("CoverBackground", _bg_cover);
    if (bg_cover_elem == NULL) return false;
    if (this->images_elem->appendChild(bg_cover_elem) == NULL) return false;
    
    DOMElement *sec1_img_elem = this->createElement("Section1", _section1_image);
    if (sec1_img_elem == NULL) return false;
    if (this->images_elem->appendChild(sec1_img_elem) == NULL) return false;

    DOMElement *sec2_img_elem = this->createElement("Section2", _section2_image);
    if (sec2_img_elem == NULL) return false;
    if (this->images_elem->appendChild(sec2_img_elem) == NULL) return false;

    DOMElement *sec3_img_elem = this->createElement("Section3", _section3_image);
    if (sec3_img_elem == NULL) return false;
    if (this->images_elem->appendChild(sec3_img_elem) == NULL) return false;

    DOMElement *letter_head_elem = this->createElement("LetterHead", _letter_head);
    if (letter_head_elem == NULL) return false;
    if (this->images_elem->appendChild(letter_head_elem) == NULL) return false;

    return true;
}

bool GssXmlWriter::insert_qr_code(
        const char* _qr)
{
    if (_qr == NULL) return false;
    if (this->images_elem == NULL) return false;
    DOMElement *qr_elem = this->createElement("QrCode", _qr);
    if (qr_elem == NULL) return false;
    if (this->images_elem->appendChild(qr_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_general_information(
        const char* _key_patient_name,
        const char* _patient_name,
        const char* _key_patient_id,
        const char* _patient_id,
        const char* _key_patient_add,
        const char* _patient_add,
        const char* _key_patient_gender,  
        const char* _patient_gender,      
        const char* _key_patient_dob,     
        const char* _patient_dob,         
        const char* _patient_age, 
        const char* _key_sample_id,
        const char* _sample_id,
        const char* _key_primary_sample_type,
        const char* _primary_sample_type,
        const char* _key_sample_collection_date,
        const char* _sample_collection_date,
        const char* _key_sample_receipt_date,
        const char* _sample_receipt_date,
        const char* _key_report_date,
        const char* _report_date,
        const char* _key_reference_no,
        const char* _reference_no,
        const char* _key_doctor_name,
        const char* _doctor_name,
        const char* _key_doctor_add,
        const char* _doctor_add,
        const char* _cover_title, 
        const char* _cover_assertion,
        const char* _chip_type, 
        const char* _partner_code, 
        const char* _label_your_drug_profile)  
{
    DOMElement *gen_info_elem = this->createElement("GeneralInformation");
    if (gen_info_elem == NULL) return false;
    
    DOMElement *patient_name_elem = this->createElement("PatientName", "key", _key_patient_name, _patient_name);
    if (patient_name_elem == NULL) return false;
    if (gen_info_elem->appendChild(patient_name_elem) == NULL) return false;
    
    DOMElement *patient_id_elem = this->createElement("PatientId", "key", _key_patient_id, _patient_id);
    if (patient_id_elem == NULL) return false;
    if (gen_info_elem->appendChild(patient_id_elem) == NULL) return false;
    
    DOMElement *patient_add_elem = this->createElement("PatientAddress", "key", _key_patient_add, _patient_add);
    if (patient_add_elem == NULL) return false;
    if (gen_info_elem->appendChild(patient_add_elem) == NULL) return false;
        
    DOMElement *patient_gender_elem = this->createElement("PatientGender", "key", _key_patient_gender, _patient_gender);
    if (patient_gender_elem == NULL) return false;
    if (gen_info_elem->appendChild(patient_gender_elem) == NULL) return false;

    DOMElement *patient_dob_elem = this->createElement("PatientDOB", "key", _key_patient_dob, _patient_dob);
    if (patient_dob_elem == NULL) return false;
    if (gen_info_elem->appendChild(patient_dob_elem) == NULL) return false; 

    DOMElement *patient_age_elem = this->createElement("PatientAge", _patient_age);
    if (patient_age_elem == NULL) return false;
    if (gen_info_elem->appendChild(patient_age_elem) == NULL) return false;
    //-----------------


    DOMElement *sample_id_elem = this->createElement("SampleId", "key", _key_sample_id, _sample_id);
    if (sample_id_elem == NULL) return false;
    if (gen_info_elem->appendChild(sample_id_elem) == NULL) return false;
    
    DOMElement *primary_sample_elem = this->createElement("PrimarySampleType", "key", _key_primary_sample_type, _primary_sample_type);
    if (primary_sample_elem == NULL) return false;
    if (gen_info_elem->appendChild(primary_sample_elem) == NULL) return false;
    
    DOMElement *collection_date_elem = this->createElement("SampleCollectionDate", "key", _key_sample_collection_date, _sample_collection_date);
    if (collection_date_elem == NULL) return false;
    if (gen_info_elem->appendChild(collection_date_elem) == NULL) return false;
    
    DOMElement *receipt_date_elem = this->createElement("SampleReceiptDate", "key", _key_sample_receipt_date, _sample_receipt_date);
    if (receipt_date_elem == NULL) return false;
    if (gen_info_elem->appendChild(receipt_date_elem) == NULL) return false;
    
    DOMElement *report_date_elem = this->createElement("ReportDate", "key", _key_report_date, _report_date);
    if (report_date_elem == NULL) return false;
    if (gen_info_elem->appendChild(report_date_elem) == NULL) return false;
    
    DOMElement *reference_elem = this->createElement("ReferenceNo", "key", _key_reference_no, _reference_no);
    if (reference_elem == NULL) return false;
    if (gen_info_elem->appendChild(reference_elem) == NULL) return false;
    
    DOMElement *doc_name_elem = this->createElement("DoctorName", "key", _key_doctor_name, _doctor_name);
    if (doc_name_elem == NULL) return false;
    if (gen_info_elem->appendChild(doc_name_elem) == NULL) return false;
    
    DOMElement *doc_add_elem = this->createElement("DoctorAddress", "key", _key_doctor_add, _doctor_add);
    if (doc_add_elem == NULL) return false;
    if (gen_info_elem->appendChild(doc_add_elem) == NULL) return false;

    DOMElement *cover_title_elem = this->createElement("CoverTitle", _cover_title);
    if (cover_title_elem == NULL) return false;
    if (gen_info_elem->appendChild(cover_title_elem) == NULL) return false;

    DOMElement *cover_assertion_elem = this->createElement("CoverAssertion", _cover_assertion);
    if (cover_assertion_elem == NULL) return false;
    if (gen_info_elem->appendChild(cover_assertion_elem) == NULL) return false;

    DOMElement *chip_type_elem = this->createElement("ChipType", _chip_type);
    if (chip_type_elem == NULL) return false;
    if (gen_info_elem->appendChild(chip_type_elem) == NULL) return false;

    DOMElement *partner_code_elem = this->createElement("PartnerCode", _partner_code);
    if (partner_code_elem == NULL) return false;
    if (gen_info_elem->appendChild(partner_code_elem) == NULL) return false;

    DOMElement *label_your_drug_profile_elem = this->createElement("Label_YourDrugProfile", _label_your_drug_profile);
    if (label_your_drug_profile_elem == NULL) return false;
    if (gen_info_elem->appendChild(label_your_drug_profile_elem) == NULL) return false;
    
    if (this->appendToRoot(gen_info_elem) == NULL) return false;

    return true;
}


bool GssXmlWriter::insert_company(
        const char* _name,
        const char* _address,
        const char* _contact_key,
        const char* _contact,
        const char* _url)
{
    DOMElement *comp_elem = this->createElement("Company");
    if (comp_elem == NULL) return false;

    DOMElement *name_elem = this->createElement("Name", _name);
    if (name_elem == NULL) return false;
    if (comp_elem->appendChild(name_elem) == NULL) return false;

    DOMElement *add_elem = this->createElement("Address", _address);
    if (add_elem == NULL) return false;
    if (comp_elem->appendChild(add_elem) == NULL) return false;

    DOMElement *contact_elem = this->createElement("Contact", "key", _contact_key, _contact);
    if (contact_elem == NULL) return false;
    if (comp_elem->appendChild(contact_elem) == NULL) return false;

    DOMElement *url_elem = this->createElement("Url", _url);
    if (url_elem == NULL) return false;
    if (comp_elem->appendChild(url_elem) == NULL) return false;

    if (this->appendToRoot(comp_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_letter(
        const char* _letter_name,
        const char* _letter_content,
        const char* _letter_signature,
        const char* _forward_letter)
{
    DOMElement *letter_elem = this->createElement("Letter");
    if (letter_elem == NULL) return false;
    
    DOMElement *letter_name_elem = this->createElement("LetterName", _letter_name);
    if (letter_name_elem == NULL) return false;
    if (letter_elem->appendChild(letter_name_elem) == NULL) return false;
    
    DOMElement *letter_content_elem = this->createElement("LetterContent", _letter_content);
    if (letter_content_elem == NULL) return false;
    if (letter_elem->appendChild(letter_content_elem) == NULL) return false;
    
    DOMElement *letter_signature_elem = this->createElement("LetterSignature", _letter_signature);
    if (letter_signature_elem == NULL) return false;
    if (letter_elem->appendChild(letter_signature_elem) == NULL) return false;

    DOMElement *forward_elem = this->createElement("Label_Forward", _forward_letter);
    if (forward_elem == NULL) return false;
    if (letter_elem->appendChild(forward_elem) == NULL) return false;
    
    if (this->appendToRoot(letter_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_limitation(
        const char* _limitation_title,
        const char* _limitation_text)
{
    DOMElement *limit_elem = this->createElement("Limitation");
    if (limit_elem == NULL) return false;

    DOMElement *limitation_title_elem = this->createElement("LimitationTitle", _limitation_title);
    if (limitation_title_elem == NULL) return false;
    if (limit_elem->appendChild(limitation_title_elem) == NULL) return false;

    DOMElement *limitation_text_elem = this->createElement("LimitationText", _limitation_text);
    if (limitation_text_elem == NULL) return false;
    if (limit_elem->appendChild(limitation_text_elem) == NULL) return false;

    if (this->appendToRoot(limit_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_term(const char* _term_title, const char* _term_contents)
{
    DOMElement *term_elem = this->createElement("Terms");
    if (term_elem == NULL) return false;
    
    DOMElement *term_title_elem = this->createElement("TermsTitle", _term_title);
    if (term_title_elem == NULL) return false;
    if (term_elem->appendChild(term_title_elem) == NULL) return false;
    
    DOMElement *term_contents_elem = this->createElement("TermsContents", _term_contents);
    if (term_contents_elem == NULL) return false;
    if (term_elem->appendChild(term_contents_elem) == NULL) return false;
    
    if (this->appendToRoot(term_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_summary(
        )
{
    this->summ_elem = this->createElement("Summary");
    if (this->summ_elem == NULL) return false;

    this->summ_results_elem = this->createElement("Results");
    if (this->summ_results_elem == NULL) return false;
    if (this->summ_elem->appendChild(this->summ_results_elem) == NULL) return false;

    if (this->appendToRoot(this->summ_elem) == NULL) return false;
    return true;
}


bool GssXmlWriter::insert_summ_increased_risk(
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
        const char *_summ_incr_risk_note4,
        const char *_summ_incr_risk_dr_note1,
        const char *_summ_incr_risk_dr_note2,
        const char *_summ_incr_risk_dr_note3,
        const char *_summ_incr_risk_dr_title)
{
    DOMElement *table_elem = this->createElement("IncreasedRiskTable");
    if (table_elem == NULL) return false;

    DOMElement *title_elem = this->createElement("Title", _summ_incr_risk_title);
    if (title_elem == NULL) return false;
    if (table_elem->appendChild(title_elem) == NULL) return false;

    DOMElement *col1_elem = this->createElement("Col1", _summ_incr_risk_col1);
    if (col1_elem == NULL) return false;
    if (table_elem->appendChild(col1_elem) == NULL) return false;

    DOMElement *col2_elem = this->createElement("Col2", _summ_incr_risk_col2);
    if (col2_elem == NULL) return false;
    if (table_elem->appendChild(col2_elem) == NULL) return false;

    DOMElement *col3_elem = this->createElement("Col3", _summ_incr_risk_col3);
    if (col3_elem == NULL) return false;
    if (table_elem->appendChild(col3_elem) == NULL) return false;

    DOMElement *col4_elem = this->createElement("Col4", _summ_incr_risk_col4);
    if (col4_elem == NULL) return false;
    if (table_elem->appendChild(col4_elem) == NULL) return false;

    DOMElement *col5_elem = this->createElement("Col5", _summ_incr_risk_col5);
    if (col5_elem == NULL) return false;
    if (table_elem->appendChild(col5_elem) == NULL) return false;

    DOMElement *col6_elem = this->createElement("Col6", _summ_incr_risk_col6);
    if (col6_elem == NULL) return false;
    if (table_elem->appendChild(col6_elem) == NULL) return false;

    DOMElement *note1_elem = this->createElement("Note1", _summ_incr_risk_note1);
    if (note1_elem == NULL) return false;
    if (table_elem->appendChild(note1_elem) == NULL) return false;

    DOMElement *note2_elem = this->createElement("Note2", _summ_incr_risk_note2);
    if (note2_elem == NULL) return false;
    if (table_elem->appendChild(note2_elem) == NULL) return false;

    DOMElement *note3_elem = this->createElement("Note3", _summ_incr_risk_note3);
    if (note3_elem == NULL) return false;
    if (table_elem->appendChild(note3_elem) == NULL) return false;

    DOMElement *note4_elem = this->createElement("Note4", _summ_incr_risk_note4);
    if (note4_elem == NULL) return false;
    if (table_elem->appendChild(note4_elem) == NULL) return false;

    DOMElement *dr_note1_elem = this->createElement("DoctorNote1", _summ_incr_risk_dr_note1);
    if (dr_note1_elem == NULL) return false;
    if (table_elem->appendChild(dr_note1_elem) == NULL) return false;

    DOMElement *dr_note2_elem = this->createElement("DoctorNote2", _summ_incr_risk_dr_note2);
    if (dr_note2_elem == NULL) return false;
    if (table_elem->appendChild(dr_note2_elem) == NULL) return false;

    DOMElement *dr_note3_elem = this->createElement("DoctorNote3", _summ_incr_risk_dr_note3);
    if (dr_note3_elem == NULL) return false;
    if (table_elem->appendChild(dr_note3_elem) == NULL) return false;

    DOMElement *dr_title_elem = this->createElement("DoctorTitle", _summ_incr_risk_dr_title);
    if (dr_title_elem == NULL) return false;
    if (table_elem->appendChild(dr_title_elem) == NULL) return false;

    if (this->summ_elem->appendChild(table_elem) == NULL) return false;
    return true;    
}


bool GssXmlWriter::insert_summ_non_increased_risk(
        const char *_summ_non_incr_risk_title,
        const char *_summ_non_incr_risk_col1,
        const char *_summ_non_incr_risk_col2,
        const char *_summ_non_incr_risk_col3,
        const char *_summ_non_incr_risk_col4,
        const char *_summ_non_incr_risk_col5,
        const char *_summ_non_incr_risk_col6,
        const char *_summ_non_incr_risk_caption,
        const char *_summ_non_incr_risk_note)
{
    DOMElement *table_elem = this->createElement("TypicalAndDecreasedRiskTable");
    if (table_elem == NULL) return false;

    DOMElement *title_elem = this->createElement("Title", _summ_non_incr_risk_title);
    if (title_elem == NULL) return false;
    if (table_elem->appendChild(title_elem) == NULL) return false;

    DOMElement *col1_elem = this->createElement("Col1", _summ_non_incr_risk_col1);
    if (col1_elem == NULL) return false;
    if (table_elem->appendChild(col1_elem) == NULL) return false;

    DOMElement *col2_elem = this->createElement("Col2", _summ_non_incr_risk_col2);
    if (col2_elem == NULL) return false;
    if (table_elem->appendChild(col2_elem) == NULL) return false;

    DOMElement *col3_elem = this->createElement("Col3", _summ_non_incr_risk_col3);
    if (col3_elem == NULL) return false;
    if (table_elem->appendChild(col3_elem) == NULL) return false;

    DOMElement *col4_elem = this->createElement("Col4", _summ_non_incr_risk_col4);
    if (col4_elem == NULL) return false;
    if (table_elem->appendChild(col4_elem) == NULL) return false;

    DOMElement *col5_elem = this->createElement("Col5", _summ_non_incr_risk_col5);
    if (col5_elem == NULL) return false;
    if (table_elem->appendChild(col5_elem) == NULL) return false;

    DOMElement *col6_elem = this->createElement("Col6", _summ_non_incr_risk_col6);
    if (col6_elem == NULL) return false;
    if (table_elem->appendChild(col6_elem) == NULL) return false;

    DOMElement *caption_elem = this->createElement("Caption", _summ_non_incr_risk_caption);
    if (caption_elem == NULL) return false;
    if (table_elem->appendChild(caption_elem) == NULL) return false;

    DOMElement *note_elem = this->createElement("Note2", _summ_non_incr_risk_note);
    if (note_elem == NULL) return false;
    if (table_elem->appendChild(note_elem) == NULL) return false;

    if (this->summ_elem->appendChild(table_elem) == NULL) return false;
    return true;
}


bool GssXmlWriter::insert_summ_drug(
        const char *_summ_drug_title,
        const char *_summ_drug_col1,
        const char *_summ_drug_col2,
        const char *_summ_drug_col3,
        const char *_summ_drug_col4)
{
    DOMElement *table_elem = this->createElement("DrugTable");
    if (table_elem == NULL) return false;

    DOMElement *title_elem = this->createElement("Title", _summ_drug_title);
    if (title_elem == NULL) return false;
    if (table_elem->appendChild(title_elem) == NULL) return false;

    DOMElement *col1_elem = this->createElement("Col1", _summ_drug_col1);
    if (col1_elem == NULL) return false;
    if (table_elem->appendChild(col1_elem) == NULL) return false;

    DOMElement *col2_elem = this->createElement("Col2", _summ_drug_col2);
    if (col2_elem == NULL) return false;
    if (table_elem->appendChild(col2_elem) == NULL) return false;

    DOMElement *col3_elem = this->createElement("Col3", _summ_drug_col3);
    if (col3_elem == NULL) return false;
    if (table_elem->appendChild(col3_elem) == NULL) return false;

    DOMElement *col4_elem = this->createElement("Col4", _summ_drug_col4);
    if (col4_elem == NULL) return false;
    if (table_elem->appendChild(col4_elem) == NULL) return false;

    if (this->summ_elem->appendChild(table_elem) == NULL) return false;
    return true;
}


bool GssXmlWriter::insert_summary_results(
        const char* _type,
        const char* _dis_drug,
        const char* _dis_drug_id,
        double _gbl_pop,
        double _risk,
        double _rel_risk,
        double _rel_genetic_risk,
        const char* _Incidence,
        bool _und,
        const char _trait)
{
    char acTrait[2]; acTrait[0]=_trait; acTrait[1]=0;
    DOMElement *result_elem = this->createElement("Result", "Name", _dis_drug, "Type", _type, "Subtype", acTrait);
    if (result_elem == NULL) return false;
    result_elem->setAttribute(this->transcode("ID"), this->transcode(_dis_drug_id));

    char patient_risk[32] = {0};
    sprintf(patient_risk, "%.2f", _risk);
    DOMElement *patient_elem = NULL;
    if (_und)
        patient_elem = this->createElement("Patient", "UND", "1", patient_risk);
    else
        patient_elem = this->createElement("Patient", "UND", "0", patient_risk);
    if (patient_elem == NULL) return false;;
    if (result_elem->appendChild(patient_elem) == NULL) return false;

    char pop_risk[32] = {0};
    sprintf(pop_risk, "%.2f", _gbl_pop);
    DOMElement *pop_elem = this->createElement("Population", pop_risk);
    if (pop_elem == NULL) return false;
    if (result_elem->appendChild(pop_elem) == NULL) return false;

    char rel_genetic_risk[32] = {0};
    sprintf(rel_genetic_risk, "%.2f", _rel_genetic_risk);
    rel_genetic_risk[31] = '\0';
    DOMElement *rel_genetic_elem = this->createElement("RelativeGeneticRisk", rel_genetic_risk);
    if (rel_genetic_elem == NULL) return false;
    if (result_elem->appendChild(rel_genetic_elem) == NULL) return false;

    DOMElement *Incidence_elem = this->createElement("Incidence", _Incidence);
    if (Incidence_elem == NULL) return false;
    if (result_elem->appendChild(Incidence_elem) == NULL) return false;

    char rel_risk[32] = {0};
    sprintf(rel_risk, "%.2f", _rel_risk);
    rel_risk[31] = '\0';
    DOMElement *rel_elem = this->createElement("Relative", rel_risk);
    if (rel_elem == NULL) return false;
    if (result_elem->appendChild(rel_elem) == NULL) return false;

    if (this->summ_results_elem->appendChild(result_elem) == false) return false;
    return true;
}


bool GssXmlWriter::insert_general_lifestyle(
        const char *_lifestyle_title,
        const char *_lifestyle_contents1,
        const char *_lifestyle_contents2)
{
    DOMElement *lifestyle_elem = this->createElement("GeneralLifeStyle");
    if (lifestyle_elem == NULL) return false;

    DOMElement *lifestyle_title_elem = this->createElement("Title", _lifestyle_title);
    if (lifestyle_title_elem == NULL) return false;
    if (lifestyle_elem->appendChild(lifestyle_title_elem) == NULL) return false;

    DOMElement *lifestyle_contents1_elem = this->createElement("Content1", _lifestyle_contents1);
    if (lifestyle_contents1_elem == NULL) return false;
    if (lifestyle_elem->appendChild(lifestyle_contents1_elem) == NULL) return false;

    DOMElement *lifestyle_contents2_elem = this->createElement("Content2", _lifestyle_contents2);
    if (lifestyle_contents2_elem == NULL) return false;
    if (lifestyle_elem->appendChild(lifestyle_contents2_elem) == NULL) return false;

    if (this->appendToRoot(lifestyle_elem) == NULL) return false;
    return true;

}

bool GssXmlWriter::insert_interpretation(
        const char *_interpretation_title, 
        const char *_interpretation_contents1,
        const char *_interpretation_contents2)
{
    DOMElement *interpretation_elem = this->createElement("Interpretation");	
    if (interpretation_elem == NULL) return false;

    DOMElement *interpretation_title_elem = this->createElement("Title", _interpretation_title);  
    if (interpretation_title_elem == NULL) return false; 
    if (interpretation_elem->appendChild(interpretation_title_elem) == NULL) return false;

    DOMElement *interpretation_contents1_elem = this->createElement("Content1", _interpretation_contents1);
    if (interpretation_contents1_elem == NULL) return false;
    if (interpretation_elem->appendChild(interpretation_contents1_elem) == NULL) return false;

    DOMElement *interpretation_contents2_elem = this->createElement("Content2", _interpretation_contents2);
    if (interpretation_contents2_elem == NULL) return false;
    if (interpretation_elem->appendChild(interpretation_contents2_elem) == NULL) return false;    

    if (this->appendToRoot(interpretation_elem) == NULL) return false;
    return true;
}


bool GssXmlWriter::insert_riskscores_def(
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
        const char *_doctor_contents7)
{
    DOMElement *riskscores_def_elem = this->createElement("RiskScoresDefinition");
    if (riskscores_def_elem == NULL) return false;

    DOMElement *title_elem = this->createElement("Title", _title);
    if (title_elem == NULL) return false;
    if (riskscores_def_elem->appendChild(title_elem) == NULL) return false;

    //-----------------------------------------------------------------------------
    
    DOMElement *patient_elem = this->createElement("PatientVersion", _title);
    if (patient_elem == NULL) return false;
    if (riskscores_def_elem->appendChild(patient_elem) == NULL) return false;
                
    
    DOMElement *patient_contents1_elem = this->createElement("Content1");
    if (patient_contents1_elem == NULL) return false;
    if (patient_elem->appendChild(patient_contents1_elem) == NULL) return false;

    DOMElement *patient_subtitle1_elem = this->createElement("SubTitle", _patient_subtitle1);
    if (patient_subtitle1_elem == NULL) return false;
    if (patient_contents1_elem->appendChild(patient_subtitle1_elem) == NULL) return false;

    DOMElement *patient_text1_elem = this->createElement("Text", _patient_contents1);
    if (patient_text1_elem == NULL) return false;
    if (patient_contents1_elem->appendChild(patient_text1_elem) == NULL) return false;

    DOMElement *patient_formula1_1_elem = this->createElement("Formula1", _patient_formula1_1);
    if (patient_formula1_1_elem == NULL) return false;
    if (patient_contents1_elem->appendChild(patient_formula1_1_elem) == NULL) return false;

    DOMElement *patient_formula1_2_elem = this->createElement("Formula2", _patient_formula1_2);
    if (patient_formula1_2_elem == NULL) return false;
    if (patient_contents1_elem->appendChild(patient_formula1_2_elem) == NULL) return false;

    DOMElement *patient_formula1_3_elem = this->createElement("Formula3", _patient_formula1_3);
    if (patient_formula1_3_elem == NULL) return false;
    if (patient_contents1_elem->appendChild(patient_formula1_3_elem) == NULL) return false;

    //-----------------------------------------------------------------------------
    
    DOMElement *patient_contents2_elem = this->createElement("Content2");
    if (patient_contents2_elem == NULL) return false;
    if (patient_elem->appendChild(patient_contents2_elem) == NULL) return false;

    DOMElement *patient_text2_elem = this->createElement("Text", _patient_contents2);
    if (patient_text2_elem == NULL) return false;
    if (patient_contents2_elem->appendChild(patient_text2_elem) == NULL) return false;

    //-----------------------------------------------------------------------------

    DOMElement *patient_contents3_elem = this->createElement("Content3");
    if (patient_contents3_elem == NULL) return false;
    if (patient_elem->appendChild(patient_contents3_elem) == NULL) return false;

    DOMElement *patient_subtitle3_elem = this->createElement("SubTitle", _patient_subtitle3);
    if (patient_subtitle3_elem == NULL) return false;
    if (patient_contents3_elem->appendChild(patient_subtitle3_elem) == NULL) return false;

    DOMElement *patient_text3_elem = this->createElement("Text", _patient_contents3);
    if (patient_text3_elem == NULL) return false;
    if (patient_contents3_elem->appendChild(patient_text3_elem) == NULL) return false;

    DOMElement *patient_formula3_elem = this->createElement("Formula", _patient_formula3);
    if (patient_formula3_elem == NULL) return false;
    if (patient_contents3_elem->appendChild(patient_formula3_elem) == NULL) return false;

    //-----------------------------------------------------------------------------
    
    DOMElement *patient_contents4_elem = this->createElement("Content4");
    if (patient_contents4_elem == NULL) return false;
    if (patient_elem->appendChild(patient_contents4_elem) == NULL) return false;

    DOMElement *patient_text4_elem = this->createElement("Text", _patient_contents4);
    if (patient_text4_elem == NULL) return false;
    if (patient_contents4_elem->appendChild(patient_text4_elem) == NULL) return false;

    //-----------------------------------------------------------------------------
    
    DOMElement *doctor_elem = this->createElement("DoctorVersion", _title);
    if (doctor_elem == NULL) return false;
    if (riskscores_def_elem->appendChild(doctor_elem) == NULL) return false;

    
    DOMElement *doctor_contents1_elem = this->createElement("Content1");
    if (doctor_contents1_elem == NULL) return false;
    if (doctor_elem->appendChild(doctor_contents1_elem) == NULL) return false;

    DOMElement *doctor_subtitle1_elem = this->createElement("SubTitle", _doctor_subtitle1);
    if (doctor_subtitle1_elem == NULL) return false;
    if (doctor_contents1_elem->appendChild(doctor_subtitle1_elem) == NULL) return false;

    DOMElement *doctor_text1_elem = this->createElement("Text", _doctor_contents1); 
    if (doctor_text1_elem == NULL) return false;
    if (doctor_contents1_elem->appendChild(doctor_text1_elem) == NULL) return false;

    DOMElement *doctor_formula1_1_elem = this->createElement("Formula1", _doctor_formula1_1);
    if (doctor_formula1_1_elem == NULL) return false;
    if (doctor_contents1_elem->appendChild(doctor_formula1_1_elem) == NULL) return false;

    DOMElement *doctor_formula1_2_elem = this->createElement("Formula2", _doctor_formula1_2);
    if (doctor_formula1_2_elem == NULL) return false;
    if (doctor_contents1_elem->appendChild(doctor_formula1_2_elem) == NULL) return false;

    DOMElement *doctor_formula1_3_elem = this->createElement("Formula3", _doctor_formula1_3);
    if (doctor_formula1_3_elem == NULL) return false;
    if (doctor_contents1_elem->appendChild(doctor_formula1_3_elem) == NULL) return false;

    //-----------------------------------------------------------------------------

    DOMElement *doctor_contents2_elem = this->createElement("Content2");
    if (doctor_contents2_elem == NULL) return false;
    if (doctor_elem->appendChild(doctor_contents2_elem) == NULL) return false;

    DOMElement *doctor_text2_elem = this->createElement("Text", _doctor_contents2); 
    if (doctor_text2_elem == NULL) return false;
    if (doctor_contents2_elem->appendChild(doctor_text2_elem) == NULL) return false;

    //-----------------------------------------------------------------------------

    DOMElement *doctor_contents3_elem = this->createElement("Content3");
    if (doctor_contents3_elem == NULL) return false;
    if (doctor_elem->appendChild(doctor_contents3_elem) == NULL) return false;

    DOMElement *doctor_subtitle3_elem = this->createElement("SubTitle", _doctor_subtitle3);
    if (doctor_subtitle3_elem == NULL) return false;
    if (doctor_contents3_elem->appendChild(doctor_subtitle3_elem) == NULL) return false;

    DOMElement *doctor_text3_elem = this->createElement("Text", _doctor_contents3);
    if (doctor_text3_elem == NULL) return false;
    if (doctor_contents3_elem->appendChild(doctor_text3_elem) == NULL) return false;

    //-----------------------------------------------------------------------------

    DOMElement *doctor_contents4_elem = this->createElement("Content4");
    if (doctor_contents4_elem == NULL) return false;
    if (doctor_elem->appendChild(doctor_contents4_elem) == NULL) return false;

    DOMElement *doctor_subtitle4_elem = this->createElement("SubTitle", _doctor_subtitle4);
    if (doctor_subtitle4_elem == NULL) return false;
    if (doctor_contents4_elem->appendChild(doctor_subtitle4_elem) == NULL) return false;

    DOMElement *doctor_text4_elem = this->createElement("Text", _doctor_contents4);
    if (doctor_text4_elem == NULL) return false;
    if (doctor_contents4_elem->appendChild(doctor_text4_elem) == NULL) return false;

    //-----------------------------------------------------------------------------

    DOMElement *doctor_contents5_elem = this->createElement("Content5");
    if (doctor_contents5_elem == NULL) return false;
    if (doctor_elem->appendChild(doctor_contents5_elem) == NULL) return false;
    
    DOMElement *doctor_subtitle5_elem = this->createElement("SubTitle", _doctor_subtitle5);
    if (doctor_subtitle5_elem == NULL) return false;
    if (doctor_contents5_elem->appendChild(doctor_subtitle5_elem) == NULL) return false;
    
    DOMElement *doctor_text5_elem = this->createElement("Text", _doctor_contents5);
    if (doctor_text5_elem == NULL) return false;
    if (doctor_contents5_elem->appendChild(doctor_text5_elem) == NULL) return false;
    
    //-----------------------------------------------------------------------------    

    DOMElement *doctor_contents6_elem = this->createElement("Content6");
    if (doctor_contents6_elem == NULL) return false;
    if (doctor_elem->appendChild(doctor_contents6_elem) == NULL) return false;

    DOMElement *doctor_subtitle6_elem = this->createElement("SubTitle", _doctor_subtitle6);
    if (doctor_subtitle6_elem == NULL) return false;
    if (doctor_contents6_elem->appendChild(doctor_subtitle6_elem) == NULL) return false;

    DOMElement *doctor_text6_elem = this->createElement("Text", _doctor_contents6);
    if (doctor_text6_elem == NULL) return false;
    if (doctor_contents6_elem->appendChild(doctor_text6_elem) == NULL) return false;

    DOMElement *doctor_formula6_elem = this->createElement("Formula", _doctor_formula6);
    if (doctor_formula6_elem == NULL) return false;
    if (doctor_contents6_elem->appendChild(doctor_formula6_elem) == NULL) return false;

    //-----------------------------------------------------------------------------    
 
    DOMElement *doctor_contents7_elem = this->createElement("Content7");
    if (doctor_contents7_elem == NULL) return false;
    if (doctor_elem->appendChild(doctor_contents7_elem) == NULL) return false;

    DOMElement *doctor_text7_elem = this->createElement("Text", _doctor_contents7);
    if (doctor_text7_elem == NULL) return false;
    if (doctor_contents7_elem->appendChild(doctor_text7_elem) == NULL) return false;

    //-----------------------------------------------------------------------------

    if (this->appendToRoot(riskscores_def_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_genetic_tbl_titles(
        const char *_genetic_col1,
        const char *_genetic_col2,
        const char *_genetic_col3,
        const char *_genetic_row1,
        const char *_genetic_row2,
        const char *_genetic_note)
{
    DOMElement *table_elem = this->createElement("GeneticTable");
    if (table_elem == NULL) return false;

    DOMElement *col1_elem = this->createElement("Col1", _genetic_col1);
    if (col1_elem == NULL) return false;
    if (table_elem->appendChild(col1_elem) == NULL) return false;

    DOMElement *col2_elem = this->createElement("Col2", _genetic_col2);
    if (col2_elem == NULL) return false;
    if (table_elem->appendChild(col2_elem) == NULL) return false;

    DOMElement *col3_elem = this->createElement("Col3", _genetic_col3);
    if (col3_elem == NULL) return false;
    if (table_elem->appendChild(col3_elem) == NULL) return false;

    DOMElement *row1_elem = this->createElement("Row1", _genetic_row1);
    if (row1_elem == NULL) return false;
    if (table_elem->appendChild(row1_elem) == NULL) return false;

    DOMElement *row2_elem = this->createElement("Row2", _genetic_row2);
    if (row2_elem == NULL) return false;
    if (table_elem->appendChild(row2_elem) == NULL) return false;

    DOMElement *note_elem = this->createElement("Note", _genetic_note);
    if (note_elem == NULL) return false;
    if (table_elem->appendChild(note_elem) == NULL) return false;

    if (this->appendToRoot(table_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_graph_writeup(
        const char *_writeup,
        const char *_label_GLR,
        const char *_label_age,
        const char *_label_you,
        const char *_label_general_population,
        const char *_label_current_status)
{
    DOMElement *graph_elem = this->createElement("Graph");
    if (graph_elem == NULL) return false;

    DOMElement *writeup_elem = this->createElement("Writeup", _writeup);
    if (writeup_elem == NULL) return false;
    if (graph_elem->appendChild(writeup_elem) == NULL) return false;

    DOMElement *label_GLR_elem = this->createElement("LabelGLR", _label_GLR);
    if (label_GLR_elem == NULL) return false;
    if (graph_elem->appendChild(label_GLR_elem) == NULL) return false;

    DOMElement *label_age_elem = this->createElement("LabelAge", _label_age);
    if (label_age_elem == NULL) return false;
    if (graph_elem->appendChild(label_age_elem) == NULL) return false;

    DOMElement *label_you_elem = this->createElement("LabelYou", _label_you);
    if (label_you_elem == NULL) return false;
    if (graph_elem->appendChild(label_you_elem) == NULL) return false;

    DOMElement *label_general_population_elem = this->createElement("LabelGeneralPopulation", _label_general_population);
    if (label_general_population_elem == NULL) return false;
    if (graph_elem->appendChild(label_general_population_elem) == NULL) return false;

    DOMElement *label_current_status_elem = this->createElement("LabelCurrentStatus", _label_current_status);
    if (label_current_status_elem == NULL) return false;
    if (graph_elem->appendChild(label_current_status_elem) == NULL) return false;

    if (this->appendToRoot(graph_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_section(
        const char *_section_1_header,
        const char *_section_1_title,
        const char *_section_2_header,
        const char *_section_2_title,
        const char *_section_3_header,
        const char *_section_3_title)
{
    DOMElement *section_elem = this->createElement("Section");
    if (section_elem == NULL) return false;
    
    DOMElement *section1_elem = this->createElement("Section1", "name", _section_1_header, _section_1_title);
    if (section1_elem == NULL) return false;
    if (section_elem->appendChild(section1_elem) == NULL) return false;
    
    DOMElement *section2_elem = this->createElement("Section2", "name", _section_2_header, _section_2_title);
    if (section2_elem == NULL) return false;
    if (section_elem->appendChild(section2_elem) == NULL) return false;
    
    DOMElement *section3_elem = this->createElement("Section3", "name", _section_3_header, _section_3_title);
    if (section3_elem == NULL) return false;
    if (section_elem->appendChild(section3_elem) == NULL) return false;
    
    if (this->appendToRoot(section_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_markers_tbl_titles(
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
        const char* _no_variant)
{
    DOMElement *tbl_elem = this->createElement("MarkersTable");
    if (tbl_elem == NULL) return false;
    if (this->appendToRoot(tbl_elem) == NULL) return false;

    DOMElement *disease_tbl_title_elem = this->createElement("DiseaseTableTitle", _disease_table_title);
    if (disease_tbl_title_elem == NULL) return false;
    if (tbl_elem->appendChild(disease_tbl_title_elem) == NULL) return false;

    DOMElement *drug_tb_title_elem = this->createElement("DrugTableTitle", _drug_table_title);
    if (drug_tb_title_elem == NULL) return false;
    if (tbl_elem->appendChild(drug_tb_title_elem) == NULL) return false;

    DOMElement *header_elem = this->createElement("Header"); 
    if (header_elem == NULL) return false;

    DOMElement *hdr_gene_elem = this->createElement("Gene", _hdr_gene);
    if (hdr_gene_elem == NULL) return false;
    if (header_elem->appendChild(hdr_gene_elem) == NULL) return false;

    DOMElement *hdr_snp_elem = this->createElement("SNP", _hdr_snp);
    if (hdr_snp_elem == NULL) return false;
    if (header_elem->appendChild(hdr_snp_elem) == NULL) return false;

    DOMElement *hdr_alleles_elem = this->createElement("Alleles", _hdr_alleles);
    if (hdr_alleles_elem == NULL) return false;
    if (header_elem->appendChild(hdr_alleles_elem) == NULL) return false;

    DOMElement *hdr_effect_elem = this->createElement("Effect", _hdr_effect);
    if (hdr_effect_elem == NULL) return false;
    if (header_elem->appendChild(hdr_effect_elem) == NULL) return false;

    DOMElement *hdr_risk_allele_elem = this->createElement("RiskAllele", _hdr_risk_allele);
    if (hdr_risk_allele_elem == NULL) return false;
    if (header_elem->appendChild(hdr_risk_allele_elem) == NULL) return false;

    DOMElement *hdr_relative_risk_elem = this->createElement("RelativeRisk", _hdr_relative_risk);
    if (hdr_relative_risk_elem == NULL) return false;
    if (header_elem->appendChild(hdr_relative_risk_elem) == NULL) return false;

    DOMElement *hdr_ref_elem = this->createElement("Reference", _hdr_ref);
    if (hdr_ref_elem == NULL) return false;
    if (header_elem->appendChild(hdr_ref_elem) == NULL) return false;

    if (tbl_elem->appendChild(header_elem) == NULL) return false;

    DOMElement *tbl_caption_elem = this->createElement("TableCaption", _table_caption);
    if (tbl_caption_elem == NULL) return false;
    if (tbl_elem->appendChild(tbl_caption_elem) == NULL) return false;

    DOMElement *variant_elem = this->createElement("NoVariant", _no_variant);
    if (variant_elem == NULL) return false;
    if (tbl_elem->appendChild(variant_elem) == NULL) return false;

    return true;
}

bool GssXmlWriter::insert_titles(
        const char* _count_risk_markers,
        const char* _no_risk_markers,
        const char* _num_of_risk_markers,
        const char* _num_of_pharmacogenetic_markers,
        const char* _references,
        const char* _nocitation,
        const char* _ref_url,
        const char* _doc_sheet,
        const char* _tear_off_instruction,
        const char* _doc_summary)
{
    DOMElement *titles_elem = this->createElement("Titles");
    if (titles_elem == NULL) return false;

    DOMElement *count_risk_markers_elem = this->createElement("CountOfRiskMarkers", _count_risk_markers);
    if (count_risk_markers_elem == NULL) return false;
    if (titles_elem->appendChild(count_risk_markers_elem) == NULL) return false;

    DOMElement *no_risk_markers_elem = this->createElement("NoRiskMarkers", _no_risk_markers);
    if (no_risk_markers_elem == NULL) return false;
    if (titles_elem->appendChild(no_risk_markers_elem) == NULL) return false;

    DOMElement *risk_markers_elem = this->createElement("NumberOfRiskMarkers", _num_of_risk_markers);
    if (risk_markers_elem == NULL) return false;
    if (titles_elem->appendChild(risk_markers_elem) == NULL) return false;

    DOMElement *pharmacogenetic_markers_elem = this->createElement("NumberOfPharmacogeneticMarkers", _num_of_pharmacogenetic_markers);
    if (pharmacogenetic_markers_elem == NULL) return false;
    if (titles_elem->appendChild(pharmacogenetic_markers_elem) == NULL) return false;

    DOMElement *ref_elem = this->createElement("References", _references);
    if (ref_elem == NULL) return false;
    if (titles_elem->appendChild(ref_elem) == NULL) return false;

    DOMElement *citation_elem = this->createElement("NoCitation", _nocitation);
    if (citation_elem == NULL) return false;
    if (titles_elem->appendChild(citation_elem) == NULL) return false;

    DOMElement *ref_url_elem = this->createElement("ReferenceURL", _ref_url);
    if (ref_url_elem == NULL) return false;
    if (titles_elem->appendChild(ref_url_elem) == NULL) return false;

    DOMElement *doc_sheet_elem = this->createElement("DoctorSheet", _doc_sheet);
    if (doc_sheet_elem == NULL) return false;
    if (titles_elem->appendChild(doc_sheet_elem) == NULL) return false;

    DOMElement *tear_off_elem = this->createElement("TearOffInstruction", _tear_off_instruction);
    if (tear_off_elem == NULL) return false;
    if (titles_elem->appendChild(tear_off_elem) == NULL) return false;

    DOMElement *doc_summary_elem = this->createElement("DoctorSummary", _doc_summary);
    if (doc_summary_elem == NULL) return false;
    if (titles_elem->appendChild(doc_summary_elem) == NULL) return false;

    if (this->appendToRoot(titles_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_dis_drug_risk(
        const char* _type,
        const char* _dis_drug,
        const char* _dis_drug_id,
        const char* _linked_disease,
        double _your_risk,
        int _total_marker,
        const char _trait)
{
    if (this->disdrug_elem != NULL) return false;
    this->type = _type;
    
    if (this->type == "Disease")
    {
        char acTrait[2]; acTrait[0]=_trait; acTrait[1]=0;  
        this->disdrug_elem = this->createElement("Disease", "ID", _dis_drug_id, "Trait", acTrait, "Linked", _linked_disease);
    }
    else if (this->type == "Drug")
        this->disdrug_elem = this->createElement("Drug", "ID", _dis_drug_id);
    else if (this->type == "Trait")
        this->disdrug_elem = this->createElement("Trait", "ID", _dis_drug_id);
    else return false;
    
    DOMElement *name_elem = this->createElement("Name", _dis_drug);
    if (name_elem == NULL) return false;
    if (this->disdrug_elem->appendChild(name_elem) == NULL) return false;
    
    char marker[2] = {0};
    sprintf(marker, "%d", _total_marker);
    DOMElement *marker_elem = this->createElement("TotalMarker", marker);
    if (marker_elem == NULL) return false;
    if (this->disdrug_elem->appendChild(marker_elem) == NULL) return false;
    
    DOMElement *risk_elem = this->createElement("Risk");
    if (risk_elem == NULL) return false;
    if (this->disdrug_elem->appendChild(risk_elem) == NULL) return false;
    
    char risk[8] = {0};
    sprintf(risk, "%.2f", _your_risk);
    DOMElement *your_risk_elem = this->createElement("YourRisk", risk);
    if (your_risk_elem == NULL) return false;
    if (risk_elem->appendChild(your_risk_elem) == NULL) return false;
    
    this->segments_elem = this->createElement("Segments");
    if (this->segments_elem == NULL) return false;
    if (risk_elem->appendChild(this->segments_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_profiles(
        int _positive,
        const char* _positive_text,
        int _neutral,
        const char* _neutral_text,
        int _negative,
        const char* _negative_text,
        int _undefined,
        const char* _undefined_text,
        int _total,
        const char* _total_text,
        double _rel_risk)
{
    DOMElement *profiles_elem = this->createElement("Profiles");
    if (profiles_elem == NULL) return false;
    
    char positive_char[16] = {0};
    sprintf(positive_char, "%d", _positive);
    DOMElement *positive_elem = this->createElement("Positive", "count", positive_char, _positive_text);
    if (positive_elem == NULL) return false;
    if (profiles_elem->appendChild(positive_elem) == NULL) return false;
    
    char neutral_char[16] = {0};
    sprintf(neutral_char, "%d", _neutral);
    DOMElement *neutral_elem = this->createElement("Neutral", "count", neutral_char, _neutral_text);
    if (neutral_elem == NULL) return false;
    if (profiles_elem->appendChild(neutral_elem) == NULL) return false;

    char negative_char[16] = {0};
    sprintf(negative_char, "%d", _negative);
    DOMElement *negative_elem = this->createElement("Negative", "count", negative_char, _negative_text);
    if (negative_elem == NULL) return false;
    if (profiles_elem->appendChild(negative_elem) == NULL) return false;
    
    char undefined_char[16] = {0};
    sprintf(undefined_char, "%d", _undefined);
    DOMElement *undefined_elem = this->createElement("Undefined", "count", undefined_char, _undefined_text);
    if (undefined_elem == NULL) return false;
    if (profiles_elem->appendChild(undefined_elem) == NULL) return false;
    
    char total_char[16] = {0};
    sprintf(total_char, "%d", _total);
    DOMElement *total_elem = this->createElement("Total", "count", total_char, _total_text);
    if (total_elem == NULL) return false;
    if (profiles_elem->appendChild(total_elem) == NULL) return false;
    
    char rel_risk[32] = {0};
    sprintf(rel_risk, "%.2f", _rel_risk);
    DOMElement *rel_elem =this->createElement("RelativeRisk", rel_risk);
    if (rel_elem == NULL) return false;
    if (profiles_elem->appendChild(rel_elem) == NULL) return false;
    
    if (this->disdrug_elem->appendChild(profiles_elem) == NULL) return false;
    
    return true;
}

bool GssXmlWriter::insert_segment(
        double _pop_percent,
        unsigned _min_marker,
        unsigned _max_marker,
        const char *_text)
{
    if (_text == NULL) return false;
    DOMElement *seg_elem = this->createElement("Segment", _text);
    if (seg_elem == NULL) return false;
    
    char pop_pct[16] = {0};
    sprintf(pop_pct, "%.2f", _pop_percent);
    char min_marker[16] = {0};
    sprintf(min_marker, "%u", _min_marker);
    char max_marker[16] = {0};
    sprintf(max_marker, "%u", _max_marker);
    
    seg_elem->setAttribute(this->transcode("pop"), this->transcode(pop_pct));
    seg_elem->setAttribute(this->transcode("minMarker"), this->transcode(min_marker));
    seg_elem->setAttribute(this->transcode("maxMarker"), this->transcode(max_marker));
    
    if (this->segments_elem->appendChild(seg_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::append_disease()
{
    if (this->disdrug_elem == NULL) return false;
    if ((this->type != "Disease") && (this->type != "Trait")) return false;    

    if (this->diseases_elem == NULL)
    {
        this->diseases_elem = this->createElement("Diseases");
        if (this->diseases_elem == NULL) return false;
        if (this->appendToFirstChild(this->diseases_elem) == NULL) return false;
    }

    if (this->diseases_elem->appendChild(this->disdrug_elem->cloneNode(true)) == NULL) return false;
    this->disdrug_elem->release();
    this->disdrug_elem = NULL;
    return true;
}

bool GssXmlWriter::append_drug()
{
    if (this->disdrug_elem == NULL) return false;
    if (this->type != "Drug") return false;

    if (this->drugs_elem == NULL)
    {
        this->drugs_elem = this->createElement("Drugs");
        if (this->drugs_elem == NULL) return false;
        if (this->appendToFirstChild(this->drugs_elem) == NULL) return false;
    }

    if (this->drugs_elem->appendChild(this->disdrug_elem->cloneNode(true)) == NULL) return false;
    this->disdrug_elem->release();
    this->disdrug_elem = NULL;
    return true;
}

bool GssXmlWriter::insert_genoeffect()
{
    this->geno_effect_elem = this->createElement("GenoEffect", "risk", "UND");
    if (this->geno_effect_elem == NULL) return false;
    this->genotypes_elem = this->createElement("Genotypes");
    if (this->genotypes_elem == NULL) return false;
    if (this->geno_effect_elem->appendChild(this->genotypes_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_genoeffect(
        double _risk)
{
    char risk[32] = {0};
    sprintf(risk, "%.0f", _risk);
    this->geno_effect_elem = this->createElement("GenoEffect", "risk", risk);
    if (this->geno_effect_elem == NULL) return false;
    
    this->genotypes_elem = this->createElement("Genotypes");
    if (this->geno_effect_elem->appendChild(this->genotypes_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_geno_effects_tag()
{
    this->geno_effects_elem = this->createElement("GenoEffects");
    if (this->geno_effects_elem == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_genotype(
        const char* _gene,
        unsigned _rsid,
        const char* _alleles,
        const char* _risk_allele,
        double _rel_risk)
{
    DOMElement *genotype_elem = this->createElement("Genotype");
    if (genotype_elem == NULL) return false;
    
    DOMElement *gene_elem = this->createElement("Gene", _gene);
    if (gene_elem == NULL) return false;
    if (genotype_elem->appendChild(gene_elem) == NULL) return false;
    
    char rsid[32] = {0};
    sprintf(rsid, "rs%u", _rsid);
    DOMElement *snp_elem = this->createElement("SNP", rsid);
    if (snp_elem == NULL) return false;
    if (genotype_elem->appendChild(snp_elem) == NULL) return false;
    
    DOMElement *alleles_elem = this->createElement("Alleles", _alleles);
    if (alleles_elem == NULL) return false;
    if (genotype_elem->appendChild(alleles_elem) == NULL) return false;
    
    DOMElement *risk_allele_elem = this->createElement("RiskAllele", _risk_allele);
    if (risk_allele_elem == NULL) return false;
    if (genotype_elem->appendChild(risk_allele_elem) == NULL) return false;
    
    char rel_risk[32] = {0};
    sprintf(rel_risk, "%.2f", _rel_risk);
    DOMElement *relative_elem = this->createElement("RelativeRisk", rel_risk);
    if (relative_elem == NULL) return false;
    if (genotype_elem->appendChild(relative_elem) == NULL) return false;
    
    if (this->genotypes_elem == NULL) return false;
    if (this->genotypes_elem->appendChild(genotype_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_effect_ref(
        const char* _effect,
        const char* _reference)
{
    if (this->geno_effect_elem->appendChild(this->genotypes_elem) == NULL) return false;
    
    DOMElement *effect_elem = this->createElement("Effect", _effect);
    if (effect_elem == NULL) return false;
    if (this->geno_effect_elem->appendChild(effect_elem) == NULL) return false;
    
    DOMElement *ref_elem = this->createElement("Reference", _reference);
    if (ref_elem == NULL) return false;
    if (this->geno_effect_elem->appendChild(ref_elem) == NULL) return false;
    
    if (this->geno_effects_elem->appendChild(this->geno_effect_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::append_geno_effects()
{
    if (this->disdrug_elem->appendChild(this->geno_effects_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_description(
        const char* _title,
        const char* _text)
{
    assert(_title != NULL);
    assert(_text != NULL);
    DOMElement *desc_elem = this->createElement("Description");
    if (desc_elem == NULL) return false;
    
    DOMElement *title_elem = this->createElement("Title", _title);
    if (title_elem == NULL) return false;
    if (desc_elem->appendChild(title_elem) == NULL) return false;
    
    string text = _text;
    size_t found = text.find('%');
    while(found != string::npos)
    {
        text.insert(found, "\\");
        found = text.find('%', found + 2);
    }
    DOMElement *text_elem = this->createElement("Text", text.c_str());
    if (text_elem == NULL) return false;
    if (desc_elem->appendChild(text_elem) == NULL) return false;
    
    if (this->disdrug_elem->appendChild(desc_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_references_tag()
{
    this->references_elem = this->createElement("References");
    if (this->references_elem == NULL) return false;
    
    return true;
}

bool GssXmlWriter::insert_reference(
        size_t _pmid,
        const char* _author,
        const char* _title,
        const char* _journal,
        const char* _pubdate)
{
    char index[12] = {0};
    sprintf(index, "%lu", _pmid);
    DOMElement *ref_elem = this->createElement("Reference", "id", index);
    if (ref_elem == NULL) return false;
    
    char pmid[12] = {0};
    sprintf(pmid, "PMID:%lu", _pmid);
    DOMElement *pmid_elem = this->createElement("PmId", pmid);
    if (pmid_elem == NULL) return false;
    if (ref_elem->appendChild(pmid_elem) == NULL) return false;
    
    DOMElement *author_elem = this->createElement("Author", _author);
    if (author_elem == NULL) return false;
    if (ref_elem->appendChild(author_elem) == NULL) return false;
    
    DOMElement *title_elem = this->createElement("Title", _title);
    if (title_elem == NULL) return false;
    if (ref_elem->appendChild(title_elem) == NULL) return false;
    
    DOMElement *journal_elem = this->createElement("Journal", _journal);
    if (journal_elem == NULL) return false;
    if (ref_elem->appendChild(journal_elem) == NULL) return false;
    
    DOMElement *pubdate_elem = this->createElement("Pubdate", _pubdate);
    if (pubdate_elem == NULL) return false;
    if (ref_elem->appendChild(pubdate_elem) == NULL) return false;
    
    if (this->references_elem->appendChild(ref_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::append_references()
{
    if (this->disdrug_elem->appendChild(this->references_elem) == NULL) return false;
    return true;
}

bool GssXmlWriter::insert_G3_content(
        const char* _tag,
        const char* _title,
        const char* _content)
{
    if (this->G3Contents_elem == NULL){ this->G3Contents_elem = this->createElement("G3Contents"); }

    DOMElement *G3_elem = this->createElement(_tag);
    if (G3_elem == NULL) return false;

    if (this->G3Contents_elem->appendChild(G3_elem) == NULL) return false;

    DOMElement *title_elem = this->createElement("Title", _title);
    if (title_elem == NULL) return false;
    if (G3_elem->appendChild(title_elem) == NULL) return false;

    DOMElement *text_elem = this->createElement("Text", _content);
    if (text_elem == NULL) return false;
    if (G3_elem->appendChild(text_elem) == NULL) return false;

    return true;
}

bool GssXmlWriter::append_G3_content()
{
    if (this->disdrug_elem == NULL) return false;
    if (this->G3Contents_elem == NULL) return true;
    if (this->disdrug_elem->appendChild(this->G3Contents_elem) == NULL) return false;
    this->G3Contents_elem = NULL;
    return true;
}


bool GssXmlWriter::insert_notes(const char* _notes)
{
    DOMElement *note_elem = this->createElement("Notes", _notes);
    if (note_elem == NULL) return false;
    if (this->appendToRoot(note_elem) == NULL) return false;
    return true;
}
