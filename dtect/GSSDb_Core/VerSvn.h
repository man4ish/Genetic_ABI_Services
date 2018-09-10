/* 
 * File:   VerSvn.h
 * Author: wuntyng
 *
 * Created on August 1, 2012, 11:27 AM
 */

/* svn propset svn:keywords "Revision HeadURL" main.cpp
 * initial set to "$Revision: 7790 $" and "$HeadURL: svn+ssh://manish@192.168.1.9/svn/repos/synamatix/gss/trunk/dtect/GSSDb_Core/VerSvn.h $"
 */

#ifndef VERSVN_H
#define	VERSVN_H
#include <vector>

#define REV "$Revision: 7790 $"
#define URL "$HeadURL: svn+ssh://manish@192.168.1.9/svn/repos/synamatix/gss/trunk/dtect/GSSDb_Core/VerSvn.h $"

using namespace std;

class VerSvn
{
private:
    int svn_revision;
    int major_number;
    int minor_number;
    int patch_number;
    bool is_trunk;
    
    inline size_t rowParser(char *_source, const char *_delimiter, vector<char*> &_fields_vec) const;
    
public:
    VerSvn();
    int get_revision() const;
    int get_major_number() const;
    int get_minor_number() const;
    int get_patch_number() const;
    bool get_is_trunk() const;
    
};

#endif	/* VERSVN_H */
