/* 
 * File:   GBLPOP.cpp
 * Author: wuntyng
 * 
 * Created on June 18, 2012, 2:37 PM
 */

#include "GblPopDownloader.h"

GblPopDownloader::GblPopDownloader()
{
    this->sample_size = 0;
}

GblPopDownloader::~GblPopDownloader()
{   
    if(this->popgeno_map.empty() == false)
    {
        this->popgeno_it = this->popgeno_map.begin();
        while(this->popgeno_it != this->popgeno_map.end())
        {
            delete this->popgeno_it->first;
            ++this->popgeno_it;
        }
        this->popgeno_map.clear();
    }
}

bool GblPopDownloader::parse(const xercesc::DOMElement* _pa_elem, const xercesc::DOMElement* _ch_elem)
{
    const XMLCh* elem_tag_xml = _ch_elem->getTagName();
    assert(elem_tag_xml != NULL);
    
    if( XMLString::equals(_ch_elem->getTagName(), this->transcode("Population")))
    {
        this->pop_id = atoi(this->transcode(_ch_elem->getAttribute(this->transcode("popId"))));
        this->pop_name = this->transcode(_ch_elem->getAttribute(this->transcode("locPopId")));
        
        this->pop_it = this->pop_map.find(pop_id);
        if(this->pop_it == this->pop_map.end())
            this->pop_it = this->pop_map.insert(pair<int, string>(this->pop_id, this->pop_name)).first;
    }
    
    if( XMLString::equals(_ch_elem->getTagName(), this->transcode("ByPop")))
    {
        this->pop_it = this->pop_map.find(atoi(this->transcode(_ch_elem->getAttribute(this->transcode("popId")))));
        if(this->pop_it != this->pop_map.end())
            this->sample_size = atoi(this->transcode(_ch_elem->getAttribute(this->transcode("sampleSize"))));
    }
    
    if((XMLString::equals(_ch_elem->getTagName(), this->transcode("GTypeFreq"))) && (XMLString::equals(_pa_elem->getTagName(), this->transcode("ByPop"))))
    {
        if(this->pop_it != this->pop_map.end())
        {
            string pop_geno = this->transcode(_ch_elem->getAttribute(this->transcode("gtype")));
            double allele_freq = (double)atof(this->transcode(_ch_elem->getAttribute(this->transcode("freq"))));

            unsigned sample_count = (int)((allele_freq * this->sample_size) + 0.5);

            if(pop_geno.length() == 3)
            {
                PopGeno popgeno;
                popgeno.population = this->pop_it->second;
                popgeno.genotype = pop_geno;

                this->popgeno_it = this->popgeno_map.find(&popgeno);
                if(this->popgeno_it == this->popgeno_map.end())
                {
                    this->popgeno_it = this->popgeno_map.insert(pair<PopGeno*, unsigned>(new PopGeno(&popgeno), 0)).first;
                    assert(this->popgeno_it != this->popgeno_map.end());
                }
                this->popgeno_it->second+=sample_count;
            }
        }
    }
    
    return true;
}

void GblPopDownloader::download_xml(unsigned _rsid)
{
    char url[2048] = {0};
    sprintf(url, "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=snp&id=%d&retmode=xml&rettype=genxml", _rsid);

    CkSpider spider;
    spider.Initialize(url);
    spider.put_MaxResponseSize(3000000);

    if (spider.CrawlNext() == true)
    {
        if (spider.lastHtml() != NULL)
        {
            FILE *tmp_xml_file = fopen("genxml.xml", "w");
            assert(tmp_xml_file != NULL);
            fprintf(tmp_xml_file, "%s", spider.lastHtml());
            fclose(tmp_xml_file);
            tmp_xml_file = NULL;

            bool ret = this->parse_file("genxml.xml");
            assert(ret == true);
        }
    }
    spider.ClearFailedUrls();
    spider.ClearOutboundLinks();
    spider.ClearSpideredUrls();
    
    // tabulation of genotype set completed
    this->popgeno_it = this->popgeno_map.begin();
}

bool GblPopDownloader::get_record(string& _population, string& _genotype, unsigned& _sample_count)
{
    if (this->popgeno_it == this->popgeno_map.end())
        return false;
    _population = this->popgeno_it->first->population;
    _genotype = this->popgeno_it->first->genotype;
    _sample_count = this->popgeno_it->second;
    
    ++this->popgeno_it;
    return true;
}

size_t GblPopDownloader::size() const
{
    return this->popgeno_map.size();
}
