/* 
 * File:   XmlParse.cpp
 * Author: Manish
 * 
 * Created on February 21, 2013, 10:39 PM
 */

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <xercesc/dom/DOMElement.hpp>
#include "XmlParse.h"

XmlParse::XmlParse()
{
    this->xerces_parser = new XercesDOMParser;
    if (this->xerces_parser == NULL)
    {
        fprintf(stderr, "\nERROR: XercesDOMParser initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    
    this->xml_level = 0;
}

XmlParse::~XmlParse()
{
    if (this->xerces_parser != NULL)
    {
        delete this->xerces_parser;
        this->xerces_parser = NULL;
    }
}

bool XmlParse::parse_file(const char* _filename)
{
    assert(_filename != NULL);
    
    struct stat st;
    if (stat(_filename, &st) != 0)
    {
        fprintf(stderr, "\nERROR: %s\n", strerror(errno));
        return false;
    }
    
    try
    {
        this->xerces_parser->parse(_filename);
        this->dom_doc = this->xerces_parser->getDocument();
        if (this->dom_doc == NULL)
        {
            fprintf(stderr, "\nERROR: getDocument()\n");
            return false;
        }
        DOMElement* root = this->dom_doc->getDocumentElement();
        if (root == NULL)
        {
            fprintf(stderr, "\nERROR: getDocumentElement()\n");
            return false;
        }
        
        bool ret = this->parse_dom(XML_FIRST, root, root);
        assert(ret == true);
        return ret;
    }
    catch(DOMException& _e)
    {
        char* message = XMLString::transcode(_e.getMessage());
        fprintf(stderr, "\nERROR: Parsing file %s error: %s.\n", _filename, message);
        XMLString::release(&message);
        exit(EXIT_FAILURE);
    }
    catch(XMLException& _e)
    {
        char* message = XMLString::transcode(_e.getMessage());
        fprintf(stderr, "\nERROR: Parsing file %s error: %s.\n", _filename, message);
        XMLString::release(&message);
        exit(EXIT_FAILURE);
    }
}

bool XmlParse::parse_dom(
        XML_ELEMENT_TYPE _type,
        xercesc::DOMElement* _pa_elem,
        xercesc::DOMElement* _ch_elem)
{
    assert(_pa_elem != NULL);
    assert(_ch_elem != NULL);
    
    if ((_type == XML_FIRST) && (this->xml_level != 0))
        return false;
    if ((_type == XML_FIRST) && (_pa_elem != _ch_elem))
        return false;
    
    if (_type == XML_CHILD)
        this->xml_level++;
    
    bool ret = this->parse(_pa_elem, _ch_elem);
    assert(ret == true);
    
    /*
     * If has child, go into the child recursively
     */
    DOMElement *child = _ch_elem->getFirstElementChild();
    if (child != NULL)
    {
        ret = this->parse_dom(XML_CHILD, _ch_elem, child);
        assert(ret == true);
    }
    
    /*
     *  If has sibling, go into sibling recursively
     */
    DOMElement *sibling = _ch_elem->getNextElementSibling();
    if (sibling != NULL)
    {
        ret = this->parse_dom(XML_SIBLING, _pa_elem, sibling);
        assert(ret == true);
    }
    
    /*
     * If no child nor sibling, exit (unroll recursive function)
     */
    if (_type == XML_CHILD)
        this->xml_level--;
    return ret;
}