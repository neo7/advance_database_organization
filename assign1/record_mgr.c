//
// Created by Saurabh Tiwari on 08/11/16.
//
#include "record_mgr.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "object_parser.h"


int schema_size;
int total_pages;

RC
initRecordManager (void *mgmtData){
    initStorageManager();
    return RC_OK;
}

RC
shutdownRecordManager (){
    return 0;
}

RC
createTable (char *name, Schema *schema){
    SM_FileHandle fileHandle;
    char *serializedSchema;
    int isFileAccessible = access(name, F_OK);
    int createPageFileErrorCode = createPageFile(name);
    int openPageFileErrorCode = openPageFile(name, &fileHandle);

    if (isFileAccessible != NO_PAGE)
    {
        return RC_TABLE_EXISTS;
    } else if (createPageFileErrorCode != RC_OK)
    {
        return RC_FILE_NOT_FOUND;
    } else if (openPageFileErrorCode != RC_OK)
    {
        return RC_FILE_NOT_FOUND;
    }
    total_pages = 0;
    schema_size = 0;
    serializedSchema = serializeSchema(schema);

    int writeBlockErrorCode = writeBlock(START_BLOCK, &fileHandle, serializedSchema);
    if (writeBlockErrorCode == 0)
    {
        return RC_OK;
    }
    else
    {
        return RC_WRITE_FAILED;
    }


}

RC
openTable (RM_TableData *rel, char *name){
    RM_TableMgmt *tableMgmt = (RM_TableMgmt *) malloc(sizeof(RM_TableMgmt));
    FILE *filePointer;

    filePointer = fopen(name, "r+");
    char *readPointer = (char *)calloc(PAGE_SIZE, sizeof(char));
    fgets(readPointer, PAGE_SIZE, filePointer);


    BM_BufferPool *bm_bufferPool = ((BM_BufferPool *) malloc (sizeof(BM_BufferPool)));
    tableMgmt->bm_bufferPool = bm_bufferPool;
    BM_PageHandle *bm_pageHandle = ((BM_PageHandle *) malloc (sizeof(BM_PageHandle)));
    initBufferPool(bm_bufferPool, name, 6, RS_FIFO, NULL);
    pinPage(bm_bufferPool, bm_pageHandle, 0);
    tableMgmt->pageNum = 0;
    rel->schema = stringToSchemaParser(bm_pageHandle->data);
    rel->name = name;
    tableMgmt->bm_pageHandle = bm_pageHandle;
    rel->mgmtData = tableMgmt;

    free(readPointer);
    return RC_OK;




    return 0;
}

RC
closeTable (RM_TableData *rel){
    unpinPage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool, ((RM_TableMgmt *)rel->mgmtData)->bm_pageHandle);
    int error_code = shutdownBufferPool(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool);
    free(rel->schema);
    free(rel->mgmtData);
    return error_code;
}

RC
deleteTable (char *name){
    return destroyPageFile(name);
}

int
getNumTuples (RM_TableData *rel){
    int tupleCount = 0;
    Record *record = (Record *)malloc(sizeof(Record));
    RID rid;
    rid.page = 1;
    rid.slot = 0;

    while(rid.page > 0 && rid.page < total_pages)
    {
        RC record_present = getRecord(rel, rid, record);
        if(record_present == RC_OK)
        {
            tupleCount += 1;
            rid.page += 1;
            rid.slot = 0;

        }
    }

    free(record);
    record = NULL;

    return tupleCount;

}

// handling records in a table
RC
insertRecord (RM_TableData *rel, Record *record){

//    Record * tempr = (Record *)calloc(1,sizeof(record));
    RID temprid ;
    temprid.page = 1 ;
    temprid.slot = 0 ;
    // Traversing to the last page available to insert the record
    while(temprid.page > 0 && temprid.page < total_pages){
        temprid.page += 1 ;
    }
    //Marking Page as free using pageNum
    int freePageNUM = temprid.page ;
    ((RM_TableMgmt *)rel->mgmtData)->pageNum = freePageNUM ;

    BM_PageHandle * temp_page = MAKE_PAGE_HANDLE() ;
    record->id.page = freePageNUM ;
    record->id.slot = 0 ;

    char *serializedRec = serializeRecord(record,rel->schema);

    pinPage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool,temp_page,freePageNUM);

    int i ;
    //Initializing the record in page to \0
    memset(temp_page->data, '\0', strlen(temp_page->data));
//    for(i=0;i<sizeof(temp_page->data);i++){
//        memcpy(&temp_page->data[i],'\0', sizeof(char));
//    }

    //Writing the serialized record to the page struct
    sprintf(temp_page->data,"%s",serializedRec);

    //Initializing error check variables for markDirty, unpinPage and forcePage
    RC error_dirtymark;
    RC error_unpinPage;
    RC error_forcePage;

    //Marking page Dirty due to writing data
    error_dirtymark = markDirty(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool,temp_page);
    if(error_dirtymark != RC_OK){
        return error_dirtymark;
    }

    //Unpinning the page after use and marking it dirty
    error_unpinPage = unpinPage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool,temp_page);
    if(error_unpinPage != RC_OK){
        return error_unpinPage ;
    }

    // Pushing the data to the Page
    error_forcePage = forcePage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool,temp_page);
    if(error_forcePage != RC_OK){
        return error_forcePage ;
    }

    ((RM_TableMgmt *)rel->mgmtData)->pageNum += 1 ;
    total_pages+=1 ;
    free(temp_page);
    return RC_OK;
}

