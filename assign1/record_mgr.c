//
// Created by Saurabh Tiwari on 08/11/16.
//
#include "record_mgr.h"
#include <stdlib.h>
#include <string.h>

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
    return 0;
}

RC
openTable (RM_TableData *rel, char *name){

    return 0;
}

RC
closeTable (RM_TableData *rel){
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

    while(rid.page > 0 && rid.page <total_pages)
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
updateRecord (RM_TableData *rel, Record *record){
    return 0;
}

RC
getRecord (RM_TableData *rel, RID id, Record *record){
    return 0;
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
next (RM_ScanHandle *scan, Record *record){
    return 0;
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
