#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <linux/input.h>   //输入事件头文件

int ts_open()
{

	
}


int LCD(char buf_pic[800*480*4])
{
	int lcd_fd = open("/dev/fb0",O_RDWR);  //打开LCD文件
	if(lcd_fd < 0)
		{
			perror("lcd open ...");
			return -1;
		}

	unsigned char *lcd=mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0); //使用虚拟内存

	int i;
	for(i=0;i<800*480*4;i++)
		*(lcd+i) = buf_pic[i];



		//write(lcd_fd,buf_pic,800*480*4); 






		close(lcd_fd);
}


char picture_read(char buf_fz[480*800*4],char name[50])
{
	int bmp_fd = open(name,O_RDWR);  //打开图片文件
	if(bmp_fd < 0)
	{
		perror("bmp open ...");
		return -1;
	}
	
	char head[54];
	read(bmp_fd,head,54);  //读取头文件数据

	char buf24[480*800*3];

	char buf32[480*800*4];

	read(bmp_fd,buf24,480*800*3);  //读取像素数据


	int i;                                          //24位像素转32位像素
	for(i=0;i<480*800;i++)
	{
		buf32[0+4*i] = buf24[0+3*i];
		buf32[1+4*i] = buf24[1+3*i];
		buf32[2+4*i] = buf24[2+3*i];
		buf32[3+4*i] = 0;

	}

	int m,n;
	

	for(n=0;n<480;n++)
	{
		for(m=0;m<800*4;m++)
		{
			buf_fz[(479-n)*800*4+m] = buf32[(0+n)*800*4+m];    //翻转图片
		}


	}
	close(bmp_fd);
	return 0;
}




int main()
{	
	char buf_pic1[480*800*4];
	char buf_pic2[480*800*4];
	char buf_pic3[480*800*4];
	char buf_pic4[480*800*4];
	char *array[4];
	array[0] = buf_pic1;
	array[1] = buf_pic2;
	array[2] = buf_pic3;
	array[3] = buf_pic4;
	char root1[] = "bmp1.bmp";
	char root2[] = "bmp2.bmp";
	char root3[] = "bmp3.bmp";
	char root4[] = "bmp4.bmp";
	picture_read( buf_pic1,root1);
	picture_read( buf_pic2,root2);
	picture_read( buf_pic3,root3);
	picture_read( buf_pic4,root4);



	LCD(array[0]);
	ts_open();
	struct input_event   ts;
	int i = 0;
	int ts_fd = open("/dev/input/event0",O_RDWR);
	if(ts_fd < 0)
	{
		perror("open fail");
		return -1 ;
	}


	while(1)
	{	

		read(ts_fd,&ts,sizeof(ts));   //处理读到的事件值 
		
		//处理X 轴事件 
		if(ts.type == EV_ABS && ts.code == ABS_X && (ts.value <= 400)&&(ts.value > 0 ))
		{
			if( i == 0)
			{
				LCD(array[3]);
				i = 3;
			}       
			else
			{
				i--;
				LCD(array[i]);
			}                        
		}
		if(ts.type == EV_ABS && ts.code == ABS_X && (ts.value <= 800)&&(ts.value > 400 ))
		{
			if(i == 3 )
			{
				LCD(array[0]);
				i = 0;
			}
				
			else
			{
				i++;
				LCD(array[i]);
			}
		}	

	

	}

	


	return 0;
}