RC
deleteRecord (RM_TableData *rel, RID id)
{

    char *tombStoneFLAG = "TOMBESTONED" ;

    char *flaggedRecordPlaceHolder = (char *)malloc(sizeof(Record)+strlen(tombStoneFLAG));
    if(id.page >0 && id.page<= total_pages)
    {
        BM_PageHandle *tempPage = MAKE_PAGE_HANDLE();

        RC error_pinpg;
        error_pinpg = pinPage(((RM_TableMgmt *) rel->mgmtData)->bm_bufferPool, tempPage, id.page);
        if (error_pinpg != RC_OK)
        {
            return error_pinpg;
        }

        //WE Concantenating the strings and assign it to temp
        strcat(flaggedRecordPlaceHolder, tombStoneFLAG);
        strcat(flaggedRecordPlaceHolder, tempPage->data);

        //we update page id
        tempPage->pageNum = id.page;

        //We rewrite the marked record in our Page file

        int i;
        //Initializing the record in page to \0
        memset(tempPage->data, '\0', strlen(tempPage->data));

        sprintf(tempPage->data, "%s", flaggedRecordPlaceHolder);

        //Initializing error check variables for markDirty, unpinPage and forcePage
        RC error_dirtymark;
        RC error_unpinPage;
        RC error_forcePage;

        //Marking page Dirty due to writing data
        error_dirtymark = markDirty(((RM_TableMgmt *) rel->mgmtData)->bm_bufferPool, tempPage);
        if (error_dirtymark != RC_OK)
        {
            return error_dirtymark;
        }

        //Unpinning the page after use and marking it dirty
        error_unpinPage = unpinPage(((RM_TableMgmt *) rel->mgmtData)->bm_bufferPool, tempPage);
        if (error_unpinPage != RC_OK)
        {
            return error_unpinPage;
        }

        // Pushing the data to the Page
        error_forcePage = forcePage(((RM_TableMgmt *) rel->mgmtData)->bm_bufferPool, tempPage);
        if (error_forcePage != RC_OK)
        {
            return error_forcePage;
        }
        free(tempPage);
        return RC_OK;
    }
    return RC_RM_NO_MORE_TUPLES;

}

RC
updateRecord (RM_TableData *rel, Record *record)
{

    int i;

    char *serialized = serializeRecord(record, rel->schema);

    if (record->id.page > total_pages || record->id.page < 0)
    {
        return RC_RM_NO_MORE_TUPLES;
    }

    BM_PageHandle *ph = MAKE_PAGE_HANDLE();

    pinPage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool, ph, record->id.page);
    memset(ph->data, '\0', strlen(ph->data));
    sprintf(ph->data, "%s", serialized);

    markDirty(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool, ph);
    unpinPage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool, ph);
    forcePage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool, ph);

    free(serialized);
    free(ph);

    return RC_OK;
}

RC
getRecord (RM_TableData *rel, RID id, Record *record){

    if(id.page > 0 && id.page <= total_pages){

        BM_PageHandle *pagehandle = MAKE_PAGE_HANDLE() ;

        //Pining the Page
        pinPage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool,pagehandle,id.page);

        char *temp_data_record = (char *)malloc(sizeof(char) * strlen(pagehandle->data));

        strcpy(temp_data_record,pagehandle->data);

        // Setting the record id
        record->id = id ;

        Record *converted_record = stringToRecordParser(temp_data_record, rel->schema);

        unpinPage(((RM_TableMgmt *)rel->mgmtData)->bm_bufferPool,pagehandle);

        // Setting the data read from pagefile after Deserializing it
        record->data = converted_record->data ;

        free(converted_record);
        free(pagehandle);

        return RC_OK ;

    }

    else {
        return RC_RM_NO_MORE_TUPLES ;
    }

    return RC_OK ;
}

// scans
RC
startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond){
    RM_ScanMgmt *scanMgmt = (RM_ScanMgmt *)malloc(sizeof(RM_ScanMgmt));
    Record *record = (Record *) malloc(sizeof(Record));
    scanMgmt->expr = cond;
    scanMgmt->record = record;
    scanMgmt->rid.page = 1;
    scanMgmt->rid.slot = 0;
    scan->mgmtData = scanMgmt;

    return RC_OK;
}

