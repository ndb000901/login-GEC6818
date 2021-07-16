#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/input.h>

#define BIG 800*480*3
#define LITTLE 76*56*3

#define X1 203+80*0
#define X2 203+80*1
#define X3 203+80*2
#define X4 203+80*3
#define X5 203+80*4
#define X6 203+80*5
#define ACCOUNT_Y 43
#define PASSWD_Y 123

#define K0 0
#define K1 1
#define K2 2
#define K3 3
#define K4 4
#define K5 5
#define K6 6
#define K7 7
#define K8 8
#define K9 9
#define K_DELETE 10
#define K_OK 11

struct Point 
{
    int x;
    int y;
};
int getPoint(int *x,int *y,int *key_status)
{
   	// int x,y;
	
	
	//1、打开触摸屏驱动
	int ts_fd;
	ts_fd=open("/dev/input/event0",O_RDWR);
	if(ts_fd==-1)
	{
		perror("打开触摸屏失败\n");
		return -1;
	}
	
	//定义事件结构体
	struct input_event ts_event;
	
	while(1)
	{
		//2、读取触摸坐标
		read(ts_fd,&ts_event,sizeof(ts_event));
		
		//3、	处理坐标，判断信息 if
		if(ts_event.type==EV_ABS)//判断当前是否为触摸
		{
			if(ts_event.code==ABS_X)//判断当前触摸坐标是否是x轴
			{
				*x=ts_event.value;//输出x轴坐标
				//printf("x轴的坐标为：%d\n",x);
			}
			if(ts_event.code==ABS_Y)//判断当前触摸坐标是否是x轴
			{
				*y=ts_event.value;//输出x轴坐标
				//printf("y轴的坐标为：%d\n",y);
			}
			printf("当前的坐标为：(%d,%d)\n",*x,*y);
		}
		
		//判断是否按下   按下：1  松开0
		if(ts_event.type==EV_KEY &&ts_event.code==BTN_TOUCH && ts_event.value==0)
		{
			printf("当前的触摸值：%d\n",ts_event.value);
            printf("最后的坐标为：(%d,%d)\n",*x,*y);
			printf("you have leaving....\n");

            break;
		}

		// if(ts_event.value==1)
		// {
		// 	if(x>400)
		// 	{
		// 		printf("右边\n");
		// 	}
			
		// 	if(x<=400)
		// 	{
		// 		printf("左边\n");
		// 	}
		// }
	}
	
	//4、关闭触摸
    *key_status = 0;
	close(ts_fd);
	
    return 0;
}



int loadBigImg(char *path,int lcdbuf[])
{
    int bmpfd;
	int ret;
	int wet;
	int i;
	char bmpbuf[BIG];
    printf("%s",path);
    bmpfd=open(path,O_RDWR);
	if(bmpfd==-1)
	{
		perror("打开图片失败\n");
		return -1;
	}
    //略过头字节
	lseek(bmpfd,54,SEEK_SET);
	
	//2、读取BMP图片的像素点
	ret=read(bmpfd,bmpbuf,BIG);
	if(ret==-1)
	{
		perror("读取图片失败\n");
		return -1;
	}
	printf("读取的图片字节数为%d\n",ret);
	
	//3-4字节转换
	for(i=0;i<800*480;i++)
	{
		//       			B                  G                  R            A
		lcdbuf[i]=bmpbuf[0+i*3]<<0 | bmpbuf[1+i*3]<<8 | bmpbuf[2+i*3]<<16 | 0x00<<24;
	}

    //翻转
	int x,y;
    int temp[800*480];
	for(y=0;y<480;y++)
	{
		for(x=0;x<800;x++)
		{
			temp[800*y+x]=lcdbuf[800*(479-y)+x];
		}
	}

    for(y = 0;y < 480;y++)
    {
        for(x = 0;x < 800;x++)
        {
            lcdbuf[800*y+x] = temp[800*y+x];
        }
    }
    close(bmpfd);
    return 0;
}

