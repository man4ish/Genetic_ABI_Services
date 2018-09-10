#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>

FILE *g_pfIn=NULL, *g_pfOut=NULL;


typedef struct _stGeneralInfo
{
    char *pcQrCode;
    char *pcImages_endTag;
    char *pcGeneralInfo_Tag;  
    char *pcPatientName;
    char *pcPatientId;     
    char *pcPatientAddress;
    char *pcPatientGender;
    char *pcPatientDOB;
    char *pcSampleId;
    char *pcPrimarySampleType;
    char *pcSampleCollectionDate;
    char *pcSampleReceiptDate;
    char *pcReportDate;
    char *pcReferenceNo;
    char *pcDoctorName;
    char *pcDoctorAddress;
    char *pcInterpretResult;
    char *pcCoverTitle;
    char *pcChipType;
    char *pcPartnerCode;
    char *pcLabel_YourDrugProfile;
    char cGender;

    _stGeneralInfo()
    {
        pcQrCode = NULL;
        pcImages_endTag = NULL;
        pcGeneralInfo_Tag = NULL;
	pcPatientName = NULL;
	pcPatientId = NULL;
	pcPatientAddress = NULL;
	pcPatientGender = NULL;
	pcPatientDOB = NULL;
	pcSampleId = NULL;
	pcPrimarySampleType = NULL;
	pcSampleCollectionDate = NULL;
	pcSampleReceiptDate = NULL;
	pcReportDate = NULL;
	pcReferenceNo = NULL;
	pcDoctorName = NULL;
	pcDoctorAddress = NULL;
	pcInterpretResult = NULL;
	pcCoverTitle = NULL;
	pcChipType = NULL;
	pcPartnerCode = NULL;
	pcLabel_YourDrugProfile = NULL;
    } 

    ~_stGeneralInfo()
    {
        if (pcQrCode) delete[] pcQrCode;
        if (pcImages_endTag) delete[] pcImages_endTag;
        if (pcGeneralInfo_Tag) delete[] pcGeneralInfo_Tag;   
        if (pcPatientName) delete[] pcPatientName;
        if (pcPatientId) delete[] pcPatientId;
        if (pcPatientId) delete[] pcPatientAddress;
        if (pcPatientGender) delete[] pcPatientGender;
        if (pcPatientDOB) delete[] pcPatientDOB;
        if (pcSampleId ) delete[] pcSampleId;
        if (pcPrimarySampleType) delete[] pcPrimarySampleType;
        if (pcSampleCollectionDate) delete[] pcSampleCollectionDate;
        if (pcSampleReceiptDate) delete[] pcSampleReceiptDate;
        if (pcReportDate) delete[] pcReportDate;
        if (pcReferenceNo) delete[] pcReferenceNo;
        if (pcDoctorName) delete[] pcDoctorName;
        if (pcDoctorAddress) delete[] pcDoctorAddress;
        if (pcInterpretResult) delete[] pcInterpretResult;
        if (pcCoverTitle) delete[] pcCoverTitle;
        if (pcChipType) delete[] pcChipType;
        if (pcPartnerCode) delete[] pcPartnerCode;
        if (pcLabel_YourDrugProfile) delete[] pcLabel_YourDrugProfile;
    }

} stGeneralInfo;


stGeneralInfo g_stGeneralInfo;

struct tm *g_ptm;
char *g_pcOutFolder=NULL, *g_pcChipType=NULL, *g_pcCurrDate_fmt1=NULL, *g_pcCurrDate_fmt2=NULL;
unsigned short g_usSampleId;


const char *ADDRESS = "27-9, Level 9, Signature Office,`Bandar Mid-Valley,`59200 Kuala Lumpur, Malaysia";

const char *TAG_QRCODE = "    <QrCode>";
const int TAG_QRCODE_LEN = strlen(TAG_QRCODE);

const char *TAG_IMAGES_END_TAG = "  </Images>";
const int TAG_IMAGES_END_TAG_LEN = strlen(TAG_IMAGES_END_TAG);

const char *TAG_GENERAL_INFO = "  <GeneralInformation>";
const int TAG_GENERAL_INFO_LEN = strlen(TAG_GENERAL_INFO);

