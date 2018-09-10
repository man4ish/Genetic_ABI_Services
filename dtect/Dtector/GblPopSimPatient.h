/* 
 * File:   GblPopSimPatient.h
 * Author: jameswong
 *
 * Created on June 22, 2012, 10:33 PM
 */

#ifndef GBLPOPSIMPATIENT_H
#define	GBLPOPSIMPATIENT_H

#include <string>
#include <map>
using namespace std;

class GblPopSimPatient
{
private:
    map<string, string> assay_vf_map;
    map<string, string>::iterator assay_vf_it;
    
public:
    GblPopSimPatient();
    ~GblPopSimPatient();
    
    void insert(
        string _assayid,
        string _vf_call);
    
    void rewind();
    
    bool get_record(
        string& _assayid,
        string& _vf_call);
};

#endif	/* GBLPOPSIMPATIENT_H */

