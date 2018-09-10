/* 
 * File:   XmlWrite.h
 * Author: Manish
 *
 * Created on October 23, 2012, 3:27 PM
 */

#ifndef XMLWRITE_H
#define	XMLWRITE_H

#include "XmlBase.h"

class XmlWrite : public XmlBase
{
private:
    DOMImplementation *pdom_implementation;
//    DOMDocument *dom_doc;
    DOMNode *root_node;
    
    bool assert_dom_doc() const;
    bool write(DOMDocument *_dom_doc, const char *_filename);
    void cleanup();
    
protected:
    XmlWrite();
    ~XmlWrite();
    
    bool init();
    
    bool output(const char *_filename);
    
    DOMElement *createElement(
            const char *_tag_name);
    
    DOMElement *createElement(
            const char *_tag_name,
            const char *_tag_text);
    
    DOMElement *createElement(
            const char *_tag_name,
            const char *_attr_id,
            const char *_attr_val);
    
    DOMElement *createElement(
            const char *_tag_name,
            const char *_attr_id,
            const char *_attr_val,
            const char *_tag_text);
    
    DOMElement *createElement(
            const char *_tag_name,
            const char *_attr_id_one,
            const char *_attr_val_one,
            const char *_attr_id_two,
            const char *_attr_val_two);
    
    DOMElement *createElement(
            const char *_tag_name,
            const char *_attr_id_one,
            const char *_attr_val_one,
            const char *_attr_id_two,
            const char *_attr_val_two,
            const char *_attr_id_three,
            const char *_attr_val_three);
    
    DOMNode *appendToRoot(
            DOMElement *_elem);
    
    DOMNode *appendToFirstChild(
            DOMElement *_elem);
};

#endif	/* XMLWRITE_H */

