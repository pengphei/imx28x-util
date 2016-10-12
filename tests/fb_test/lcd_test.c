#include "fb_draw.h"
int col[] = {0xffffffff,0x00000000,~0x1f,0x0000f800,0x7e0,0x1f};
int main()
{
   struct fb_lcd fb_info;
   int k=0, i=0, j=0;
   int color=0;
   char	inputc;
   int fd = -1;
   fd = fb_open("/dev/fb0",&fb_info);
   if (fd == -1) {
	printf("can not ope /dev/fb0\r\n");
	exit(0);
   }
#if 1
	printf("fb0 opened\r\n");

	for(k = 0; k<1600; k++)
	{
	   printf("%d:col[%d] = 0x%x",k,k%5,col[k%5]);

	   color = 1<<k;
	   
	   for(i=0; i<480; i++) {
		 // for(j=0; j<272; j++) {
		fb_draw_point(fb_info.fb_mem,fb_info.fb_xres,fb_info.fb_yres,i,0,col[k%5]);
			 

		fb_draw_point(fb_info.fb_mem,fb_info.fb_xres,fb_info.fb_yres,i,271,col[k%5]);
	  }
	  for(j=0; j<272; j++) {
		fb_draw_point(fb_info.fb_mem,fb_info.fb_xres,fb_info.fb_yres,0,j,col[k%5]);
	  
		fb_draw_point(fb_info.fb_mem,fb_info.fb_xres,fb_info.fb_yres,479,j,col[k%5]);
	  }

	for(i=100; i<150; i++) {
                 for(j=100; j<150; j++) {
                     fb_draw_point(fb_info.fb_mem,fb_info.fb_xres,fb_info.fb_yres,i,j,col[k%5]);
		}
	}
 		   
	   inputc = getchar();
 	   if (inputc == 'Q') break;
	}

#endif

	fb_close(&fb_info);
	printf("lcd have draw .\n");	
	return 0;


}


