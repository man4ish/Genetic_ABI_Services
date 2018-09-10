/* 
 * File:   XmlWrite.cpp
 * Author: Manish
 * 
 * Created on October 23, 2012, 3:27 PM
 */

#include <iostream>
#include "XmlWrite.h"
#include <xercesc/util/TransService.hpp>

using namespace std;

bool XmlWrite::assert_dom_doc() const
{
    if (this->pdom_implementation == NULL) return false;
    if (this->dom_doc == NULL) return false;
    return true;
}

XmlWrite::XmlWrite()
{
    XMLCh tempStr[100];
    bool ret = XMLString::transcode("LS", tempStr, 99);
    assert(ret == true);
    this->pdom_implementation = DOMImplementationRegistry::getDOMImplementation(tempStr);
    assert(this->pdom_implementation != NULL);
    this->dom_doc = NULL;
    this->root_node = NULL;
}

XmlWrite::~XmlWrite()
{
    this->cleanup();
}

bool XmlWrite::write(DOMDocument *_dom_doc, const char *_filename)
{
    if (_dom_doc == NULL) return false;
    try
    {
        DOMLSSerializer *the_serializer;
        XMLFormatTarget *stdout_target;
        DOMLSOutput *the_output;
    
        the_serializer = ((DOMImplementationLS*)this->pdom_implementation)->createLSSerializer();
        if (the_serializer == NULL) return false;
        
        if (_filename == NULL)
        {
            stdout_target = new StdOutFormatTarget();
            if (stdout_target == NULL) return false;
        }
        else
        {
            stdout_target = new LocalFileFormatTarget(this->transcode(_filename));
            if (stdout_target == NULL) return false;
        }
        
        the_output = ((DOMImplementationLS*)this->pdom_implementation)->createLSOutput();
        if (the_output == NULL) return false;
        the_output->setByteStream(stdout_target);
        the_serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
        the_serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTSplitCdataSections, true);
        the_serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTEntities, false);
        
        the_serializer->write(_dom_doc, the_output);
        
        if (the_output != NULL)
            the_output->release();
        if (the_serializer != NULL)
            the_serializer->release();
        if (stdout_target != NULL)
            delete stdout_target;
    }
    catch (const XMLException &e)
    {
        cerr << "Exception: " << this->transcode(e.getMessage()) << endl;
        exit(EXIT_FAILURE);
    }
    catch (const DOMException &e)
    {
        cerr << "Exception: " << this->transcode(e.msg) << endl;
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        cerr << "Unexpected exception" << endl;
        exit(EXIT_FAILURE);
    }
    return true;
}

bool XmlWrite::init()
{
    if (this->dom_doc != NULL) return true;
    this->dom_doc = this->pdom_implementation->createDocument(0, this->transcode("Root"), 0);
    if (this->dom_doc == NULL) return false;
    this->root_node = NULL;
    
    DOMElement *elem = this->dom_doc->getDocumentElement();
    if (elem == NULL) return false;
    if (strcmp(this->transcode(elem->getNodeName()), "Root") == 0)
    {
        this->root_node = elem;
        return true;
    }
    
    if (this->root_node == NULL) return false;
    return true;
}

void XmlWrite::cleanup()
{
    if (this->dom_doc != NULL)
    {
        this->dom_doc->release();
        this->dom_doc = NULL;
    }
    this->root_node = NULL;
    this->reset();
}

bool XmlWrite::output(const char *_filename)
{
    bool ret = this->write(this->dom_doc, _filename);
    this->cleanup();
    return ret;
}

DOMElement *XmlWrite::createElement(
        const char* _tag_name)
{
    assert(_tag_name != NULL);
    assert(this->assert_dom_doc());
    return this->dom_doc->createElement(this->transcode(_tag_name));
}

