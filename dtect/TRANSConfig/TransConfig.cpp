/* 
 * File:   TransConfig.cpp
 * Author: jameswong
 * 
 * Created on January 9, 2013, 12:01 PM
 */

#include "TransConfig.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>

TransConfig::TransConfig()
{
    this->init();
}

TransConfig::~TransConfig()
{
    this->cleanup();
}

bool TransConfig::init()
{
    this->env_var = getenv("TRANSConfig");
    if (this->env_var == NULL)
    {
        fprintf(stderr, "\nERROR: TRANSConfig environment variable not set.\n");
        exit(EXIT_FAILURE);
    }
    
    bool ret = this->parse_file(this->env_var);
    assert(ret == true);
    
    return true;
}

void TransConfig::cleanup()
{
    for (this->docinfo_it = this->docinfo_map.begin(); this->docinfo_it != this->docinfo_map.end(); ++this->docinfo_it)    
        delete this->docinfo_it->second;
    this->docinfo_map.clear();
}

bool TransConfig::parse(
        const xercesc::DOMElement* _pa_elem,
        const xercesc::DOMElement* _ch_elem)
{
    if ((_ch_elem == NULL) || (_pa_elem == NULL))
        throw(runtime_error("Invalid DOM element"));
    
    bool ret;
    try
    {
        char* pa_elem_tagname = this->transcode(_pa_elem->getTagName());
        assert(pa_elem_tagname != NULL);
        
        char* ch_elem_tagname = this->transcode(_ch_elem->getTagName());
        assert(ch_elem_tagname != NULL);
        
        if (strcmp(ch_elem_tagname, "root") == 0)
        {
        }
        else if (strcmp(ch_elem_tagname,"latex") == 0)
        {
            this->latex = this->transcode(_ch_elem->getTextContent());
        }   
        else if ((strcmp(pa_elem_tagname, "label") == 0) && (strcmp(ch_elem_tagname, "doc_id") == 0))
        {
            ret = this->insert_doc(pa_elem_tagname);
            assert(ret == true);
            this->docinfo_it->second->insert_doc_id(pa_elem_tagname, this->transcode(_ch_elem->getTextContent()));
        }
        else if ((strcmp(pa_elem_tagname, "label") == 0) && (strcmp(ch_elem_tagname, "version") == 0))
        {
            ret = this->insert_doc(pa_elem_tagname);
            assert(ret == true);
            this->docinfo_it->second->version = atoi(this->transcode(_ch_elem->getTextContent()));
        }
        else if ((strcmp(pa_elem_tagname, "chip") == 0) && (strcmp(ch_elem_tagname, "format") == 0))
        {
            ret = this->insert_doc(this->transcode(_pa_elem->getAttribute(this->transcode("testcode"))));
            assert(ret == true);
            this->docinfo_it->second->doc_format = atoi(this->transcode(_ch_elem->getTextContent()));
        }
        else if ((strcmp(pa_elem_tagname, "chip") == 0) && (strcmp(ch_elem_tagname, "version") == 0))
        {
            ret = this->insert_doc(this->transcode(_pa_elem->getAttribute(this->transcode("testcode"))));
            assert(ret == true);
            this->docinfo_it->second->version = atoi(this->transcode(_ch_elem->getTextContent()));
        }
        else if ((strcmp(pa_elem_tagname, "chip") == 0) && (strcmp(ch_elem_tagname, "doc_id") == 0))
        {
            ret = this->insert_doc(this->transcode(_pa_elem->getAttribute(this->transcode("testcode"))));
            assert(ret == true);
            this->docinfo_it->second->insert_doc_id(
                    this->transcode(_ch_elem->getAttribute(this->transcode("type"))),
                    this->transcode(_ch_elem->getTextContent()));
        }
//        else
//        {
//            char error_message[1024] = {0};
//            sprintf(error_message, "invalid tag <%s>", ch_elem_tagname);
//            throw(runtime_error(error_message));
//            return false;
//        }
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

bool TransConfig::insert_doc(
        const char* _key)
{
    assert(_key != NULL);
    this->docinfo_it = this->docinfo_map.find(_key);
    if (this->docinfo_it == this->docinfo_map.end())
        this->docinfo_it = this->docinfo_map.insert(STR_DOCINFO_PAIR(_key, new DocInfo())).first;
    return (this->docinfo_it != this->docinfo_map.end());
}

const char* TransConfig::get_doc_id(
        const string& _type) const
{
    assert(_type.empty() == false);
    STR_DOCINFO_MAP::const_iterator cit;
    cit = this->docinfo_map.find(_type);
    assert(cit != this->docinfo_map.end());
    return cit->second->get_doc_id(_type);
}

const char* TransConfig::get_doc_id(
        const string& _testcode,
        const string& _type) const
{
    assert(_testcode.empty() == false);
    assert(_type.empty() == false);
    STR_DOCINFO_MAP::const_iterator cit;
    cit = this->docinfo_map.find(_testcode);
    assert(cit != this->docinfo_map.end());
    return cit->second->get_doc_id(_type);
}

int TransConfig::get_version(
        const string& _testcode) const
{
    assert(_testcode.empty() == false);
    STR_DOCINFO_MAP::const_iterator cit;
    cit = this->docinfo_map.find(_testcode);
    assert(cit != this->docinfo_map.end());
    return cit->second->version;
}

const char* TransConfig::get_latex() const
{
    return this->latex.c_str();
}
