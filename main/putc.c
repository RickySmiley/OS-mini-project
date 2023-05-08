#define MAX_ROWS 25
#define MAX_COLS 80


unsigned short *video = (unsigned short*)0xB8000;
int color = 7;
static int cursorX=1;
static int cursorY=1;



void putc(unsigned char c) {
    unsigned short *where;
    unsigned setColor = color <<8; 

   
    if (c == '\n'){
        cursorX = 0;
        cursorY++;
    } else if (c >= ' '){
        where = video + (cursorY * MAX_COLS + cursorX);
        *where = c | setColor;
        cursorX++;
    }

    if (cursorX >= MAX_COLS){
        cursorX= 0;
        cursorY++;
    }
    unsigned temp = cursorY -25 +1;
    if(cursorY >= MAX_ROWS) {
	    for (int i=1;i<MAX_ROWS;i++){
		    for(int j=0; j<MAX_COLS;j++){
			    video[j+((i*160)-160)] = video[j+((i*160))];


	    //memoryCopy(video,video+temp * 80,(25-temp)*80*2);
	    }
	   }
	    for(int i =0; i<MAX_COLS; i++) {
		    video[i+((MAX_ROWS-1)*160)] = 0;

	    
    }
	    cursorX = 0;
	    cursorY = MAX_ROWS-2;
    }
}



void memoryCopy(char *source, char *dest, int nbytes) {
     int i;
     for (i = 0; i < nbytes; i++) {
         *(dest + i) = *(source + i);
     }
 }