const char *TAG_PATIENT_NAME = "    <PatientName";
const int TAG_PATIENT_NAME_LEN = strlen(TAG_PATIENT_NAME);

const char *TAG_PATIENT_ID = "    <PatientId";
const int TAG_PATIENT_ID_LEN = strlen(TAG_PATIENT_ID);

const char *TAG_PATIENT_ADDRESS = "    <PatientAddress";
const int TAG_PATIENT_ADDRESS_LEN = strlen(TAG_PATIENT_ADDRESS);

const char *TAG_PATIENT_GENDER = "    <PatientGender";
const int TAG_PATIENT_GENDER_LEN = strlen(TAG_PATIENT_GENDER);

const char *TAG_PATIENT_DOB = "    <PatientDOB";
const int TAG_PATIENT_DOB_LEN = strlen(TAG_PATIENT_DOB);

const char *TAG_SAMPLE_ID = "    <SampleId";
const int TAG_SAMPLE_ID_LEN = strlen(TAG_SAMPLE_ID);

const char *TAG_PRIMARY_SAMPLE_TYPE = "    <PrimarySampleType";
const int TAG_PRIMARY_SAMPLE_TYPE_LEN = strlen(TAG_PRIMARY_SAMPLE_TYPE);

const char *TAG_SAMPLE_COLLECTION_DATE = "    <SampleCollectionDate";
const int TAG_SAMPLE_COLLECTION_DATE_LEN = strlen(TAG_SAMPLE_COLLECTION_DATE);

const char *TAG_SAMPLE_RECEIPT_DATE = "    <SampleReceiptDate";
const int TAG_SAMPLE_RECEIPT_DATE_LEN = strlen(TAG_SAMPLE_RECEIPT_DATE);

const char *TAG_REPORT_DATE = "    <ReportDate";
const int TAG_REPORT_DATE_LEN = strlen(TAG_REPORT_DATE);

const char *TAG_REFERENCE_NO = "    <ReferenceNo";
const int TAG_REFERENCE_NO_LEN = strlen(TAG_REFERENCE_NO);

const char *TAG_DOCTOR_NAME = "    <DoctorName";
const int TAG_DOCTOR_NAME_LEN = strlen(TAG_DOCTOR_NAME);

const char *TAG_DOCTOR_ADDRESS = "    <DoctorAddress";
const int TAG_DOCTOR_ADDRESS_LEN = strlen(TAG_DOCTOR_ADDRESS);

const char *TAG_INTERPRET_RESULT = "    <InterpretResult";
const int TAG_INTERPRET_RESULT_LEN = strlen(TAG_INTERPRET_RESULT);

const char *TAG_COVER_TITLE = "    <CoverTitle";
const int TAG_COVER_TITLE_LEN = strlen(TAG_COVER_TITLE);

const char *TAG_CHIPTYPE = "    <ChipType";
const int TAG_CHIPTYPE_LEN = strlen(TAG_CHIPTYPE);

const char *TAG_PARTNERCODE = "    <PartnerCode";
const int TAG_PARTNERCODE_LEN = strlen(TAG_PARTNERCODE);

const char *TAG_LABEL_YOUR_DRUG_PROFILE = "    <Label_YourDrugProfile";
const int TAG_LABEL_YOUR_DRUG_PROFILE_LEN = strlen(TAG_LABEL_YOUR_DRUG_PROFILE);

const char *TAG_GENERAL_INFO_END = "  </GeneralInformation>";  
const int TAG_GENERAL_INFO_END_LEN = strlen(TAG_GENERAL_INFO_END);

