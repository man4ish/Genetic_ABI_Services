/* 
 * File:   SXGenerateRiskScore.cpp
 * Author: manish
 *
 * Created on October 21, 2013, 12:39 AM
 */

#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

struct description
{
    char* rsid;
    char risk_allele;
    double alle_frequency, homo_odds_ratio, hetero_odds_ratio;
    double risk, homorisk, heterorisk, homononrisk;
    char homo_risk_genotype[3], het_risk_genotype[3], homo_non_risk_genotype[3];
    description(char* _rsid, char _risk_allele, double _alle_frequency, double _homo_odds_ratio,double _hetero_odds_ratio, char* _homo_risk_genotype, char* _het_risk_genotype, char* _homo_non_risk_genotype, double _risk, double _homorisk, double _heterorisk, double _homononrisk):risk_allele(_risk_allele),alle_frequency(_alle_frequency),homo_odds_ratio(_homo_odds_ratio), hetero_odds_ratio(_hetero_odds_ratio), risk(_risk),homorisk(_homorisk),heterorisk(_heterorisk),homononrisk(_homononrisk)
    {
        int rslen = strlen(_rsid);
        rsid = new char[rslen+1];
        strcpy(rsid,_rsid);
        strcpy(homo_risk_genotype,_homo_risk_genotype);
        strcpy(het_risk_genotype,_het_risk_genotype); 
        strcpy(homo_non_risk_genotype,_homo_non_risk_genotype);        
    }
    
    ~description()
    {
        delete [] rsid;
    }
};

struct HapmapDescription
{
    char* header;
    char* value;
    HapmapDescription(char* _header, char* _value)
    {
        int headlen = strlen(_header);
        header = new char[headlen+1];
        strcpy(header,_header);
        
        int vallen = strlen(_value);
        value = new char[vallen+1];
        strcpy(value,_value);       
    }
    
    ~HapmapDescription()
    {
        delete [] header;
        delete [] value;
    }
};

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

typedef multimap<const char*, HapmapDescription*, ltstr> SNPMap;
SNPMap snpmap;

typedef multimap<const char*, description*, ltstr> DiseaseMap;
DiseaseMap Dmap;

typedef multimap<const char*, map<const char*,double,ltstr>, ltstr> DiseaseHapMap;
DiseaseHapMap disease_snp_map;

float getrisk(float ORhomo, float ORhetero, float ORhomonr, float p) {
      return ORhomo*p*p + 2*p*(1-p)*ORhetero + (1-p)*(1-p)*ORhomonr;    
}

void getHapMap(const char* dir) {
    
     ifstream fin;
     
     DIR *dp;
     struct dirent *dirp;
     
     dp = opendir( dir);
     if (dp == NULL)
     {       
         cout << "Could not open directory\n";
         exit(1);
     }
     char* rec;
     char* header;
     char line[5000];
     
     int cnt =0;
     while ((dirp = readdir( dp )))
     {
           string filepath;
           filepath += dir ;
           filepath += "/" ;
           filepath += dirp->d_name;
           
           ifstream in(filepath.c_str());                                           
           
           while(in) {
              
              in.getline(line,5000);
              int rlen = strlen(line);
              rec = new char[rlen+1];
              strcpy(rec,line); 
              
              if(in) {                                    
                 if(rec[0] == 'r' && rec[1] == 's' && rec[2] == '#')
                 {
                    header = new char[rlen+1];
                    strcpy(header,rec);
                 }
                 else
                 {
                    char recline[5000];
                    strcpy(recline,rec);
                    char* pchar = strtok(recline," ");
                    int slen= strlen(pchar);
                    char* snpid = new char[slen+1];                   
                    strcpy(snpid,pchar);
                    snpmap.insert(pair<const char*, HapmapDescription*>(snpid, new HapmapDescription(header,rec))); 
                    cout<<"Loading Hapmap "<<cnt++<<"\r";
                 }                  
             }
         } 
    }    
    closedir(dp);   
}