DOMElement *XmlWrite::createElement(
        const char* _tag_name,
        const char* _tag_text)
{
    assert(_tag_name != NULL);
    assert(_tag_text != NULL);
    assert(this->assert_dom_doc());
    DOMElement *elem = this->dom_doc->createElement(this->transcode(_tag_name));
    if (elem == NULL) return NULL;
    DOMText *text = this->dom_doc->createTextNode(this->transcode(_tag_text));
    if (text == NULL) return NULL;
    if (elem->appendChild(text) == NULL) return NULL;
    return elem;
}

DOMElement *XmlWrite::createElement(
        const char* _tag_name,
        const char* _attr_id,
        const char* _attr_val)
{
    assert(_tag_name != NULL);
    assert(_attr_id != NULL);
    assert(_attr_val != NULL);
    assert(this->assert_dom_doc());
    DOMElement *elem = this->dom_doc->createElement(this->transcode(_tag_name));
    if (elem == NULL) return NULL;
    elem->setAttribute(this->transcode(_attr_id), this->transcode(_attr_val));
    return elem;
}

DOMElement *XmlWrite::createElement(
        const char* _tag_name,
        const char* _attr_id,
        const char* _attr_val,
        const char* _tag_text)
{
    assert(_tag_name != NULL);
    assert(_attr_id != NULL);
    assert(_attr_val != NULL);
    assert(_tag_text != NULL);
    assert(this->assert_dom_doc());
    DOMElement *elem = this->dom_doc->createElement(this->transcode(_tag_name));
    if (elem == NULL) return NULL;
    elem->setAttribute(this->transcode(_attr_id), this->transcode(_attr_val));
    DOMText *text = this->dom_doc->createTextNode(this->transcode(_tag_text));
    if (text == NULL) return false;
    if (elem->appendChild(text) == NULL) return NULL;
    return elem;
}

DOMElement *XmlWrite::createElement(
        const char* _tag_name,
        const char* _attr_id_one,
        const char* _attr_val_one,
        const char* _attr_id_two,
        const char* _attr_val_two)
{
    assert(_tag_name != NULL);
    assert(_attr_id_one != NULL);
    assert(_attr_val_one != NULL);
    assert(_attr_id_two != NULL);
    assert(_attr_val_two != NULL);
    assert(this->assert_dom_doc());
    DOMElement *elem = this->dom_doc->createElement(this->transcode(_tag_name));
    if (elem == NULL) return NULL;
    elem->setAttribute(this->transcode(_attr_id_one), this->transcode(_attr_val_one));
    elem->setAttribute(this->transcode(_attr_id_two), this->transcode(_attr_val_two));
    return elem;
}

DOMElement *XmlWrite::createElement(
        const char* _tag_name,
        const char* _attr_id_one,
        const char* _attr_val_one,
        const char* _attr_id_two,
        const char* _attr_val_two,
        const char* _attr_id_three,
        const char* _attr_val_three)
{
    assert(_tag_name != NULL);
    assert(_attr_id_one != NULL);
    assert(_attr_val_one != NULL);
    assert(_attr_id_two != NULL);
    assert(_attr_val_two != NULL);
    assert(_attr_id_three != NULL);
    assert(_attr_val_three != NULL);
    assert(this->assert_dom_doc());
    DOMElement *elem = this->dom_doc->createElement(this->transcode(_tag_name));
    if (elem == NULL) return NULL;
    elem->setAttribute(this->transcode(_attr_id_one), this->transcode(_attr_val_one));
    elem->setAttribute(this->transcode(_attr_id_two), this->transcode(_attr_val_two));
    elem->setAttribute(this->transcode(_attr_id_three), this->transcode(_attr_val_three));
    return elem;
}

DOMNode *XmlWrite::appendToRoot(
        DOMElement* _elem)
{
    assert(_elem != NULL);
    assert(this->assert_dom_doc());
    assert(this->root_node != NULL);
    return this->root_node->appendChild(_elem);
}

DOMNode *XmlWrite::appendToFirstChild(
        DOMElement* _elem)
{
    assert(_elem != NULL);
    assert(this->assert_dom_doc());
    return this->dom_doc->getFirstChild()->appendChild(_elem);
}