int loadLittleImg(char *path,int lcdbuf[])
{
    int bmpfd;
	int ret;
	int wet;
	int i;
	char bmpbuf[LITTLE];

    bmpfd=open(path,O_RDWR);
	if(bmpfd==-1)
	{
		perror("打开图片失败\n");
		return -1;
	}
    //略过头字节
	lseek(bmpfd,54,SEEK_SET);
	
	//2、读取BMP图片的像素点
	ret=read(bmpfd,bmpbuf,LITTLE);
	if(ret==-1)
	{
		perror("读取图片失败\n");
		return -1;
	}
	printf("读取的图片字节数为%d\n",ret);
	
	//3-4字节转换
	for(i=0;i<76*56;i++)
	{
		//       			B                  G                  R            A
		lcdbuf[i]=bmpbuf[0+i*3]<<0 | bmpbuf[1+i*3]<<8 | bmpbuf[2+i*3]<<16 | 0x00<<24;
	}


        //翻转
	int x,y;
    int temp[76*56];
	for(y=0;y<56;y++)
	{
		for(x=0;x<76;x++)
		{
			temp[76*y+x]=lcdbuf[76*(55-y)+x];
		}
	}

    for(y = 0;y < 56;y++)
    {
        for(x = 0;x < 76;x++)
        {
            lcdbuf[76*y+x] = temp[76*y+x];
        }
    }
    close(bmpfd);

    return 0;
}

void changeBuf(int lcdbuf[],int data[],int type,int index)
{

    int i;
    int j;
    int x = -1;
    int y = -1;
    if(type == 0)
    {
        y = ACCOUNT_Y;
        switch (index)
        {
        case 1:
            x = X1;
            break;
        case 2:
            x = X2;
            break;
        case 3:
            x = X3;
            break;
        case 4:
            x = X4;
            break;
        case 5:
            x = X5;
            break;
        case 6:
            x = X6;
            break; 
        }
    }
    else if(type == 1)
    {
        y = PASSWD_Y;
        switch (index)
        {
        case 1:
            x = X1;
            break;
        case 2:
            x = X2;
            break;
        case 3:
            x = X3;
            break;
        case 4:
            x = X4;
            break;
        case 5:
            x = X5;
            break;
        case 6:
            x = X6;
            break; 
        }
    }

    for(i = y;i < 56 + y;i++)
    {
        for(j = x;j < 76 + x;j++)
        {
            lcdbuf[800 * i + j] = data[(i - y) * 76 + j - x];
        }
    } 
}

int updateGui(int lcdbuf[])
{
    int *lcdmmap;
    int lcdfd;

    lcdfd=open("/dev/fb0",O_RDWR);
	if(lcdfd==-1)
	{
		perror("打开lcd失败\n");
		return -1;
	}
    lcdmmap=mmap(NULL,800*480*4,PROT_EXEC|PROT_READ|PROT_WRITE,MAP_SHARED,lcdfd,0);
	if(lcdmmap==NULL)
	{
		perror("映射失败\n");
		return -1;//输出错误
	}

    // //翻转
	// int x,y;
	// for(y=0;y<480;y++)
	// {
	// 	for(x=0;x<800;x++)
	// 	{
	// 		lcdmmap[800*y+x]=lcdbuf[800*(479-y)+x];
	// 	}
	// }

    	//释放内存
    int x,y;
    for(y = 0;y < 480;y++)
    {
        for(x = 0;x < 800;x++)
        {
            lcdmmap[800*y+x] = lcdbuf[800*y+x];
        }
    }
    
	munmap(lcdmmap,800*480*4);
	
	
	//4、关闭BMP和lcd

	close(lcdfd);
    return 0;
}

int getKey(int x,int y)
{
    if(x >= 261 && x <= 579 && y >= 241 && y <= 419)
    {
        if(x >= 501 && x <= 579 && y >= 301 && y <= 359)
        {
            return K0;
        }
        else if(x >= 261 && x <= 339 && y >=  241 && y <= 299)
        {
            return K1;
        }
        else if(x >= 341 && x <= 419 && y >= 241 && y <= 299)
        {
            return K2;
        }
        else if(x >= 421 && x <= 499 && y >= 241 && y <= 299)
        {
            return K3;
        }
        else if(x >= 261 && x <= 339 && y >= 301 && y <= 359)
        {
            return K4;
        }
        else if(x >= 341 && x <= 419 && y >= 301 && y <= 359)
        {
            return K5;
        }
        else if(x >= 421 && x <= 499 && y >= 301 && y <= 359)
        {
            return K6;
        }
        else if(x >= 261 && x <= 339 && y >= 361 && y <= 419)
        {
            return K7;
        }
        else if(x >= 341 && x <= 419 && y >= 361 && y <= 419)
        {
            return K8;
        }
        else if(x >= 421 && x <= 499 && y >= 361 && y <= 419)
        {
            return K9;
        }
        else if(x >= 501 && x <= 579 && y >= 241 && y <= 299)
        {
            return K_DELETE;
        }
        else if(x >= 501 && x <= 579&& y >= 361 && y <= 419)
        {
            return K_OK;
        }
 
    }
    return -1;
}