void readinput(const char* infile)
{
    ifstream in(infile);
    char line[1000];
    char* disease_name;
    char* disease_id;
    char rsid[12];
    char risk_allele;
    double alle_freq, homo_or, het_or, homo_nor;
    double risk, homo_risk, het_risk, hom_nr_risk;

    char homo_r_genotype[3], het_r_genotype[3], homo_nr_genotype[3];
    while(in)        
    {
        in.getline(line,1000);
        if(in)
        {
            if(line[0] != '#')
            {
                char* pch;
                pch = strtok(line,"\t");
                int count = 0;
                while(pch != NULL)
                {
                        count++;
                
                        if(count == 1)
                        {
                                int len = strlen(pch);
                                disease_name = new char[len+1];
                                strcpy(disease_name,pch);                                    
                        }
                
                        if(count == 2)
                                strcpy(rsid,pch); 
   
                        if(count == 3)
                        {
                                int len = strlen(pch);
                                disease_id = new char[len+1];
                                strcpy(disease_id,pch);
                        } 
                        if(count == 4)
                                risk_allele = pch[0];
                
                        if(count == 5)
                                alle_freq = atof(pch);
                
                        if(count == 7)                               
                                het_or = atof(pch);                     // hetero as 5th column
                
                        if(count == 6)                                  //homo as 6th column
                                homo_or = atof(pch);

                        if(count == 8)
                                homo_nor = atof(pch);
  
                        if(count == 9)
                                strcpy(homo_r_genotype,pch);
                
                        if(count == 10)
                                strcpy(het_r_genotype,pch);
                
                        if(count == 11)
                                strcpy(homo_nr_genotype,pch);
                      
                        if(count == 12) 
                                risk =atof(pch);
                        if(count == 13) 
                                homo_risk=atof(pch);
                        if(count == 14)
                                het_risk=atof(pch);
                        if(count == 15)
                                hom_nr_risk=atof(pch);
                        pch = strtok(NULL,"\t");                
                }
                Dmap.insert(pair<const char*, description*>(disease_id, new description(rsid,risk_allele,alle_freq,homo_or,het_or,homo_r_genotype,het_r_genotype,homo_nr_genotype,risk,homo_risk,het_risk,hom_nr_risk)));               
            }
        }
    }
    in.close();
}

void getriskscore()
{  
    double risk_score;
    ofstream rfile("MaxRiskfile.log");
    rfile<<"<Diseases>\n";
    for( DiseaseMap::iterator it = Dmap.begin(), end = Dmap.end(); it != end; it = Dmap.upper_bound(it->first))
    {
        pair<DiseaseMap::iterator, DiseaseMap::iterator> ppp;
        ppp = Dmap.equal_range(it->first);
        
        vector <map<const char*,double,ltstr> > supvec;
        double maxrisk=1; 
        for (DiseaseMap::iterator it2 = ppp.first; it2 != ppp.second;it2++)
        {  
            maxrisk = maxrisk*(double((*it2).second->homorisk));
            map <const char*, double, ltstr> risk;
            map <const char*, double, ltstr> riskmap;
            map <const char*, double, ltstr> valriskscore;
                 
            
            if((*it2).second->homo_risk_genotype[0] != (*it2).second->risk_allele && (*it2).second->homo_risk_genotype[1] != (*it2).second->risk_allele )
            {
                riskmap.insert(std::pair<const char*, double>((*it2).second->homo_non_risk_genotype,(double)(*it2).second->homo_odds_ratio));
                riskmap.insert(std::pair<const char*, double>((*it2).second->homo_risk_genotype,1));
                risk_score = getrisk(1, (*it2).second->hetero_odds_ratio, (*it2).second->homo_odds_ratio, 1-(*it2).second->alle_frequency);
                valriskscore.insert(std::pair<const char*, double>((*it2).second->homo_non_risk_genotype,(double)(*it2).second->homorisk));
                valriskscore.insert(std::pair<const char*, double>((*it2).second->homo_risk_genotype,(double)(*it2).second->homononrisk));
                valriskscore.insert(std::pair<const char*, double>((*it2).second->het_risk_genotype,(double)(*it2).second->heterorisk));
                  
            }
            else
            {   
                riskmap.insert(std::pair<const char*, double>((*it2).second->homo_risk_genotype,(double)(*it2).second->homo_odds_ratio));
                riskmap.insert(std::pair<const char*, double>((*it2).second->homo_non_risk_genotype,1));
                risk_score = getrisk((*it2).second->homo_odds_ratio, (*it2).second->hetero_odds_ratio, 1, (*it2).second->alle_frequency);
                valriskscore.insert(std::pair<const char*, double>((*it2).second->homo_risk_genotype,(double)(*it2).second->homorisk));
                valriskscore.insert(std::pair<const char*, double>((*it2).second->homo_non_risk_genotype,(double)(*it2).second->homononrisk));
                valriskscore.insert(std::pair<const char*, double>((*it2).second->het_risk_genotype,(double)(*it2).second->heterorisk));
 
            }
                
            valriskscore.insert(std::pair<const char*, double>("NN",0));
            riskmap.insert(std::pair<const char*, double>((*it2).second->het_risk_genotype,(double)(*it2).second->hetero_odds_ratio));
            riskmap.insert(std::pair<const char*, double>("NN",0));
              
                  
            if(snpmap.count((*it2).second->rsid)!=0) {
               pair<multimap<const char*, HapmapDescription*, ltstr>::iterator, multimap<const char*, HapmapDescription*, ltstr> ::iterator> pppp;
               int count =0;
               pppp = snpmap.equal_range((*it2).second->rsid);

               for (multimap<const char*, HapmapDescription*, ltstr>::iterator it3 = pppp.first;it3 != pppp.second; it3++) {                   
                  int hlen = strlen((*it3).second->header); 
                  char* head = new char[hlen+1];
                  strcpy(head,(*it3).second->header);
                 
                  int rlen = strlen((*it3).second->value); 
                  char* rec = new char[rlen+1];
                  strcpy(rec,(*it3).second->value); 
                  
                
                char* pchar = strtok(rec," ");
                vector <char*> genotypes;                   
                while(pchar != NULL) {
                      genotypes.push_back(pchar);
                      pchar = strtok(NULL," "); 
                }  
                
                vector<const char*> individuals;  
                char* hdr = strtok(head," ");
                while(hdr != NULL) {
                       individuals.push_back(hdr);
                       hdr = strtok(NULL," ");
                }
                
                for (int i = 11; i< genotypes.size(); i++) {                      
                      double overall_risk = double(valriskscore[genotypes[i]]);
                      risk[individuals[i]]=overall_risk;                                          
                } 
                individuals.clear();
                genotypes.clear();
              }              
            }    
            
            disease_snp_map.insert(pair<const char* ,map<const char*,double,ltstr> >((*it2).first,risk)); 
        }
        rfile<<"<"<<it->first<<">"<< maxrisk<<"<"<<it->first<<">\n";                     
    }    
    rfile<<"</Diseases>\n";
    rfile.close();
}

