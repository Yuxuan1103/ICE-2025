#include "main.h"

/*****************************************************************************************
Function Name       :  FLASH_PagesMask
Description         :  计算文件所需要占用的Flash页数
Input               :  Size：文件大小
Output              :  pagenumber：页数
Author              :  Lh
Creation Date       :  
Revisor             :  1.0
Revision Date       : 
Note                :  None
******************************************************************************************/ 
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;

}

/*****************************************************************************************
Function Name       :  FLASH_ErasePage
Description         :  擦除指定 页 内容  
						   Page_Address：指定擦除起始页
						   Number_Pages：擦除页数
Output              :        status：擦除状态
Author              :  Lh
Creation Date       : 
Revisor             :  1.0
Revision Date       :  
Note                :  None
******************************************************************************************/
HAL_StatusTypeDef FLASH_ErasePage(uint32_t Page_Address, uint32_t Number_Pages)
{ 
	uint32_t PageError;
	HAL_StatusTypeDef status;
	FLASH_EraseInitTypeDef pEraseInit;
  
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;/* 擦除方式，这里选择按页擦除 */
	pEraseInit.PageAddress = Page_Address;//擦除指定地址
	pEraseInit.NbPages = Number_Pages;/* 页数量，模块中只对单页进行操作 */
	
	HAL_FLASH_Unlock();//FlASH解锁
	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP | 
	FLASH_FLAG_ENDHV | FLASH_FLAG_READY | FLASH_FLAG_PGAERR | 
	FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_FWWERR | 
	FLASH_FLAG_RDERR | FLASH_FLAG_WRPERR | FLASH_FLAG_NOTZEROERR);


	status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
	HAL_FLASH_Lock();//FlASH上锁
	
	return status;
}

/*****************************************************************************************
Function Name       :  FlashProgramData
Description         :  将字节数据数组写到Flash指定一个地址块
Input               :   address：目标块起始地址
											 databuff：字节数据数组
											     size：数组长度（4的倍数）
Output              :  0：写入成功
                       1：Flash 数据校对失败
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/
uint8_t FlashProgramData(uint32_t address, uint8_t* buf_ptr1 , uint32_t size)
{
	uint32_t j;
	uint32_t SourceAddress = (uint32_t)buf_ptr1;//初始化Flash地址变量
	uint32_t FlashDestination3 = address; //存放地址
	HAL_FLASH_Unlock();//FlASH解锁
	//清标志位
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP | 
	FLASH_FLAG_ENDHV | FLASH_FLAG_READY | FLASH_FLAG_PGAERR | 
	FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_FWWERR | 
	FLASH_FLAG_RDERR | FLASH_FLAG_WRPERR | FLASH_FLAG_NOTZEROERR);
	
	for (j = 0; (j < size) && (FlashDestination3 <  address + size); j += 4)
    {
        //把缓存的数据编写到应用程序Flash中
		HAL_FLASH_Program(TYPEPROGRAM_WORD, FlashDestination3, *(uint32_t*)SourceAddress);//按字写数据
        //FLASH_ProgramWord(FlashDestination3, *(uint32_t*)SourceAddress);
        if (*(uint32_t*)FlashDestination3 != *(uint32_t*)SourceAddress)//数据校对
        {
            return 1;//数据校对失败				
        }
				FlashDestination3 += 4;
				SourceAddress += 4;
    }
		HAL_FLASH_Lock();//FlASH上锁
		return 0;
}
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(uint32_t*)faddr; 
}


