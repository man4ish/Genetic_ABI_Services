/* 
 * File:   GSSDb_Private.cpp
 * Author: jameswong
 * 
 * Created on May 21, 2012, 16:43 PM
 */

#include "GSSDb.h"
#include <iostream>

using namespace std;

bool GSSDb::check_table()
{
    mysqlpp::Query qry = this->db_conn.query();
    bool ret;
    
    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "Chip("
            "ChipSK INT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "ChipType CHAR(3) NOT NULL,"
            "ChipVer CHAR(2) NOT NULL,"
            "PRIMARY KEY(ChipSK),"
            "UNIQUE(ChipType, ChipVer)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);

    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "Assay("
            "AssaySK INT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "ChipFK INT UNSIGNED NOT NULL,"
            "RsId INT UNSIGNED NOT NULL,"
            "AssayIdNK VARCHAR(16) NOT NULL,"
            "VicAllele ENUM('A', 'C', 'G', 'T') NOT NULL,"
            "FamAllele ENUM('A', 'C', 'G', 'T') NOT NULL,"
            "PRIMARY KEY(AssaySK),"
            "FOREIGN KEY(ChipFK) REFERENCES Chip(ChipSK),"
            "UNIQUE(ChipFK, RsId, AssayIdNK, VicAllele, FamAllele)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);

    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "Genotype("
            "GenotypeSK INT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "AssayFK INT UNSIGNED NOT NULL,"
            "AlleleOne ENUM('A', 'C', 'G', 'T') NOT NULL,"
            "AlleleTwo ENUM('A', 'C', 'G', 'T') NOT NULL,"
            "PRIMARY KEY (GenotypeSK),"
            "FOREIGN KEY(AssayFK) REFERENCES Assay(AssaySK),"
            "UNIQUE(AssayFK, AlleleOne, AlleleTwo)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);

    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "GenoLink("
            "GenoLinkSK INT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "GenoCurrFK INT UNSIGNED NOT NULL,"
            "GenoNextFK INT UNSIGNED NOT NULL,"
            "PRIMARY KEY (GenoLinkSK),"
            "FOREIGN KEY (GenoCurrFK) REFERENCES Genotype(GenotypeSK),"
            "FOREIGN KEY (GenoNextFK) REFERENCES Genotype(GenotypeSK),"
            "UNIQUE (GenoCurrFK, GenoNextFK)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);

    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "DisDrug("
            "DisDrugID VARCHAR(64) NOT NULL,"
            "Type ENUM('Disease', 'Drug', 'Trait') NOT NULL,"
            "Sex ENUM('M', 'F', 'B') NOT NULL,"
            "Incidence DOUBLE,"
            "Trait ENUM('Y', 'N') NOT NULL," 
            "PRIMARY KEY (DisDrugID),"
            "UNIQUE (DisDrugID, Type)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);
    
    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "RelativeRisk("
            "AssayFK INT UNSIGNED NOT NULL,"
            "DisDrugID VARCHAR(64) NOT NULL,"
            "RiskAllele ENUM('A', 'C', 'G', 'T') NOT NULL,"
            "RR_Factor DOUBLE,"
            "RN_Factor DOUBLE,"
            "NN_Factor DOUBLE,"
            "FOREIGN KEY (AssayFK) REFERENCES Assay(AssaySK),"
            "FOREIGN KEY (DisDrugID) REFERENCES DisDrug(DisDrugID),"
            "UNIQUE (AssayFK, DisDrugID)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);

    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "Effects("
            "EffectID VARCHAR(64) NOT NULL,"
            "DisDrugID VARCHAR(64) NOT NULL,"
            "GenotypeCount TINYINT UNSIGNED NOT NULL,"
            "Risk DOUBLE NOT NULL,"
            "PRIMARY KEY (EffectID),"
            "FOREIGN KEY (DisDrugID) REFERENCES DisDrug(DisDrugID),"
            "UNIQUE(DisDrugID, GenotypeCount, Risk, EffectID)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);

    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "GenoEffect("
            "GenoEffectSK INT UNSIGNED NOT NULL AUTO_INCREMENT,"
            "GenoLinkFK INT UNSIGNED NOT NULL,"
            "EffectID VARCHAR(64) NOT NULL,"
            "PRIMARY KEY (GenoEffectSK),"
            "FOREIGN KEY (GenoLinkFK) REFERENCES GenoLink(GenoLinkSK),"
            "FOREIGN KEY (EffectID) REFERENCES Effects(EffectID),"
            "UNIQUE (GenoLinkFK, EffectID)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);

    qry.reset();
    qry << "CREATE TABLE IF NOT EXISTS "
            "Citation("
            "GenoEffectFK INT UNSIGNED NOT NULL,"
            "PmId INT UNSIGNED NOT NULL,"
            "FOREIGN KEY (GenoEffectFK) REFERENCES GenoEffect(GenoEffectSK),"
            "UNIQUE (GenoEffectFK, PmId)"
            ") ENGINE=InnoDB";
    ret = this->query_execute(qry);
    assert(ret == true);
        
    return true;
}
