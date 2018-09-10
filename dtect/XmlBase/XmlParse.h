/* 
 * File:   XmlParse.h
 * Author: jameswong
 *
 * Created on February 21, 2013, 10:39 PM
 */

#ifndef XMLPARSE_H
#define	XMLPARSE_H

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include "XmlBase.h"

enum XML_ELEMENT_TYPE { XML_FIRST, XML_CHILD, XML_SIBLING };

class XmlParse : public XmlBase
{
private:
    xercesc::XercesDOMParser* xerces_parser;
    int xml_level;
    
    bool parse_dom(
        XML_ELEMENT_TYPE _type,
        xercesc::DOMElement* _pa_elem,
        xercesc::DOMElement* _ch_elem);
    
protected:
    virtual bool parse(
        const xercesc::DOMElement* _pa_elem,
        const xercesc::DOMElement* _ch_elem) = 0;
    
    bool parse_file(const char *_filename);
    
public:
    XmlParse();
    virtual ~XmlParse();
};

#endif	/* XMLPARSE_H */

