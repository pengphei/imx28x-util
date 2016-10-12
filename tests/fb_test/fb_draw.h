#ifndef _FB_DRAW_H
#define _FB_DRAW_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

struct fb_lcd
{
   int fb_fd;
   void *fb_mem;
   unsigned int fb_xres;
   unsigned int fb_yres;
   unsigned int fb_line_len;
   unsigned int fb_size;
   unsigned int fb_bpp;
};

void fb_draw_point(void *memp, 
	  			   unsigned int xres, unsigned int yres, 
				   unsigned int x, unsigned int y, 
				   unsigned int color)
{
   *((unsigned short int *)memp+xres*y+x)=color;
}


int fb_open(char *pathname, struct fb_lcd *fb_info)
{
   struct fb_var_screeninfo vinfo;
   struct fb_fix_screeninfo finfo;

   fb_info->fb_fd = open(pathname,O_RDWR);

   if(!fb_info->fb_fd)
   {
	  printf("Error:cannot open /dev/fb0.\n");
	  return -1;
   }
		 

	if(ioctl(fb_info->fb_fd, FBIOGET_FSCREENINFO, &finfo))
	{
	   printf("Error:reading fixed info.\n");
	   return -1;
	}
	if(ioctl(fb_info->fb_fd, FBIOGET_VSCREENINFO, &vinfo))
	{
	   printf("Error reading var info.\n");
	   return -1;
	}

	fb_info->fb_xres = vinfo.xres;
	fb_info->fb_yres = vinfo.yres;
	fb_info->fb_line_len = finfo.line_length;
	fb_info->fb_bpp = vinfo.bits_per_pixel;

	fb_info->fb_size = finfo.smem_len;
	//映射
	fb_info->fb_mem = mmap(0, fb_info->fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_info->fb_fd, 0);

	if((int)fb_info->fb_mem == -1)
	{
	   printf("Error: failed to map framebuffer device to memory.\n");
	   return -1;
	}
	return 0;
}


int fb_close(struct fb_lcd *fb_info)
{
   if(0!=munmap(fb_info->fb_mem,  fb_info->fb_size))
   {
	  printf("Error:munmap was fault.\n");
	  return -1;
   }
   if(0!=close(fb_info->fb_fd))
   {
	  printf("Error:close file was fault.\n");
	  return -1;
   }

   return 0;
}

#endif