RC
next (RM_ScanHandle *scan, Record *record)
{
    RID rid;
    Expr* condition;
    Value *con_result;

    rid = ((RM_ScanMgmt *)scan->mgmtData)->rid;
    condition = ((RM_ScanMgmt *)scan->mgmtData)->expr;

    if( condition == NULL ) {
        if (rid.page > 0 && rid.page < total_pages) {
            getRecord(scan->rel, rid, ((RM_ScanMgmt *)scan->mgmtData)->record);
            record = ((RM_ScanMgmt *)scan->mgmtData)->record;

            rid.page = ((RM_ScanMgmt *)scan->mgmtData)->rid.page + 1;
            rid.slot = ((RM_ScanMgmt *)scan->mgmtData)->rid.slot;

            return RC_OK;
        }
    }
    else {
        if (rid.page > 0 && rid.page < total_pages) {

            getRecord(scan->rel, rid, ((RM_ScanMgmt *)scan->mgmtData)->record);
            evalExpr(((RM_ScanMgmt *)scan->mgmtData)->record, scan->rel->schema, condition, &con_result);
            if (con_result->dt == DT_BOOL && con_result->v.boolV == true) {
                record = ((RM_ScanMgmt *)scan->mgmtData)->record;

                return RC_OK;
            }
            else {
                rid.page = ((RM_ScanMgmt *)scan->mgmtData)->rid.page + 1;
                rid.slot = ((RM_ScanMgmt *)scan->mgmtData)->rid.slot;
            }
        }
    }

    ((RM_ScanMgmt *)scan->mgmtData)->rid.page = 1;


    return RC_RM_NO_MORE_TUPLES;
}

RC
closeScan (RM_ScanHandle *scan){

    RM_ScanMgmt *scanMgmt = (RM_ScanMgmt *)scan->mgmtData;
    free(scanMgmt->record);
    free(scanMgmt->expr);
    free(scanMgmt);
    scanMgmt = NULL;

    return RC_OK;

}

// dealing with schemas
int
getRecordSize (Schema *schema)
{
    //change the logic if not shows the correct result.
    int recordSize = getRecordSizeOffset(schema, schema->numAttr);
    return recordSize;
}

Schema
*createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{
    Schema *blueprint = (Schema *) malloc (sizeof(Schema));
    blueprint->numAttr = numAttr;
    blueprint->attrNames = attrNames;
    blueprint->dataTypes = dataTypes;
    blueprint->typeLength = typeLength;
    blueprint->keySize = keySize;
    blueprint->keyAttrs = keys;

    return blueprint;

}

RC
freeSchema (Schema *schema)
{
    free(schema);
    return RC_OK;
}

// dealing with records and attribute values
RC createRecord (Record **record, Schema *schema)
{
    *record = (Record *) malloc(sizeof(Record));
    char *data = (char *)malloc(getRecordSize(schema));
    (*record)->data = data;

    return RC_OK;

}

RC freeRecord (Record *record)
{
    free(record);
    return RC_OK;
}

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
    //note: recheck string assignment on errors.
    int offset = getRecordSizeOffset(schema, attrNum);
    char *attributeData = record->data + offset;
    int typeLength = schema->typeLength[attrNum];

    *value = (Value *)malloc(sizeof(Value));
    (*value)->dt = schema->dataTypes[attrNum];
    if (schema->dataTypes[attrNum] == DT_INT)
    {
        memcpy(&((*value)->v.intV), attributeData, typeLength);
    } else if (schema->dataTypes[attrNum] == DT_FLOAT)
    {
        memcpy(&((*value)->v.floatV), attributeData, typeLength);
    }
    else if (schema->dataTypes[attrNum] == DT_BOOL)
    {
        memcpy(&((*value)->v.boolV), attributeData, typeLength);
    }
    else if (schema->dataTypes[attrNum] == DT_STRING)
    {
        memcpy((*value)->v.stringV, attributeData, typeLength);
    } else
    {
        return RC_RM_UNKOWN_DATATYPE;
    }

    return RC_OK;

}

RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
    //Modifying rm_serializer serializeAttr

    char *attrData;

    //calculate the offset values
    int offset = getRecordSizeOffset(schema, attrNum);
    attrData = record->data + offset;

    //switch on attributes datatype value
    switch(schema->dataTypes[attrNum])
    {
        case DT_INT:
        {
            memcpy(attrData,&(value->v.intV) ,sizeof(int));		//copy the newly set attribute value
        }
            break;

        case DT_STRING:
        {
            char *buf;
            int len = schema->typeLength[attrNum];
            buf = (char *) malloc(len);
            buf = value->v.stringV;
            memcpy(attrData,buf,len);
        }
            break;

        case DT_FLOAT:
        {
            memcpy(attrData,&(value->v.floatV), sizeof(float));
        }
            break;

        case DT_BOOL:
        {
            memcpy(attrData,&(value->v.boolV) ,sizeof(bool));
        }
            break;

        default:
            return RC_RM_UNKOWN_DATATYPE;
    }
    return RC_OK;
}

int
getRecordSizeOffset(Schema *schema, int attrNum)
{
    int offset = 0;
    int attrPos = 0;

    for(attrPos = 0; attrPos < attrNum; attrPos++)
        switch (schema->dataTypes[attrPos])
        {
            case DT_STRING:
                offset += schema->typeLength[attrPos];
                break;
            case DT_INT:
                offset += sizeof(int);
                break;
            case DT_FLOAT:
                offset += sizeof(float);
                break;
            case DT_BOOL:
                offset += sizeof(bool);
                break;
        }
    return offset;
}
