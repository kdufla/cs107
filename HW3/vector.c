#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
	assert(elemSize>0);
	v->elemSize = elemSize;
	v->logLen = 0;
	int len=4;
	if(initialAllocation>0)len=initialAllocation;
	v->allocLen = len;
	v->freeFn = freeFn;
	v->elems = malloc(v->allocLen * v->elemSize);
	assert(v->elems != NULL);
}

void VectorDispose(vector *v){
	if (v->freeFn != NULL) {
		for (int i = 0; i<v->logLen; i++) {
			v->freeFn((char *)v->elems + i*v->elemSize);
		}
	}
	free(v->elems);
}

int VectorLength(const vector *v)
{ return v->logLen; }

void *VectorNth(const vector *v, int position){
	assert(position>=0);
	assert(position<v->logLen);
	return (void*)((char*)v->elems+v->elemSize*position);
 }

void VectorReplace(vector *v, const void *elemAddr, int position){
	assert(position>=0);
	assert(position<v->logLen);
	if(v->freeFn!=NULL)v->freeFn((char*)v->elems+v->elemSize*position);
	memcpy((char*)v->elems+v->elemSize*position,elemAddr,v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
	assert(position>=0);
	assert(position<=v->logLen);
	if(v->logLen==v->allocLen){
		v->allocLen*=2;
		v->elems=realloc(v->elems,v->allocLen*v->elemSize);
		assert(v->elems!=NULL);
	}
	memmove((char*)v->elems+v->elemSize*(position+1),(char*)v->elems+v->elemSize*position,(v->logLen-position)*v->elemSize);
	//VectorReplace(v,elemAddr,position);
	memcpy((char*)v->elems+v->elemSize*position,elemAddr,v->elemSize);
	v->logLen++;
}

void VectorAppend(vector *v, const void *elemAddr){
	VectorInsert(v,elemAddr,v->logLen);
}

void VectorDelete(vector *v, int position){
	assert(position>=0);
	assert(position<v->logLen);
	if(v->freeFn!=NULL)v->freeFn((char*)v->elems+v->elemSize*position);
	memmove((char*)v->elems+v->elemSize*position,(char*)v->elems+v->elemSize*(position+1),(v->logLen-position-1)*v->elemSize);
	v->logLen--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
	assert(compare!=NULL);
	qsort(v->elems,v->logLen,v->elemSize,compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
	assert(mapFn!=NULL);
	for (int i = 0; i < v->logLen; ++i){
		mapFn((char*)v->elems+v->elemSize*i,auxData);
	}
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){
	assert(searchFn!=NULL&&key!=NULL);
	assert(startIndex<=v->logLen);
	assert(startIndex>=0);
	void* value;
	if(isSorted){
		value=bsearch(key, (char*)v->elems+v->elemSize*startIndex,v->logLen-startIndex,v->elemSize,searchFn);
	}else{
		size_t bla=v->logLen-startIndex;
		value=lfind(key, (char*)v->elems+v->elemSize*startIndex,&bla,v->elemSize,searchFn);
	}
	if(value==NULL)return -1;
	else return ((char*)value-(char*)v->elems)/v->elemSize;
  } 
