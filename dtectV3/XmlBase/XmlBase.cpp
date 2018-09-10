/* 
 * File:   XmlBase.cpp
 * Author: Manish
 * 
 * Created on February 21, 2013, 10:29 PM
 */

#include <stdio.h>
#include "XmlBase.h"

XmlBase::XmlBase()
{
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch (XMLException& _e)
    {
        char* message = XMLString::transcode(_e.getMessage());
        fprintf(stderr, "\nERROR: XML toolkit initialization error: %s.\n", message);
        XMLString::release(&message);
        exit(EXIT_FAILURE);
    }
    this->dom_doc = NULL;
    this->reset();
}

XmlBase::~XmlBase()
{
    this->reset();
    try
    {
        XMLPlatformUtils::Terminate();
    }
    catch(XMLException &e)
    {
        char *message = XMLString::transcode(e.getMessage());
        fprintf(stderr, "\nERROR: XML toolkit teardown error: %s.\n", message);
        XMLString::release(&message);
        exit(EXIT_FAILURE);
    }
}

char* XmlBase::transcode(const XMLCh* _str)
{
    assert(_str != NULL);
    try
    {
        while (this->ch_str_q.size() > 20)
        {
            XMLString::release(&(this->ch_str_q.front()));
            this->ch_str_q.pop();
        }
        char* ch_str = XMLString::transcode(_str);
        assert(ch_str != NULL);
        this->ch_str_q.push(ch_str);
    }
    catch (...)
    {
        throw("xml_transcode(const XMLCh* _str) failed.\n");
    }
    return this->ch_str_q.back();
}

XMLCh* XmlBase::transcode(const char* _str)
{
    assert(_str != NULL);
    try
    {
        while (this->xmlch_str_q.size() > 20)
        {
            XMLString::release(&(this->xmlch_str_q.front()));
            this->xmlch_str_q.pop();
        }
        XMLCh *xml_str = XMLString::transcode(_str);
        assert(xml_str != NULL);
        this->xmlch_str_q.push(xml_str);
    }
    catch (...)
    {
        throw("xml_transcode(const char* _str) failed\n");
    }
    return this->xmlch_str_q.back();
}

void XmlBase::reset()
{
    while (this->ch_str_q.empty() == false)
    {
        XMLString::release(&(this->ch_str_q.front()));
        this->ch_str_q.pop();
    }
    while (this->xmlch_str_q.empty() == false)
    {
        XMLString::release(&(this->xmlch_str_q.front()));
        this->xmlch_str_q.pop();
    }
}