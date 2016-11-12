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
    return 0;
}

RC
deleteTable (char *name){
    return 0;
}

int
getNumTuples (RM_TableData *rel){
    return 0;
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
    return 0;
}

RC
next (RM_ScanHandle *scan, Record *record){
    return 0;
}

RC
closeScan (RM_ScanHandle *scan){
    return 0;
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
    char *data = (*record)->data;
    data = (char *)malloc(getRecordSize(schema));
    (*record)->data = data;


}

RC freeRecord (Record *record)
{
    free(record);
    return RC_OK;
}

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
    return 0;
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
        }
        case DT_FLOAT:
        {
            memcpy(attributeData, &(value->v.floatV), schema->typeLength[attrNum]);
        }
        case DT_INT:
        {
            memcpy(attributeData, &(value->v.intV), schema->typeLength[attrNum]);
        }
        case DT_STRING:
        {
            memcpy(attributeData, value->v.stringV, schema->typeLength[attrNum]*sizeof(char));
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
