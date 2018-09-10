#include "atom_helper.h"
#include <stdio.h>

namespace gdata
{
    using namespace std;
    using xmlpp::TextNode;
    using xmlpp::Attribute;
    
    AtomHelper::AtomHelper()
    {
//        namespaces_["atom"] = "http://www.w3.org/2007/app";
        namespaces_["atom"] = "http://www.w3.org/2005/Atom";
        
//        namespaces_["openSearch"] = "http://a9.com/-/spec/opensearchrss/1.0/";
        namespaces_["openSearch"] = "http://a9.com/-/spec/opensearch/1.1/";
        
        namespaces_["gs"] = "http://schemas.google.com/spreadsheets/2006";
        
        namespaces_["gd"] = "http://schemas.google.com/g/2005";
        
        namespaces_["gsx"] = "http://schemas.google.com/spreadsheets/2006/extended";
    }
    
    void AtomHelper::RegisterNamespaces(const map<string, string>& namespaces)
    {
        map<string, string>::const_iterator iter;
        for (iter = namespaces.begin(); iter != namespaces.end(); ++iter)
            namespaces_[iter->first] = iter->second;
    }
    
    void AtomHelper::Parse(const string& xml_str)
    {
        parser_.parse_memory(xml_str);
    }
    
    NodeSet AtomHelper::Find(const Node* node, string xpath) const
    {
        return node->find(xpath, namespaces_);
    }
    
    const Node* AtomHelper::Entry() const
    {
        const Node *node = NULL;
        if (parser_)
            node = parser_.get_document()->get_root_node();
        return node;
    }
    
    NodeSet AtomHelper::Entries() const
    {
        NodeSet entries;
        if (parser_)
            entries = Find(parser_.get_document()->get_root_node(), "./atom:entry");
        return entries;
    }
    
    string AtomHelper::AlternateLinkHref(const Node *entry) const
    {
        if (!parser_) return "";
        return Attribute(Link(entry, "alternate"), "href");
    }
    
    string AtomHelper::CategoryLabel(const Node *entry) const
    {
        if (!parser_) return "";
        string label;
        NodeSet categories = Find(entry, "./atom:category");
        for (unsigned int i = 0; i < categories.size(); ++i)
        {
            const Element *nodeElement = dynamic_cast<const Element*>(categories[i]);
            const Element::AttributeList& attributes = nodeElement->get_attributes();

            Element::AttributeList::const_iterator iter;
            for (iter = attributes.begin(); iter != attributes.end(); ++iter)
            {
                const xmlpp::Attribute *attribute = *iter;
                if (attribute->get_name() == "scheme" && attribute->get_value() == "http://schemas.google.com/g/2005#kind")
                {
                    label = Attribute(nodeElement, "label");
                    break;
                }
            }
        }
        return label;
    }

    string AtomHelper::AclRole(const Node *entry) const
    {
        if (!parser_) return "";
        NodeSet roles = Find(entry, "./gAcl:role");
        if (roles.size())
            return Attribute(dynamic_cast<const Element*>(roles[0]), "value");
        return "";
    }

    string AtomHelper::AclScope(const Node *entry) const
    {
        if (!parser_) return "";
        NodeSet scopes = Find(entry, "./gAcl:scope");
        if (scopes.size())
            return Attribute(dynamic_cast<const Element*>(scopes[0]), "value");
        return "";
    }

    string AtomHelper::ContentSrc(const Node *entry) const
    {
        if (!parser_) return "";
        NodeSet contents = Find(entry, "./atom:content");
        if (contents.size())
            return Attribute(dynamic_cast<const Element*>(contents[0]), "src");
        return "";
    }
    
    string AtomHelper::CellFeed(const Node* entry) const
    {
        if (!parser_) return "";
        NodeSet contents = Find(entry, "./atom:link");
        for (size_t i = 0; i < contents.size(); i++)
        {
            string attr_val = this->Attribute(dynamic_cast<const Element*>(contents[i]), "rel");
            if (attr_val.find("cellsfeed") != attr_val.npos)
                return this->Attribute(dynamic_cast<const Element*>(contents[i]), "href");
        }
        return "";
    }
    
    string AtomHelper::Content(const Node* entry) const
    {
        if (!parser_) return "";
        NodeSet contents = Find(entry, "./atom:content");
        if (contents.size())
        {
            const TextNode *content = dynamic_cast<const TextNode*>(*contents[0]->get_children().begin());
            return content->get_content();
        }
        return "";
    }

    string AtomHelper::EditLinkHref(const Node *entry) const
    {
        if (!parser_) return "";
        return Attribute(Link(entry, "edit"), "href");
    }

    string AtomHelper::EditMediaLinkHref(const Node *entry) const
    {
        if (!parser_) return "";
        return Attribute(Link(entry, "edit-media"), "href");
    }

