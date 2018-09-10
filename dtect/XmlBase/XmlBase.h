/* 
 * File:   XmlBase.h
 * Author: jameswong
 *
 * Created on February 21, 2013, 10:29 PM
 */

#ifndef XMLBASE_H
#define	XMLBASE_H

#include <string>
#include <queue>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

using namespace std;
using namespace XERCES_CPP_NAMESPACE;

class XmlBase
{
private:
    queue<char*> ch_str_q;
    queue<XMLCh*> xmlch_str_q;
    
protected:
    DOMDocument* dom_doc;
    char* transcode(const XMLCh* _str);
    XMLCh* transcode(const char* _str);
    
    void reset();
    
public:
    XmlBase();
    virtual ~XmlBase();
};

#endif	/* XMLBASE_H */