void delete(int lcdbuf[],int type,int index)
{
    int i;
    int j;
    int x = -1;
    int y = -1;
    if(type == 0)
    {
        y = ACCOUNT_Y;
        switch (index)
        {
        case 1:
            x = X1;
            break;
        case 2:
            x = X2;
            break;
        case 3:
            x = X3;
            break;
        case 4:
            x = X4;
            break;
        case 5:
            x = X5;
            break;
        case 6:
            x = X6;
            break; 
        }
    }
    else if(type == 1)
    {
        y = PASSWD_Y;
        switch (index)
        {
        case 1:
            x = X1;
            break;
        case 2:
            x = X2;
            break;
        case 3:
            x = X3;
            break;
        case 4:
            x = X4;
            break;
        case 5:
            x = X5;
            break;
        case 6:
            x = X6;
            break; 
        }
    }

    for(i = y;i < 56 + y;i++)
    {
        for(j = x;j < 76 + x;j++)
        {
            // lcdbuf[800 * i + j] = data[(i - y) * 76 + j - x];
            lcdbuf[800 * i + j] = 0xFFFFFF;
        }
    } 
}

int check(int passwd[],int account[],int inputAccount[],int inputPasswd[])
{
    int i;
    for(i = 0;i < 6;i++)
    {
        if(account[i] != inputAccount[i])
        {
            return -1;
        }
        if(passwd[i] != inputPasswd[i])
        {
            return -1;
        }
    }
    return 0;
}

