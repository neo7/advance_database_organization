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

    total_pages = atoi(readPointer);
    BM_BufferPool *bm_bufferPool = ((BM_BufferPool *) malloc (sizeof(BM_BufferPool)));
    tableMgmt->bm_bufferPool = bm_bufferPool;
    BM_PageHandle *bm_pageHandle = ((BM_PageHandle *) malloc (sizeof(BM_PageHandle)));
    initBufferPool(bm_bufferPool, name, 6, RS_FIFO, NULL);
    pinPage(bm_bufferPool, bm_pageHandle, 0);
    tableMgmt->pageNum = total_pages;
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
    return 0;
}

RC
deleteRecord (RM_TableData *rel, RID id){
    return 0;
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
    for (i = 0; i < sizeof(ph->data); i++)
        memcpy(&ph->data[i], NULL, sizeof(ph->data));

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

        Record *converted_record = temp_data_record;

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
    //note: recheck string assignment on errors.
    int offset = getRecordSizeOffset(schema, attrNum);
    char *attributeData = record->data;
    attributeData = attributeData + offset;
    switch (schema->dataTypes[attrNum])
    {
        case DT_BOOL:
        {
            memcpy(attributeData, &(value->v.boolV), schema->typeLength[attrNum]);
            break;
        }
        case DT_FLOAT:
        {
            memcpy(attributeData, &(value->v.floatV), schema->typeLength[attrNum]);
            break;
        }
        case DT_INT:
        {
            memcpy(attributeData, &(value->v.intV), schema->typeLength[attrNum]);
            break;
        }
        case DT_STRING:
        {
            memcpy(attributeData, value->v.stringV, schema->typeLength[attrNum]);
            break;
        }
        default:
        {
            return RC_RM_UNKOWN_DATATYPE;
        }
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
