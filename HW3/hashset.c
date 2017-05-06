#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
	
	
	assert(numBuckets>0);
	assert(elemSize>0);
	assert(comparefn!=NULL&&hashfn!=NULL);
	h->elemSize=elemSize;
	h->numBuckets=numBuckets;
	h->numElems=0;
	h->hashfn=hashfn;
	h->comparefn=comparefn;
	h->freefn=freefn;
	h->elems=malloc(numBuckets*sizeof(vector));
	assert(h->elems!=NULL);
	for (int i = 0; i < numBuckets; ++i){
		VectorNew((vector*)h->elems+i,h->elemSize ,h->freefn ,0);
	}
	
}

void HashSetDispose(hashset *h){
	for (int i = 0; i < h->numBuckets; ++i){
		VectorDispose((char*)h->elems+i*sizeof(vector));
	}
	free(h->elems);
}

int HashSetCount(const hashset *h)
{ return h->numElems; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn!=NULL);
	for (int i = 0; i < h->numBuckets; ++i){
		VectorMap((vector*)h->elems+i,mapfn,auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr){
	assert(elemAddr!=NULL);
	int adr=h->hashfn(elemAddr,h->numBuckets);
	assert(adr>=0&&adr<h->numBuckets);
	int pos=VectorSearch((vector*)h->elems+adr, elemAddr, h->comparefn, 0,0);
	 // printf("%d\n",pos);

	if(pos==-1){
		VectorAppend((vector*)h->elems+adr,elemAddr);
		h->numElems++;
	}else VectorReplace((vector*)h->elems+adr,elemAddr,pos);
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	assert(elemAddr!=NULL);
	int adr=h->hashfn(elemAddr,h->numBuckets);
	assert(adr>=0&&adr<h->numBuckets);
	int pos=VectorSearch((vector*)h->elems+adr, elemAddr, h->comparefn, 0,0);
	if(pos==-1)return NULL;
	else return VectorNth((vector*)h->elems+adr,pos);
 }