int main()
{
    int lcdbuf_main[800*480];
    int lcdbuf_dl[800*480];
    int lcdbuf_sb[800*480];
    int lcdbuf0[76*56];
    int lcdbuf1[76*56];
    int lcdbuf2[76*56];
    int lcdbuf3[76*56];
    int lcdbuf4[76*56];
    int lcdbuf5[76*56];
    int lcdbuf6[76*56];
    int lcdbuf7[76*56];
    int lcdbuf8[76*56];
    int lcdbuf9[76*56];
    int lcdbufm[76*56];
    int key_status = 0;
    int x = -1;
    int y = -1;
    int accountNum = 0;
    int passwdNum = 0;
    int account[] = {1,2,3,4,5,6};
    int passwd[] = {0,0,1,1,2,2};
    int inputAccount[6] = {-1,-1,-1,-1,-1,-1};
    int inputPasswd[6] = {-1,-1,-1,-1,-1,-1};
    int input_status = 1;
    // int key_x[] = {261,339,341,419,421,499,501,579};
    // int key_y[] = {241,299,301,359,361,419};

    loadBigImg("main.bmp",lcdbuf_main);
    loadBigImg("dl.bmp",lcdbuf_dl);
    loadBigImg("sb.bmp",lcdbuf_sb);
    loadLittleImg("0.bmp",lcdbuf0);
    loadLittleImg("1.bmp",lcdbuf1);
    loadLittleImg("2.bmp",lcdbuf2);
    loadLittleImg("3.bmp",lcdbuf3);
    loadLittleImg("4.bmp",lcdbuf4);
    loadLittleImg("5.bmp",lcdbuf5);
    loadLittleImg("6.bmp",lcdbuf6);
    loadLittleImg("7.bmp",lcdbuf7);
    loadLittleImg("8.bmp",lcdbuf8);
    loadLittleImg("9.bmp",lcdbuf9);
    loadLittleImg("m.bmp",lcdbufm);
    // changeBuf(lcdbuf_main,lcdbuf0,X1,ACCOUNT_Y);
    // changeBuf(lcdbuf_main,lcdbuf1,X2,ACCOUNT_Y);
    // changeBuf(lcdbuf_main,lcdbuf2,X3,ACCOUNT_Y);
    // changeBuf(lcdbuf_main,lcdbuf3,X4,ACCOUNT_Y);
    // changeBuf(lcdbuf_main,lcdbuf4,X5,ACCOUNT_Y);
    // changeBuf(lcdbuf_main,lcdbuf5,X6,ACCOUNT_Y);  

    // changeBuf(lcdbuf_main,lcdbuf6,X1,PASSWD_Y);
    // changeBuf(lcdbuf_main,lcdbuf7,X2,PASSWD_Y);
    // changeBuf(lcdbuf_main,lcdbuf8,X3,PASSWD_Y);
    // changeBuf(lcdbuf_main,lcdbuf9,X4,PASSWD_Y);
    // changeBuf(lcdbuf_main,lcdbufm,X5,PASSWD_Y);
    // changeBuf(lcdbuf_main,lcdbuf0,X6,PASSWD_Y);  
    updateGui(lcdbuf_main);
	

    while(1)
    {
        getPoint(&x,&y,&key_status);
        if(key_status == 0)
        {
            printf("x = %d  y = %d\n",x,y);
            switch(getKey(x,y))
            {
                case K0:
                    printf("您按下了0\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf0,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K0;
                        input_status++;
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf0,0,input_status);
                        
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K0;
                        input_status++;
                    }
                    break;
                case K1:
                    printf("您按下了1\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf1,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K1;
                        input_status++;                        
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf1,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K1;
                        input_status++;
                    }
                    break;    
                case K2:
                    printf("您按下了2\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf2,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K2;
                        input_status++;                        
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf2,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K2;
                        input_status++;
                    }
                    break;
                case K3:
                    printf("您按下了3\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf3,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K3;
                        input_status++;                        
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf3,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K3;
                        input_status++;
                    }
                    break;
                case K4:
                    printf("您按下了4\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf4,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K4;
                        input_status++;                        
                    }  
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf4,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K4;
                        input_status++;
                    }                  
                    break;
                case K5:
                    printf("您按下了5\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf5,1,input_status - 6);                        
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K5;
                        input_status++;                        
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf5,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K5;
                        input_status++;
                    }                   
                    break;
                case K6:
                    printf("您按下了6\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf6,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K6;
                        input_status++;                        
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf6,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K6;
                        input_status++;
                    }
                    break;    
                case K7:
                    printf("您按下了7\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf7,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K7;
                        input_status++;                        
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf7,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K7;
                        input_status++;
                    }
                    break;
                case K8:
                    printf("您按下了8\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf8,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K8;
                        input_status++;                        
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf8,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K8;
                        input_status++;
                    }
                    break;
                case K9:
                    printf("您按下了9\n");
                    if(input_status > 12)
                    {
                        break;
                        print("输入已满\n");
                    }

                    if(input_status > 6 && input_status <= 12)
                    {
                        changeBuf(lcdbuf_main,lcdbuf9,1,input_status - 6);
                        updateGui(lcdbuf_main);
                        inputPasswd[input_status - 7] = K9;
                        input_status++;                        
                    }
                    if(input_status > 0 && input_status <= 6)
                    {
                        changeBuf(lcdbuf_main,lcdbuf9,0,input_status);
                        updateGui(lcdbuf_main);
                        inputAccount[input_status - 1] = K9;
                        input_status++;
                    }
                    break;
                case K_DELETE:
                    printf("您按下了Delete\n");
                    if(input_status > 1)
                    {
                        if(input_status > 1 && input_status <= 7)
                        {
                            
                            delete(lcdbuf_main,0,input_status - 1);
                            updateGui(lcdbuf_main);
                            input_status--;
                            printf("input_status %d\n",input_status);
                        }
                        if(input_status > 7 && input_status <= 13)
                        {
                            
                            delete(lcdbuf_main,1,input_status - 7);
                            updateGui(lcdbuf_main);
                            input_status--;   
                            printf("input_status %d\n",input_status);                     
                        }

                    }
                    break;
                case K_OK:
                    printf("您按下了OK\n");
                    if(input_status != 13)
                    {
                        break;
                    }
                    if(check(passwd,account,inputAccount,inputPasswd) == 0)
                    {
                        printf("登录成功！！\n");
                        updateGui(lcdbuf_dl);
                        sleep(5);
                        updateGui(lcdbuf_main);

                    }
                    else
                    {
                        printf("登录失败！！\n");
                        updateGui(lcdbuf_sb);
                        sleep(5);
                        updateGui(lcdbuf_main);
                    }
                    break;
                default:
                    break;
            }
        }
        
    }

	return 0;
}