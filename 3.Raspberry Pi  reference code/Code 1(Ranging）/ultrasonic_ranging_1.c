/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         avoid_ultrasonic.c
* @author       Danny
* @version      V1.0
* @date         2017.08.16
* @brief        超声波避障实验
* @details
* @par History  见如下说明
*
*/
#include <wiringPi.h>
#include <softPwm.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

//定义引脚


int SingPin = 29;             //定义回声脚为连接Raspberry的wiringPi编码30口


/**
* Function       Distance
* @author        Danny
* @date          2017.08.16
* @brief         超声波测一次前方的距离
* @param[in]     void
* @param[out]    void
* @retval        float:distance返回距离值
* @par History   无
*/
float Distance()
{
	float distance;
	struct timeval tv1;
	struct timeval tv2;
	struct timeval tv3;
	struct timeval tv4;
	long start, stop;
	
	pinMode(SingPin, OUTPUT);
	digitalWrite(SingPin, LOW);
	delayMicroseconds(2);
	digitalWrite(SingPin, HIGH);      //向引脚输入至少10US的高电平
	delayMicroseconds(15);
	digitalWrite(SingPin, LOW);
    
	//pinMode(SingPin, INPUT);
	
	//防止程序未检测到电平变化，陷入死循环，加入一个超时重测机制
    gettimeofday(&tv3, NULL);        //超时重测机制开始计时
	start = tv3.tv_sec * 1000000 + tv3.tv_usec;
	pinMode(SingPin, INPUT);
	while(!digitalRead(SingPin) == 1)
	{
		gettimeofday(&tv4, NULL);    //超时重测机制结束计时
		stop = tv4.tv_sec * 1000000 + tv4.tv_usec;
		
		if ((stop - start) > 30000)  //最大测5米时的时间值：10/340=0.03s
		{
			return -1;               //超时返回-1
		}
	} 
	
	//防止程序未检测到电平变化，陷入死循环，加入一个超时重测机制
	gettimeofday(&tv1, NULL);      //当echo脚电平变高时开始计时
    start = tv1.tv_sec*1000000+tv1.tv_usec;
	pinMode(SingPin, INPUT);//添加
	while(!digitalRead(SingPin) == 0)
	{
		gettimeofday(&tv3,NULL);   //超时重测机制开始计时
		stop = tv3.tv_sec*1000000+tv3.tv_usec;
		if ((stop - start) > 30000)
		{
			return -1;
		}
	}                              //超时重测机制结束计时
	gettimeofday(&tv2, NULL);      //当echo脚电平变低时结束计时

	start = tv1.tv_sec * 1000000 + tv1.tv_usec;
	stop = tv2.tv_sec * 1000000 + tv2.tv_usec;
	distance = (float)(stop - start)/1000000 * 34000 / 2;
	return distance;
}

/**
* Function       bubble
* @author        Danny
* @date          2017.08.16
* @brief         超声波测五次的数据进行冒泡排序
* @param[in1]    a:超声波数组首地址
* @param[in2]    n:超声波数组大小
* @param[out]    void
* @retval        void
* @par History   无
*/
void bubble(unsigned long *a, int n)

{
  int i, j, temp;
  for (i = 0; i < n - 1; i++)
  {
    for (j = i + 1; j < n; j++)
    {
      if (a[i] > a[j])
      {
        temp = a[i];
        a[i] = a[j];
        a[j] = temp;
      }
    }
  }
}

/**
* Function       Distane_test
* @author        Danny
* @date          2017.08.16
* @brief         超声波测五次，去掉最大值,最小值,
*                取平均值,提高测试准确性
* @param[in]     void
* @param[out]    void
* @retval        float:distance返回距离值
* @par History   无
*/
float Distance_test()
{
  float distance;
  unsigned long ultrasonic[5] = {0};
  int num = 0;
  while (num < 5)
  {
     distance = Distance();
	 //超时返回-1，重新测试
	 while((int)distance == -1)
	 {
		 distance = Distance();
	 }
    //过滤掉测试距离中出现的错误数据大于500
    while ( (int)distance >= 500 || (int)distance == 0)
    {
         distance = Distance();
    }
    ultrasonic[num] = distance;
    num++;
	delay(10);
  }
  num = 0;
  bubble(ultrasonic, 5);
  distance = (ultrasonic[1] + ultrasonic[2] + ultrasonic[3]) / 3;
  
  printf("distance:%f\n",distance);      //打印测试的距离
  return distance;
}

/**
* Function       main
* @author        Danny
* @date          2017.08.16
* @brief         先调用按键扫描函数，
*                超声波避障模式开启
* @param[in]     void
* @retval        void
* @par History   无
*/
void main()
{
  float distance;
  
  //wiringPi初始化
  wiringPiSetup();
  
  //初始化超声波引脚
  pinMode(SingPin, INPUT);    //定义超声波输入脚

  //调用按键扫描函数
  //key_scan();

  while(1)
  {
     Distance_test();
 }
}

