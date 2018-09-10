#include "DtectMapper.h"

bool DtectMapper::is_valid_call(const char* _vf_call)
{
    if (_vf_call == NULL)
        return false;
    if ((strcmp(_vf_call, "VIC/FAM") == 0) ||
        (strcmp(_vf_call, "VIC/VIC") == 0) ||
        (strcmp(_vf_call, "FAM/FAM") == 0) ||
        (strcmp(_vf_call, "A/A") == 0) ||
        (strcmp(_vf_call, "A/C") == 0) ||
        (strcmp(_vf_call, "A/G") == 0) ||
        (strcmp(_vf_call, "A/T") == 0) ||
        (strcmp(_vf_call, "C/A") == 0) ||
        (strcmp(_vf_call, "C/C") == 0) ||
        (strcmp(_vf_call, "C/G") == 0) ||
        (strcmp(_vf_call, "C/T") == 0) ||
        (strcmp(_vf_call, "G/A") == 0) ||
        (strcmp(_vf_call, "G/C") == 0) ||
        (strcmp(_vf_call, "G/G") == 0) ||
        (strcmp(_vf_call, "G/T") == 0) ||
        (strcmp(_vf_call, "T/A") == 0) ||
        (strcmp(_vf_call, "T/C") == 0) ||
        (strcmp(_vf_call, "T/G") == 0) ||
        (strcmp(_vf_call, "T/T") == 0))
        return true;
    return false;
}

bool DtectMapper::is_valid_call(
        const char* _vf_call,
        string& _call_str,
        const char _vic_allele,
        const char _fam_allele)
{
    if (_vf_call == NULL)
        return false;
    
    bool is_valid = is_valid_call(_vf_call);
    
    if ((is_valid) && (_vic_allele != 0) && (_fam_allele != 0))
    {
        char call_str[5] = {0};
        if (strcmp(_vf_call, "VIC/FAM") == 0)
            sprintf(call_str, "%c/%c", _vic_allele, _fam_allele);
        else if (strcmp(_vf_call, "VIC/VIC") == 0)
            sprintf(call_str, "%c/%c", _vic_allele, _vic_allele);
        else if (strcmp(_vf_call, "FAM/FAM") == 0)
            sprintf(call_str, "%c/%c", _fam_allele, _fam_allele);
        else
        {
            if ((_vf_call[0] == _vic_allele) && (_vf_call[2] == _fam_allele))
                sprintf(call_str, "%c/%c", _vic_allele, _fam_allele);
            else if ((_vf_call[0] == _vic_allele) && (_vf_call[2] == _vic_allele))
                sprintf(call_str, "%c/%c", _vic_allele, _vic_allele);
            else if ((_vf_call[0] == _fam_allele) && (_vf_call[2] == _fam_allele))
                sprintf(call_str, "%c/%c", _fam_allele, _fam_allele);
            else { return false; }
        }
        _call_str.assign(call_str);
    }
    return is_valid;
}

bool DtectMapper::is_valid_nucleotide(
        const char _nuc)
{
    switch(_nuc)
    {
        case 'a':
        case 'A':
        case 'c':
        case 'C':
        case 'g':
        case 'G':
        case 't':
        case 'T':
            return true;
        default:
            return false;
    }
    return false;
}