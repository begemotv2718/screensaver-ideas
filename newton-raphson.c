#include <stdio.h>
#include <math.h>
#include <complex.h>

#define NPOINTS 3

int picture[1024*1024];
complex double iterate(complex double z){
  complex double res_z;
  res_z=z-(z*z*z-1.0)/(2*z*z);
  return res_z;
}

double distance(complex double z1, complex double z2){
  complex double diff=z1-z2;
  return sqrt(diff*conj(diff));
}

double mindistance(complex double z, complex double *points, int npoints){
  int i;
  double min,tmp;
  if(0==npoints) return 0.0;
  min = distance(z,points[0]);
  for(i=1;i<npoints;i++){
    tmp=distance(z,points[i]);
    if(tmp<min){
      min=tmp;
    }
  }
  return min;
}

void drawbmp (char * filename, int *picture, int width, int height) {

unsigned int headers[13];
FILE * outfile;
int extrabytes;
int paddedsize;
int x; int y; int n;
int red, green, blue;

extrabytes = 4 - ((width * 3) % 4);                 // How many bytes of padding to add to each
                                                    // horizontal line - the size of which must
                                                    // be a multiple of 4 bytes.
if (extrabytes == 4)
   extrabytes = 0;

paddedsize = ((width * 3) + extrabytes) * height;

// Headers...
// Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers".

headers[0]  = paddedsize + 54;      // bfSize (whole file size)
headers[1]  = 0;                    // bfReserved (both)
headers[2]  = 54;                   // bfOffbits
headers[3]  = 40;                   // biSize
headers[4]  = width;  // biWidth
headers[5]  = height; // biHeight

// Would have biPlanes and biBitCount in position 6, but they're shorts.
// It's easier to write them out separately (see below) than pretend
// they're a single int, especially with endian issues...

headers[7]  = 0;                    // biCompression
headers[8]  = paddedsize;           // biSizeImage
headers[9]  = 0;                    // biXPelsPerMeter
headers[10] = 0;                    // biYPelsPerMeter
headers[11] = 0;                    // biClrUsed
headers[12] = 0;                    // biClrImportant

outfile = fopen(filename, "wb");

//
// Headers begin...
// When printing ints and shorts, we write out 1 character at a time to avoid endian issues.
//

fprintf(outfile, "BM");

for (n = 0; n <= 5; n++)
{
   fprintf(outfile, "%c", headers[n] & 0x000000FF);
   fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
   fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
   fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
}

// These next 4 characters are for the biPlanes and biBitCount fields.

fprintf(outfile, "%c", 1);
fprintf(outfile, "%c", 0);
fprintf(outfile, "%c", 24);
fprintf(outfile, "%c", 0);

for (n = 7; n <= 12; n++)
{
   fprintf(outfile, "%c", headers[n] & 0x000000FF);
   fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
   fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
   fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
}

//
// Headers done, now write the data...
//

for (y = height - 1; y >= 0; y--)     // BMP image format is written from bottom to top...
{
   for (x = 0; x <= width - 1; x++)
   {

      red = picture[x+width*y]*5;
      green = picture[x+width*y]*5;
      blue = picture[x+width*y]*5;

      if (red > 255) red = 255; if (red < 0) red = 0;
      if (green > 255) green = 255; if (green < 0) green = 0;
      if (blue > 255) blue = 255; if (blue < 0) blue = 0;

      // Also, it's written in (b,g,r) format...

      fprintf(outfile, "%c", blue);
      fprintf(outfile, "%c", green);
      fprintf(outfile, "%c", red);
   }
   if (extrabytes)      // See above - BMP lines must be of lengths divisible by 4.
   {
      for (n = 1; n <= extrabytes; n++)
      {
         fprintf(outfile, "%c", 0);
      }
   }
}

fclose(outfile);
return;
}




int main(void){
    complex double z;
    int x,y;
    complex double roots[NPOINTS];
    double step=2.0/1024.0;
    int niterations;

    roots[0]=1.0+0.0*I;
    roots[1]=-0.5+I*sqrt(3)/2.0;
    roots[2]=-0.5-I*sqrt(3)/2.0;


    for(y=0;y<1024;y++){
      for(x=0;x<1024;x++){
        z=(x*step-1.0)+(y*step-1.0)*I;
        niterations=0;
        while((mindistance(z,roots,NPOINTS)>1e-7)&&(niterations<1000)){
          z=iterate(z);
          niterations++;
        }
        //printf("x=%d y=%d niterations=%d\n",x,y,niterations);
        picture[x+1024*y]=niterations;
      }
    }
    drawbmp("/tmp/test.bmp",picture,1024,1024);

}


        
