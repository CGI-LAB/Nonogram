#include "Parsers.h"

void parseTaai(char* path, Puzzle* quests){
	FILE* fp = fopen(path,"r");
	char myBuf[P_SIZE+30];
	char puzBuf[P_SIZE*P_SIZE+30];
	char* c = puzBuf;
	for(int i = 0; i < NUM_OF_QUESTIONS; i++){
		fgets(myBuf , P_SIZE + 3 , fp);
		for(int j = 0; j < P_SIZE*2; j++){
			fgets(myBuf , P_SIZE + 3 , fp);
            if( strlen(myBuf)==0 )
            {
				*c = 'a' -1;
				c++;
			}
            else
            {
			    char* token;
			    for(token = strtok(myBuf," \t"); token; token = strtok(NULL," \t"))
                {
				    *c = (char)atoi(token) + 'a' -1;
				    c++;
			    }
            }
			*c = 'z';
			c++;
		}
		*c = 0;
		readBuffer( quests[i], puzBuf);
		c = puzBuf;
	}
	c = NULL;
	fclose(fp);
}

void parseOne(char* path, Puzzle& quest){
	FILE* fp = fopen(path,"r");
	char myBuf[P_SIZE+3];
	char puzBuf[P_SIZE*P_SIZE];
	char* c = puzBuf;
	for(int j = 0; j < P_SIZE*2; j++){
			fgets(myBuf , P_SIZE + 3 , fp);
			char* token;
			for(token = strtok(myBuf," \t"); token; token = strtok(NULL," \t")){
				*c = (char)atoi(token) + 'a' -1;
				c++;
			}
			*c = 'z';
			c++;
		}
	*c = 0;
	readBuffer( quest, puzBuf);
	c = puzBuf;
}