    string AtomHelper::ETag(const Node *entry) const
    {
        if (!parser_) return "";
        return Attribute(dynamic_cast<const Element*>(entry), "etag");
    }

    string AtomHelper::FeedLinkHref(const Node *entry) const
    {
        if (!parser_) return "";
        NodeSet feedLinks = Find(entry, "./gd:feedLink");
        if (feedLinks.size())
            return Attribute(dynamic_cast<const Element*>(feedLinks[0]), "href");
        return "";
    }

    string AtomHelper::Id(const Node *entry) const
    {
        if (!parser_) return "";
        NodeSet ids = Find(entry, "./atom:id");
        if (ids.size())
        {
            const TextNode *id = dynamic_cast<const TextNode*>(*ids[0]->get_children().begin());
            return id->get_content();
        }
        return "";
    }

    string AtomHelper::Title(const Node *entry) const
    {
        if (!parser_) return "";
        NodeSet titles = Find(entry, "./atom:title");
        if (titles.size())
        {
            const TextNode *title = dynamic_cast<const TextNode*>(*titles[0]->get_children().begin());
            return title->get_content();
        }
        return "";
    }
    
    string AtomHelper::Updated(const Node* entry) const
    {
        if (!this->parser_) return "";
        NodeSet updated = Find(entry, "./atom:updated");
        if (updated.size())
        {
            const TextNode *update = dynamic_cast<const TextNode*>(*updated[0]->get_children().begin());
            return update->get_content();
        }
        return "";
    }
    
    string AtomHelper::RowCount(const Node* entry) const
    {
        if (!this->parser_) return "";
        NodeSet rows = Find(entry, "./gs:rowCount");
        if (rows.size())
        {
            const TextNode *row = dynamic_cast<const TextNode*>(*rows[0]->get_children().begin());
            return row->get_content();
        }
        return "";
    }
    
    string AtomHelper::ColCount(const Node* entry) const
    {
        if (!this->parser_) return "";
        NodeSet cols = Find(entry, "./gs:colCount");
        if (cols.size())
        {
            const TextNode *col = dynamic_cast<const TextNode*>(*cols[0]->get_children().begin());
            return col->get_content();
        }
        return "";
    }
    
    string AtomHelper::Lang(const Node* entry, string lang) const
    {
        if (!this->parser_) return "";
        NodeSet langs = Find(entry, "./gsx:" + lang);
        if (langs.size())
        {
            if (langs[0]->get_children().size())
            {
                const TextNode *lang_node = dynamic_cast<const TextNode*>(*langs[0]->get_children().begin());
                return lang_node->get_content();
            }
        }
        return "";
    }
    
    string AtomHelper::Author(const Node* entry) const
    {
        if (!this->parser_) return "";
        NodeSet authors = Find(entry, "preceding-sibling::atom:author/atom:name");
        if (authors.size())
        {
            const TextNode *author = dynamic_cast<const TextNode*>(*authors[0]->get_children().begin());
            return author->get_content();
        }
        return "";
    }
    
    string AtomHelper::AuthorEmail(const Node* entry) const
    {
        if (!this->parser_) return "";
        NodeSet authors = Find(entry, "preceding-sibling::atom:author/atom:email");
        if (authors.size())
        {
            const TextNode *author = dynamic_cast<const TextNode*>(*authors[0]->get_children().begin());
            return author->get_content();
        }
        return "";
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Private members
    ////////////////////////////////////////////////////////////////////////////////
    string AtomHelper::Attribute(const Element *from_element,
                                 string attr_name) const
    {
        if (!parser_) return "";
        const Element::AttributeList& attributes = from_element->get_attributes();
        Element::AttributeList::const_iterator iter;
        for (iter = attributes.begin(); iter != attributes.end(); ++iter)
        {
            const xmlpp::Attribute *attribute = *iter;
            if (attribute->get_name() == attr_name)
                return attribute->get_value();
        }
        return "";
    }

    const Element* AtomHelper::Link(const Node *entry, string rel) const
    {
        if (!parser_) return NULL;
        NodeSet links = Find(entry, "//atom:link");
//        NodeSet links = Find(entry, "./atom:link");
        for (unsigned int i = 0; i < links.size(); ++i)
        {
            const Element *nodeElement = dynamic_cast<const Element*>(links[i]);
            const Element::AttributeList& attributes = nodeElement->get_attributes();

            Element::AttributeList::const_iterator iter;
            for (iter = attributes.begin(); iter != attributes.end(); ++iter)
            {
                const xmlpp::Attribute *attribute = *iter;
                
                cerr << attribute->get_name() << "   " << attribute->get_value() << endl;
                
                if (attribute->get_name() == "rel" && attribute->get_value() == rel)
                    return nodeElement;
            }
            cerr << endl;
        }
        return NULL;
    }

} // namespace gdata
