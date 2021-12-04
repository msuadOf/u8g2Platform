# u8g2库在clion开发环境的移植
origin from[http://msuad.simi.work/u8g2%e5%ba%93%e5%9c%a8clion%e5%bc%80%e5%8f%91%e7%8e%af%e5%a2%83%e7%9a%84%e7%a7%bb%e6%a4%8d/](http://msuad.simi.work/u8g2%e5%ba%93%e5%9c%a8clion%e5%bc%80%e5%8f%91%e7%8e%af%e5%a2%83%e7%9a%84%e7%a7%bb%e6%a4%8d/)
​		——写于2021.12.4

---

## 前言

本文最后的目标是做一个u8g2的开发平台出来，方便我个人拿来就用

已开源：https://github.com/msuadOf/u8g2Platform

- OLEDscreen、main为自己编写，其他为CubeMX生成

平台：STM32F401CCU6+CubeMX+Clion

声明：本人所有成果仅用于学习交流，若有任何人用于商业引起的纠纷，本人概不负责

## 正常创建项目

### 配置引脚

​	为了保证移植性，可以统一命名

![5](https://view.moezx.cc/images/2021/12/04/942d53dd460bb89ccdb8f99cf6d58eba.png)

![6](https://view.moezx.cc/images/2021/12/04/99d0a4b85225f957555274b67d441307.png)

## 导入u8g2库（库移植）

把源文件打包了扔进一个文件夹，然后放进项目目录

![7](https://view.moezx.cc/images/2021/12/04/a21a725be123e170bd4fbfc97371e0f0.png)

我们在文件中搜索“**include_directories**”可以看到如下语句：

（其实我是背的，第48行和52行，当中隔了2行）

```
include_directories(${includes})
```

我们在这句话的下方加一句：

```
include_directories(u8g2-master)
```

然后将下方的这个语句做一点小小的更改：

```
#更改前：
file(GLOB_RECURSE SOURCES ${sources})

#更改后：
file(GLOB_RECURSE SOURCES ${sources} "u8g2-master/*.*")
```

完成后右击(.ioc）文件，选择“使用CubeMX更新CMake项目”：

![1619958919004](https://zhewana.cn/images/%E5%88%A9%E7%94%A8Clion%E7%9A%84STM32%E5%BC%80%E5%8F%91/1619958919004-1620467115428.png)

回到主函数文件编译，此时应该没有问题了。

##  编辑代码

- 配置好引脚，进入Clion

  - 本地化u8g2库，就是把u8g2库和SPI的接口对接

    记得加上
	  
    ```c
    u8g2_t u8g2;
    ```
    
    作为u8g2库的句柄（作为操作对象）
    
    u8g2初始化函数
  
  ```c
  uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
      U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
      U8X8_UNUSED void *arg_ptr)
  {
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      HAL_Delay(1);
      break;
    case U8X8_MSG_DELAY_MILLI:
      HAL_Delay(arg_int);
      break;
    case U8X8_MSG_GPIO_DC:
      HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
      break;
    case U8X8_MSG_GPIO_RESET:
      HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, arg_int);
      break;
    }
    return 1;
  }
  
  uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
      void *arg_ptr)
  {
    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
      HAL_SPI_Transmit(&hspi1, (uint8_t *) arg_ptr, arg_int, HAL_MAX_DELAY);
      break;
    case U8X8_MSG_BYTE_INIT:
      break;
    case U8X8_MSG_BYTE_SET_DC:
      HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
      break;
	  case U8X8_MSG_BYTE_START_TRANSFER:
  		HAL_GPIO_WritePin(OLED_NSS_GPIO_Port, OLED_NSS_Pin, GPIO_PIN_RESET);
	    break;
	  case U8X8_MSG_BYTE_END_TRANSFER:
  		HAL_GPIO_WritePin(OLED_NSS_GPIO_Port, OLED_NSS_Pin, GPIO_PIN_SET);
      break;
	  default:
	    return 0;
	  }
    return 1;
  }
  ```
  
  main函数中代码：
  
  ```c
    //u8g2设置、初始化
	u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	
	  //u8g2调用以驱动屏幕
	  u8g2_FirstPage(&u8g2);
	  do
	  {
	      u8g2_SetFont(&u8g2, u8g2_font_courR12_tf);
	      u8g2_DrawStr(&u8g2, 15, 15, "SAST Electronics");
	      u8g2_DrawStr(&u8g2, 40, 15, "hello STM32!");
	  } while (u8g2_NextPage(&u8g2));
	```

## 另外附上一份高质量代码

```c
//矩阵键盘相关内容

typedef struct {
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
} GPIO_Pin_TypeDef;

GPIO_Pin_TypeDef keyboardRow[] = {
	{ KB_R3_GPIO_Port, KB_R3_Pin },
	{ KB_R2_GPIO_Port, KB_R2_Pin },
	{ KB_R1_GPIO_Port, KB_R1_Pin },
	{ KB_R0_GPIO_Port, KB_R0_Pin },
};

GPIO_Pin_TypeDef keyboardCol[] = {
	{ KB_C3_GPIO_Port, KB_C3_Pin },
	{ KB_C2_GPIO_Port, KB_C2_Pin },
	{ KB_C1_GPIO_Port, KB_C1_Pin },
	{ KB_C0_GPIO_Port, KB_C0_Pin },
};

uint32_t KeyboardScan(void)
{
	uint32_t result = 0;
	for (int row = 0; row < sizeof(keyboardRow) / sizeof(GPIO_Pin_TypeDef); row++)
	{
		HAL_GPIO_WritePin(keyboardRow[row].GPIOx, keyboardRow[row].GPIO_Pin, GPIO_PIN_SET);	
		for (int col = 0; col < sizeof(keyboardCol) / sizeof(GPIO_Pin_TypeDef); col++)
		{
			result <<= 1;
			result |= HAL_GPIO_ReadPin(keyboardCol[col].GPIOx, keyboardCol[col].GPIO_Pin) == GPIO_PIN_SET;
		}	
		HAL_GPIO_WritePin(keyboardRow[row].GPIOx, keyboardRow[row].GPIO_Pin, GPIO_PIN_RESET);
	}
	return result;
}
```

部分借鉴学长哲哲的博客

[可以前往友链模块查看或者点击友链查看](https://zhewana.cn/2021/c69c321f18aa/)
