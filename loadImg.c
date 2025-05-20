#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int loadImg(char *filename, float **verts, float **vCols, int *vertCnt)
{
   int width, height, channels;
   int i, j, x, y, r, g, b, a;
   float xCenter, yCenter;
   unsigned char *img_data = stbi_load(filename, &width, &height, &channels, 0);

   *vertCnt = 0;
   *vCols = 0;
   *verts = 0;
   
   if (img_data == NULL)
   {
      printf("Error loading image\n");
      return 1;
   }

   //printf("Loaded image: %dx%d with %d channels\n", width, height, channels);

   *vCols = malloc(width * height * sizeof(float) * 3);
   *verts = malloc(width * height * sizeof(float) * 3);
   
   xCenter = width / 2;
   yCenter = height / 2;
   i=0; j=0;
   if (channels == 3)
   for (y=height-1; y >= 0; y--)
   for (x=0; x < width; x++)
   {
      r = img_data[j+0];
      g = img_data[j+1];
      b = img_data[j+2];
      
      (*verts)[j+0] = (float)x - xCenter;
      (*verts)[j+1] = (float)y - yCenter;
      (*verts)[j+2] = 0;
   
      (*vCols)[j+0] = (float)r / 255;
      (*vCols)[j+1] = (float)g / 255;
      (*vCols)[j+2] = (float)b / 255;
      j += 3;
   }
   
   if (channels == 4)
   for (y=height-1; y >= 0; y--)
   for (x=0; x < width; x++)
   {
      r = img_data[i+0];
      g = img_data[i+1];
      b = img_data[i+2];
      a = img_data[i+3];
      i += 4;
      
      if (a)
      {
         (*verts)[j+0] = (float)x - xCenter;
         (*verts)[j+1] = (float)y - yCenter;
         (*verts)[j+2] = 0;
         
         (*vCols)[j+0] = (float)r / 255;
         (*vCols)[j+1] = (float)g / 255;
         (*vCols)[j+2] = (float)b / 255;
         j += 3;
      }
   }
   
   // free excess memory
   if (j)
   {
      *vCols = realloc(*vCols, j * sizeof(float));
      *verts = realloc(*verts, j * sizeof(float));
   }
   
   *vertCnt = j / 3;
   
   stbi_image_free(img_data);
   return 0;
}

void freeImg(float *verts, float *vCols)
{
   if (verts)
   free(verts);
   
   if (vCols)
   free(vCols);
}