void ProcessFile()
{
    fprintf(stdout,"ProcessFile...\n");

    char acbuf[409600], *pChr=NULL, *pChr2=NULL, *pcTmp=NULL;
    unsigned long ulTest1=0, ulTest2=0;  bool bFinishedProcessedGeInfo=false, bGenInfo = false;

    while (!feof(g_pfIn)){
        if (!fgets(acbuf,sizeof(acbuf),g_pfIn)) break;

        if (++ulTest1 == 1000) {fprintf(stdout,"Total lines of data processed = %lu x 10K\n",++ulTest2); ulTest1=0;} 

        pChr = strchr(acbuf,'\r'); if (pChr) *pChr=0;
        pChr = strchr(acbuf,'\n'); if (pChr) *pChr=0;

        if (bFinishedProcessedGeInfo) { fprintf(g_pfOut,"%s\n",acbuf); continue; }   

        if (!bGenInfo) { 
           if (strncmp(acbuf,TAG_QRCODE,TAG_QRCODE_LEN)==0)
           {   
               g_stGeneralInfo.pcQrCode = new char[4096]; 
               strcpy(g_stGeneralInfo.pcQrCode,acbuf); 
               bGenInfo = true; continue; 
           } else {
              fprintf(g_pfOut,"%s\n",acbuf);
           }
        }

        if (!bGenInfo) { continue; }

        if (strncmp(acbuf,TAG_GENERAL_INFO_END,TAG_GENERAL_INFO_END_LEN)==0 )
        {
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcQrCode);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcImages_endTag);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcGeneralInfo_Tag);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcPatientName);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcPatientId);
	     fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcPatientAddress);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcPatientGender);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcPatientDOB);
    	     fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcSampleId);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcPrimarySampleType);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcSampleCollectionDate);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcSampleReceiptDate);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcReportDate);
    	     fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcReferenceNo);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcDoctorName);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcDoctorAddress);
    	     fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcInterpretResult);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcCoverTitle);
    	     fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcChipType);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcPartnerCode);
             fprintf(g_pfOut,"%s\n",g_stGeneralInfo.pcLabel_YourDrugProfile);
             fprintf(g_pfOut,"%s\n",acbuf);  
             
             bFinishedProcessedGeInfo = true; bGenInfo = false; continue;
        }       

        if (strncmp(acbuf,TAG_IMAGES_END_TAG,TAG_IMAGES_END_TAG_LEN)==0)  
        {
             g_stGeneralInfo.pcImages_endTag = new char[4096]; strcpy(g_stGeneralInfo.pcImages_endTag,acbuf);  
        } 
        else if (strncmp(acbuf,TAG_GENERAL_INFO,TAG_GENERAL_INFO_LEN)==0 )
        {
             g_stGeneralInfo.pcGeneralInfo_Tag = new char[4096]; strcpy(g_stGeneralInfo.pcGeneralInfo_Tag,acbuf);            
        } 
        else if (strncmp(acbuf,TAG_PATIENT_NAME,TAG_PATIENT_NAME_LEN)==0)
        {
             g_stGeneralInfo.pcPatientName = new char[4096]; strcpy(g_stGeneralInfo.pcPatientName,acbuf);
        } 
        else if (strncmp(acbuf,TAG_PATIENT_ID,TAG_PATIENT_ID_LEN)==0)
        {
             g_stGeneralInfo.pcPatientId = new char[4096]; strcpy(g_stGeneralInfo.pcPatientId,acbuf);
        } 
        else if (strncmp(acbuf,TAG_PATIENT_ADDRESS,TAG_PATIENT_ADDRESS_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;

             g_stGeneralInfo.pcPatientAddress = new char[4096];
             sprintf(g_stGeneralInfo.pcPatientAddress,"%s%s%s",pChr,ADDRESS,pChr2);            
        } 
        else if (strncmp(acbuf,TAG_PATIENT_GENDER,TAG_PATIENT_GENDER_LEN)==0)
        { 
             g_stGeneralInfo.pcPatientGender = new char[4096]; strcpy(g_stGeneralInfo.pcPatientGender,acbuf);

             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; g_stGeneralInfo.cGender = pcTmp[0];

             g_usSampleId = (g_stGeneralInfo.cGender == 'F')?123:122;  
                                       
             strcpy(acbuf,g_stGeneralInfo.pcPatientName); pChr=acbuf;
             pcTmp = strchr(pChr,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;               

             if (g_stGeneralInfo.cGender == 'F') {    
                sprintf(g_stGeneralInfo.pcPatientName,"%sFemale Anonymous%s",pChr,pChr2);
             } else {
                sprintf(g_stGeneralInfo.pcPatientName,"%sMale Anonymous%s",pChr,pChr2);
             }     

             strcpy(acbuf,g_stGeneralInfo.pcPatientId); pChr=acbuf;
             pcTmp = strchr(pChr,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;
           
             if (g_stGeneralInfo.cGender == 'F') { 
                sprintf(g_stGeneralInfo.pcPatientId,"%sS123456%s",pChr,pChr2);
             } else {
                sprintf(g_stGeneralInfo.pcPatientId,"%sS123455%s",pChr,pChr2);
             }            
        }     
        else if (strncmp(acbuf,TAG_PATIENT_DOB,TAG_PATIENT_DOB_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;
             g_stGeneralInfo.pcPatientDOB = new char[4096]; 
             
             if (g_stGeneralInfo.cGender == 'F') {
                sprintf(g_stGeneralInfo.pcPatientDOB,"%s1965-01-28%s",pChr,pChr2);
             } else {
                sprintf(g_stGeneralInfo.pcPatientDOB,"%s1953-01-02%s",pChr,pChr2);
             }
        }       
        else if (strncmp(acbuf,TAG_SAMPLE_ID,TAG_SAMPLE_ID_LEN)==0)
        { 
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;
             g_stGeneralInfo.pcSampleId = new char[4096];

             sprintf(g_stGeneralInfo.pcSampleId,"%s000%u%s",pChr,g_usSampleId,pChr2);
        }
        else if (strncmp(acbuf,TAG_PRIMARY_SAMPLE_TYPE,TAG_PRIMARY_SAMPLE_TYPE_LEN)==0)
        {
             g_stGeneralInfo.pcPrimarySampleType = new char[4096];
             strcpy(g_stGeneralInfo.pcPrimarySampleType,acbuf);
        } 
        else if (strncmp(acbuf,TAG_SAMPLE_COLLECTION_DATE,TAG_SAMPLE_COLLECTION_DATE_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;
             
             g_stGeneralInfo.pcSampleCollectionDate = new char[4096];
             sprintf(g_stGeneralInfo.pcSampleCollectionDate,"%s%s%s",pChr,g_pcCurrDate_fmt2,pChr2);
        } 
        else if (strncmp(acbuf,TAG_SAMPLE_RECEIPT_DATE,TAG_SAMPLE_RECEIPT_DATE_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;            

             g_stGeneralInfo.pcSampleReceiptDate = new char[4096];
             sprintf(g_stGeneralInfo.pcSampleReceiptDate,"%s%s%s",pChr,g_pcCurrDate_fmt2,pChr2);
        }
        else if (strncmp(acbuf,TAG_REPORT_DATE,TAG_REPORT_DATE_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;

             g_stGeneralInfo.pcReportDate = new char[4096];
             sprintf(g_stGeneralInfo.pcReportDate,"%s%s%s",pChr,g_pcCurrDate_fmt2,pChr2);
        }
        else if (strncmp(acbuf,TAG_REFERENCE_NO,TAG_REFERENCE_NO_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,' '); *pcTmp=0;
             g_stGeneralInfo.pcReferenceNo = new char[4096];
             sprintf(g_stGeneralInfo.pcReferenceNo,"%s000%u%s",pChr,g_usSampleId,pChr2);

             g_pcChipType = new char[strlen(pChr2)+1]; 
             strcpy(g_pcChipType,pChr2+1); g_pcChipType[5]=0;

             strcpy(acbuf,g_stGeneralInfo.pcQrCode); pChr=acbuf;
             pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0; 
              
             g_stGeneralInfo.pcQrCode = new char[4096];
            
             g_usSampleId =  (g_stGeneralInfo.cGender == 'F')? 123: 122;
             sprintf(g_stGeneralInfo.pcQrCode,"%s%s/%s_000%u_%s_AAA88_%s%02d%02d%02d.png%s",
                     pChr,g_pcOutFolder,g_pcChipType,g_usSampleId,g_pcCurrDate_fmt1,
                     g_pcCurrDate_fmt1,g_ptm->tm_hour,g_ptm->tm_min,g_ptm->tm_sec,pChr2);
        }
        else if (strncmp(acbuf,TAG_DOCTOR_NAME,TAG_DOCTOR_NAME_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;
             g_stGeneralInfo.pcDoctorName = new char[4096]; 
             sprintf(g_stGeneralInfo.pcDoctorName,"%sDr Pawel Suwinski%s",pChr,pChr2);
        } 
        else if (strncmp(acbuf,TAG_DOCTOR_ADDRESS,TAG_DOCTOR_ADDRESS_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;

             g_stGeneralInfo.pcDoctorAddress = new char[4096];
             sprintf(g_stGeneralInfo.pcDoctorAddress,"%s%s%s",pChr,ADDRESS,pChr2);
        } 
        else if (strncmp(acbuf,TAG_INTERPRET_RESULT,TAG_INTERPRET_RESULT_LEN)==0)
        {
             g_stGeneralInfo.pcInterpretResult = new char[4096];    
 
             strcpy(g_stGeneralInfo.pcInterpretResult,acbuf);  
        }  
        else if (strncmp(acbuf,TAG_COVER_TITLE,TAG_COVER_TITLE_LEN)==0)
        {
             g_stGeneralInfo.pcCoverTitle = new char[4096];

             strcpy(g_stGeneralInfo.pcCoverTitle,acbuf);
        }
        else if (strncmp(acbuf,TAG_CHIPTYPE,TAG_CHIPTYPE_LEN)==0)
        {
             g_stGeneralInfo.pcChipType = new char[4096];

             strcpy(g_stGeneralInfo.pcChipType,acbuf);

             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pChr2=0; 
        }
        else if (strncmp(acbuf,TAG_PARTNERCODE,TAG_PARTNERCODE_LEN)==0)
        {
             pChr = acbuf; pcTmp = strchr(acbuf,'>')+1; pChr2 = strchr(pcTmp,'<'); *pcTmp=0;
             g_stGeneralInfo.pcPartnerCode = new char[4096];

             sprintf(g_stGeneralInfo.pcPartnerCode,"%sMGRC%s",pChr,pChr2);
        }
        else if (strncmp(acbuf,TAG_LABEL_YOUR_DRUG_PROFILE,TAG_LABEL_YOUR_DRUG_PROFILE_LEN)==0)
        {
             g_stGeneralInfo.pcLabel_YourDrugProfile = new char[4096];
             strcpy(g_stGeneralInfo.pcLabel_YourDrugProfile,acbuf);
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc != 3){
        fprintf(stdout,"Invalid Parameter(s)!\n");   
        fprintf(stdout,"Usage: %s <Inputfile Name> <Absolute Output Folder Path>\n", argv[0]);
        goto ExitRtn; 
    }

    g_pfIn = fopen(argv[1],"r"); if (!g_pfIn) {fprintf(stdout,"Failed to open %s for reading.\n",argv[1]); goto ExitRtn;}  

    g_pcOutFolder = new char[strlen(argv[2])+1];
    strcpy(g_pcOutFolder, argv[2]);
 
    char acFileName[1024];  sprintf(acFileName,"%s/Sample.xml",g_pcOutFolder);

    g_pfOut = fopen(acFileName,"w"); if (!g_pfOut) {fprintf(stdout,"Failed to open %s for writing.\n",acFileName); goto ExitRtn;}

    g_pcChipType = argv[1]; g_pcChipType[5]=0;
    
    time_t ttime; (void) time(&ttime); g_ptm = localtime(&ttime);
    g_pcCurrDate_fmt1 = new char[1024];

    sprintf(g_pcCurrDate_fmt1,"20%02d%02d%02d",g_ptm->tm_year-100,g_ptm->tm_mon+1,g_ptm->tm_mday);

    g_pcCurrDate_fmt2 = new char[1024];
    sprintf(g_pcCurrDate_fmt2,"20%02d-%02d-%02d",g_ptm->tm_year-100,g_ptm->tm_mon+1,g_ptm->tm_mday);    

    ProcessFile();

ExitRtn:
    if (g_pfIn) fclose(g_pfIn); if (g_pfOut) fclose(g_pfOut); 
    if (g_pcOutFolder) delete[] g_pcOutFolder; 
    if (g_pcCurrDate_fmt1) delete[] g_pcCurrDate_fmt1;  
    if (g_pcCurrDate_fmt2) delete[] g_pcCurrDate_fmt2;    
}