void write_risk_score(char* infile) {
    
     char riskfile[1000];
     strcpy(riskfile,infile);
     strcat(riskfile,".risk_score");
     vector<vector<double> > riskmatrix;
     set <const char*,ltstr> popid;
     DiseaseHapMap outmap;
     
     // getting headers from the list
     for(DiseaseHapMap::iterator it = disease_snp_map.begin(), end = disease_snp_map.end(); it != end; it = disease_snp_map.upper_bound(it->first)){             
         int n = (it->second).size(); 
         map <const char*, double,ltstr> m;
         pair<DiseaseHapMap::iterator, DiseaseHapMap::iterator> ppp;
         ppp = disease_snp_map.equal_range(it->first);
                          
         
         for (multimap<const char* ,map<const char*,double,ltstr>,ltstr >::iterator it2 = ppp.first; it2 != ppp.second;it2++){  
              int index=0;
              
              map<const char*,double,ltstr> riskmap = it2->second;
              for(map<const char*,double,ltstr>::iterator ii=riskmap.begin(); ii!=riskmap.end(); ii++)
              {
                  popid.insert(ii->first);
              }
         }
         outmap.insert(pair<const char* ,map<const char*,double,ltstr> >(it->first,m));
     }
                                    
     vector <double> riskscores;    
     vector <const char*> diseasenames;
     
  
     ofstream outfile(riskfile);
     
     for (std::set<const char*>::iterator itt=popid.begin(); itt!=popid.end(); itt++) {
         diseasenames.push_back(*itt);
     }
        
     for(  DiseaseHapMap::iterator it = disease_snp_map.begin(), end = disease_snp_map.end(); it != end; it = disease_snp_map.upper_bound(it->first))
     {    
              for (int i =0; i < popid.size() ; i++)
              riskscores.push_back(1);
              outfile << it->first<<"\t";
                                            
               pair<DiseaseHapMap::iterator, DiseaseHapMap::iterator> ppp;
               ppp = disease_snp_map.equal_range(it->first);
               
               for (DiseaseHapMap::iterator it2 = ppp.first; it2 != ppp.second;it2++)
               {                    
                   int index = 0;
                   map<const char*, double, ltstr> riskmap = it2->second;
                    for (std::set<const char*>::iterator itt = popid.begin(); itt!=popid.end(); itt++) { 
                        
                        map<const char*,double,ltstr>::iterator iitt = riskmap.find(*itt);
                                         

                        if (iitt != riskmap.end()){
                            if (iitt->second != 0) { riskscores[index] *= iitt->second; }
                        }

                        index++;
                    }      
               }
               
               riskmatrix.push_back(riskscores);
               riskscores.clear();
                
       } 
       outfile << "\n";
       int nTotalCol =  (riskmatrix[0]).size();
       
       for (int i=0; i<nTotalCol; i++){
           for (int j = 0; j< riskmatrix.size();j++){
               outfile << (riskmatrix[j])[i] << "\t";  
           }    
           outfile <<"\n";
       }
       outfile.close();
       
}

void manual() {
    cout << "\nUsage\n";
    cout << "~~~~~~~~~~~~~~SXGenRiskScore~~~~~~~~~~~~~\n";
    cout << "SXGenRiskScore <HapMap Dir> <Risk File>\n\n";
}

int main(int argc, char** argv) {
    
    if(argc != 3)
    {
        manual();
        exit(1);
    }
    
    getHapMap(argv[1]); 
    readinput(argv[2]);    
    getriskscore();
    write_risk_score(argv[2]); 
    snpmap.clear();
    Dmap.clear();
    disease_snp_map.clear();
    return 0;
}
