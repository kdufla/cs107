using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "imdb.h"
#include <stdio.h>
#include <string.h>

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

struct stru{
	const void* object;
	const void* data;
};

int compAct(const void * a, const void * b){
	stru * st = (stru *)a;
	char* base=(char*)st->data;
	int bInt=*(int*)b;
	char* bChar=base+bInt;
	return strcmp((char*)st->object,bChar);
}

int compMov(const void * a, const void * b){
	stru* st=(stru*)a;
	char* base=(char*)st->data;
	int bInt=*(int*)b;
	char* bChar=base+bInt;
	int bYear=*(char*)(bChar+strlen(bChar)+1);
	film f1=*(film*)st->object;
	film f2;
	f2.title=bChar;
	f2.year=bYear+1900;
	if(f1==f2)return 0;
	if(f1<f2)return -1;
	return 1;
}

bool imdb::getCredits(const string& player, vector<film>& films) const {
	int actorNum = *(int *)actorFile;
	stru newstruct;
	newstruct.object = player.c_str();
	newstruct.data = actorFile;
	void* search=bsearch(&newstruct,(char*)actorFile+sizeof(int),actorNum,sizeof(int),compAct);
	if(search==NULL)return false;
	int actorPointer=*(int*)search;
	char* actorName=(char*)actorFile+actorPointer;
	int nameLen=strlen(actorName);
	if(nameLen%2==0)nameLen++;
	nameLen++;
	short movieNum=*(short*)(actorName+nameLen);
	if((nameLen+sizeof(short))%4!=0)nameLen+=2;
	for(short i=0;i<movieNum;i++){
		int adrs=*(int*)(actorName+nameLen+sizeof(short)+i*sizeof(int));
		char* movieName=(char*)movieFile+adrs;
		int movieLen=strlen(movieName);
		movieLen++;
		int movieYear=*(char*)(movieName+movieLen);
		film fil;
		fil.title=movieName;
		fil.year=movieYear+1900;
		films.push_back(fil);
	}
 return true; }

bool imdb::getCast(const film& movie, vector<string>& players) const {
	int movieNum = *(int*)movieFile;
	stru newstruct;

	newstruct.object = &movie;
	newstruct.data = movieFile;
	void* search=bsearch(&newstruct,(char*)movieFile+sizeof(int),movieNum,sizeof(int),compMov);
	if(search==NULL)return false;
	int moviePointer=*(int*)search;
	char* movieName=(char*)movieFile+moviePointer;
	int nameLen=strlen(movieName)+2; // + \0 and year
	if(nameLen%2!=0)nameLen++;
	short actorNum=*(short*)(movieName+nameLen);
	if((nameLen+sizeof(short))%4!=0)nameLen+=2;
	for (short i = 0; i < actorNum; ++i){
		int adrs=*(int*)(movieName+nameLen+sizeof(short)+i*sizeof(int));
		char* actorName=(char*)actorFile+adrs;
		players.push_back(actorName);
	}
 return true; }

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
