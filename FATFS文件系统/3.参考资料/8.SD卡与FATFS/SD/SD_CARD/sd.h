#ifndef __SD_H
#define __SD_H																			   
#include "stm32f4xx.h" 													   



//SDIO相关标志位,拷贝自:stm32f4xx_sdio.h
#define SDIO_FLAG_CCRCFAIL                  ((uint32_t)0x00000001)
#define SDIO_FLAG_DCRCFAIL                  ((uint32_t)0x00000002)
#define SDIO_FLAG_CTIMEOUT                  ((uint32_t)0x00000004)
#define SDIO_FLAG_DTIMEOUT                  ((uint32_t)0x00000008)
#define SDIO_FLAG_TXUNDERR                  ((uint32_t)0x00000010)
#define SDIO_FLAG_RXOVERR                   ((uint32_t)0x00000020)
#define SDIO_FLAG_CMDREND                   ((uint32_t)0x00000040)
#define SDIO_FLAG_CMDSENT                   ((uint32_t)0x00000080)
#define SDIO_FLAG_DATAEND                   ((uint32_t)0x00000100)
#define SDIO_FLAG_STBITERR                  ((uint32_t)0x00000200)
#define SDIO_FLAG_DBCKEND                   ((uint32_t)0x00000400)
#define SDIO_FLAG_CMDACT                    ((uint32_t)0x00000800)
#define SDIO_FLAG_TXACT                     ((uint32_t)0x00001000)
#define SDIO_FLAG_RXACT                     ((uint32_t)0x00002000)
#define SDIO_FLAG_TXFIFOHE                  ((uint32_t)0x00004000)
#define SDIO_FLAG_RXFIFOHF                  ((uint32_t)0x00008000)
#define SDIO_FLAG_TXFIFOF                   ((uint32_t)0x00010000)
#define SDIO_FLAG_RXFIFOF                   ((uint32_t)0x00020000)
#define SDIO_FLAG_TXFIFOE                   ((uint32_t)0x00040000)
#define SDIO_FLAG_RXFIFOE                   ((uint32_t)0x00080000)
#define SDIO_FLAG_TXDAVL                    ((uint32_t)0x00100000)
#define SDIO_FLAG_RXDAVL                    ((uint32_t)0x00200000)
#define SDIO_FLAG_SDIOIT                    ((uint32_t)0x00400000)
#define SDIO_FLAG_CEATAEND                  ((uint32_t)0x00800000)


//用户配置区			  
//SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz
//使用DMA模式的时候,传输速率可以到48Mhz(bypass on时),不过如果你的卡不是高速
//卡,可能也会出错,出错就请降低时钟
#define SDIO_INIT_CLK_DIV        0x76 		//SDIO初始化频率，最大400Kh  
#define SDIO_TRANSFER_CLK_DIV    0x00		//SDIO传输频率,该值太小可能会导致读写文件出错 
										 

//////////////////////////////////////////////////////////////////////////////////////////////////// 
//SDIO工作模式定义,通过SD_SetDeviceMode函数设置.
#define SD_POLLING_MODE    	0  	//查询模式,该模式下,如果读写有问题,建议增大SDIO_TRANSFER_CLK_DIV的设置.
#define SD_DMA_MODE    		1	//DMA模式,该模式下,如果读写有问题,建议增大SDIO_TRANSFER_CLK_DIV的设置.   

//SDIO 各种错误枚举定义
typedef enum
{	 
	//特殊错误定义 
	SD_CMD_CRC_FAIL                    = (1), /*!< Command response received (but CRC check failed) */
	SD_DATA_CRC_FAIL                   = (2), /*!< Data bock sent/received (CRC check Failed) */
	SD_CMD_RSP_TIMEOUT                 = (3), /*!< Command response timeout */
	SD_DATA_TIMEOUT                    = (4), /*!< Data time out */
	SD_TX_UNDERRUN                     = (5), /*!< Transmit FIFO under-run */
	SD_RX_OVERRUN                      = (6), /*!< Receive FIFO over-run */
	SD_START_BIT_ERR                   = (7), /*!< Start bit not detected on all data signals in widE bus mode */
	SD_CMD_OUT_OF_RANGE                = (8), /*!< CMD's argument was out of range.*/
	SD_ADDR_MISALIGNED                 = (9), /*!< Misaligned address */
	SD_BLOCK_LEN_ERR                   = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
	SD_ERASE_SEQ_ERR                   = (11), /*!< An error in the sequence of erase command occurs.*/
	SD_BAD_ERASE_PARAM                 = (12), /*!< An Invalid selection for erase groups */
	SD_WRITE_PROT_VIOLATION            = (13), /*!< Attempt to program a write protect block */
	SD_LOCK_UNLOCK_FAILED              = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
	SD_COM_CRC_FAILED                  = (15), /*!< CRC check of the previous command failed */
	SD_ILLEGAL_CMD                     = (16), /*!< Command is not legal for the card state */
	SD_CARD_ECC_FAILED                 = (17), /*!< Card internal ECC was applied but failed to correct the data */
	SD_CC_ERROR                        = (18), /*!< Internal card controller error */
	SD_GENERAL_UNKNOWN_ERROR           = (19), /*!< General or Unknown error */
	SD_STREAM_READ_UNDERRUN            = (20), /*!< The card could not sustain data transfer in stream read operation. */
	SD_STREAM_WRITE_OVERRUN            = (21), /*!< The card could not sustain data programming in stream mode */
	SD_CID_CSD_OVERWRITE               = (22), /*!< CID/CSD overwrite error */
	SD_WP_ERASE_SKIP                   = (23), /*!< only partial address space was erased */
	SD_CARD_ECC_DISABLED               = (24), /*!< Command has been executed without using internal ECC */
	SD_ERASE_RESET                     = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
	SD_AKE_SEQ_ERROR                   = (26), /*!< Error in sequence of authentication. */
	SD_INVALID_VOLTRANGE               = (27),
	SD_ADDR_OUT_OF_RANGE               = (28),
	SD_SWITCH_ERROR                    = (29),
	SD_SDIO_DISABLED                   = (30),
	SD_SDIO_FUNCTION_BUSY              = (31),
	SD_SDIO_FUNCTION_FAILED            = (32),
	SD_SDIO_UNKNOWN_FUNCTION           = (33),
	//标准错误定义
	SD_INTERNAL_ERROR, 
	SD_NOT_CONFIGURED,
	SD_REQUEST_PENDING, 
	SD_REQUEST_NOT_APPLICABLE, 
	SD_INVALID_PARAMETER,  
	SD_UNSUPPORTED_FEATURE,  
	SD_UNSUPPORTED_HW,  
	SD_ERROR,  
	SD_OK = 0 
} SD_Error;		  

//SD卡CSD寄存器数据		  
typedef struct
{
	unsigned char  CSDStruct;            /*!< CSD structure */
	unsigned char  SysSpecVersion;       /*!< System specification version */
	unsigned char  Reserved1;            /*!< Reserved */
	unsigned char  TAAC;                 /*!< Data read access-time 1 */
	unsigned char  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
	unsigned char  MaxBusClkFrec;        /*!< Max. bus clock frequency */
	unsigned short CardComdClasses;      /*!< Card command classes */
	unsigned char  RdBlockLen;           /*!< Max. read data block length */
	unsigned char  PartBlockRead;        /*!< Partial blocks for read allowed */
	unsigned char  WrBlockMisalign;      /*!< Write block misalignment */
	unsigned char  RdBlockMisalign;      /*!< Read block misalignment */
	unsigned char  DSRImpl;              /*!< DSR implemented */
	unsigned char  Reserved2;            /*!< Reserved */
	unsigned int   DeviceSize;           /*!< Device Size */
	unsigned char  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
	unsigned char  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
	unsigned char  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
	unsigned char  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
	unsigned char  DeviceSizeMul;        /*!< Device size multiplier */
	unsigned char  EraseGrSize;          /*!< Erase group size */
	unsigned char  EraseGrMul;           /*!< Erase group size multiplier */
	unsigned char  WrProtectGrSize;      /*!< Write protect group size */
	unsigned char  WrProtectGrEnable;    /*!< Write protect group enable */
	unsigned char  ManDeflECC;           /*!< Manufacturer default ECC */
	unsigned char  WrSpeedFact;          /*!< Write speed factor */
	unsigned char  MaxWrBlockLen;        /*!< Max. write data block length */
	unsigned char  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
	unsigned char  Reserved3;            /*!< Reserded */
	unsigned char  ContentProtectAppli;  /*!< Content protection application */
	unsigned char  FileFormatGrouop;     /*!< File format group */
	unsigned char  CopyFlag;             /*!< Copy flag (OTP) */
	unsigned char  PermWrProtect;        /*!< Permanent write protection */
	unsigned char  TempWrProtect;        /*!< Temporary write protection */
	unsigned char  FileFormat;           /*!< File Format */
	unsigned char  ECC;                  /*!< ECC code */
	unsigned char  CSD_CRC;              /*!< CSD CRC */
	unsigned char  Reserved4;            /*!< always 1*/
} SD_CSD;   

//SD卡CID寄存器数据
typedef struct
{
	unsigned char  ManufacturerID;       /*!< ManufacturerID */
	unsigned short OEM_AppliID;          /*!< OEM/Application ID */
	unsigned int   ProdName1;            /*!< Product Name part1 */
	unsigned char  ProdName2;            /*!< Product Name part2*/
	unsigned char  ProdRev;              /*!< Product Revision */
	unsigned int   ProdSN;               /*!< Product Serial Number */
	unsigned char  Reserved1;            /*!< Reserved1 */
	unsigned short ManufactDate;         /*!< Manufacturing Date */
	unsigned char  CID_CRC;              /*!< CID CRC */
	unsigned char  Reserved2;            /*!< always 1 */
} SD_CID;	 
//SD卡状态
typedef enum
{
	SD_CARD_READY                  = ((uint32_t)0x00000001),
	SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
	SD_CARD_STANDBY                = ((uint32_t)0x00000003),
	SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
	SD_CARD_SENDING                = ((uint32_t)0x00000005),
	SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
	SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
	SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
	SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;

//SD卡信息,包括CSD,CID等数据
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  long long CardCapacity;  			//SD卡容量,单位:字节,最大支持2^64字节大小的卡.
  unsigned int CardBlockSize; 		//SD卡块大小	
  unsigned short RCA;				//卡相对地址
  unsigned char CardType;			//卡类型
} SD_CardInfo;
extern SD_CardInfo SDCardInfo;//SD卡信息			 
////////////////////////////////////////////////////////////////////////////////////////////////////
//SDIO 指令集
#define SD_CMD_GO_IDLE_STATE                       ((unsigned char)0)
#define SD_CMD_SEND_OP_COND                        ((unsigned char)1)
#define SD_CMD_ALL_SEND_CID                        ((unsigned char)2)
#define SD_CMD_SET_REL_ADDR                        ((unsigned char)3) /*!< SDIO_SEND_REL_ADDR for SD Card */
#define SD_CMD_SET_DSR                             ((unsigned char)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((unsigned char)5)
#define SD_CMD_HS_SWITCH                           ((unsigned char)6)
#define SD_CMD_SEL_DESEL_CARD                      ((unsigned char)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((unsigned char)8)
#define SD_CMD_SEND_CSD                            ((unsigned char)9)
#define SD_CMD_SEND_CID                            ((unsigned char)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((unsigned char)11) /*!< SD Card doesn't support it */
#define SD_CMD_STOP_TRANSMISSION                   ((unsigned char)12)
#define SD_CMD_SEND_STATUS                         ((unsigned char)13)
#define SD_CMD_HS_BUSTEST_READ                     ((unsigned char)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((unsigned char)15)
#define SD_CMD_SET_BLOCKLEN                        ((unsigned char)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((unsigned char)17)
#define SD_CMD_READ_MULT_BLOCK                     ((unsigned char)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((unsigned char)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((unsigned char)20) 
#define SD_CMD_SET_BLOCK_COUNT                     ((unsigned char)23) 
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((unsigned char)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((unsigned char)25)
#define SD_CMD_PROG_CID                            ((unsigned char)26)
#define SD_CMD_PROG_CSD                            ((unsigned char)27)
#define SD_CMD_SET_WRITE_PROT                      ((unsigned char)28)
#define SD_CMD_CLR_WRITE_PROT                      ((unsigned char)29)
#define SD_CMD_SEND_WRITE_PROT                     ((unsigned char)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((unsigned char)32) /*!< To set the address of the first write
                                                                  block to be erased. (For SD card only) */
#define SD_CMD_SD_ERASE_GRP_END                    ((unsigned char)33) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For SD card only) */
#define SD_CMD_ERASE_GRP_START                     ((unsigned char)35) /*!< To set the address of the first write block to be erased.
                                                                  (For MMC card only spec 3.31) */

#define SD_CMD_ERASE_GRP_END                       ((unsigned char)36) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For MMC card only spec 3.31) */

#define SD_CMD_ERASE                               ((unsigned char)38)
#define SD_CMD_FAST_IO                             ((unsigned char)39) /*!< SD Card doesn't support it */
#define SD_CMD_GO_IRQ_STATE                        ((unsigned char)40) /*!< SD Card doesn't support it */
#define SD_CMD_LOCK_UNLOCK                         ((unsigned char)42)
#define SD_CMD_APP_CMD                             ((unsigned char)55)
#define SD_CMD_GEN_CMD                             ((unsigned char)56)
#define SD_CMD_NO_CMD                              ((unsigned char)64)

/** 
  * @brief Following commands are SD Card Specific commands.
  *        SDIO_APP_CMD ：CMD55 should be sent before sending these commands. 
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((unsigned char)6)  /*!< For SD Card only */
#define SD_CMD_SD_APP_STAUS                        ((unsigned char)13) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((unsigned char)22) /*!< For SD Card only */
#define SD_CMD_SD_APP_OP_COND                      ((unsigned char)41) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((unsigned char)42) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_SCR                     ((unsigned char)51) /*!< For SD Card only */
#define SD_CMD_SDIO_RW_DIRECT                      ((unsigned char)52) /*!< For SD I/O Card only */
#define SD_CMD_SDIO_RW_EXTENDED                    ((unsigned char)53) /*!< For SD I/O Card only */

/** 
  * @brief Following commands are SD Card Specific security commands.
  *        SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_SD_APP_GET_MKB                      ((unsigned char)43) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_MID                      ((unsigned char)44) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((unsigned char)45) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((unsigned char)46) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((unsigned char)47) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((unsigned char)48) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((unsigned char)18) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((unsigned char)25) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((unsigned char)38) /*!< For SD Card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((unsigned char)49) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((unsigned char)48) /*!< For SD Card only */
  			   
//支持的SD卡定义
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((unsigned int)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((unsigned int)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((unsigned int)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((unsigned int)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((unsigned int)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((unsigned int)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((unsigned int)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((unsigned int)0x00000007)

//SDIO相关参数定义
#define NULL 0
#define SDIO_STATIC_FLAGS               ((unsigned int)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((unsigned int)0x00010000)	  
#define SDIO_DATATIMEOUT                ((unsigned int)0xFFFFFFFF)	  
#define SDIO_FIFO_Address               ((unsigned int)0x40018080)

//Mask for errors Card Status R1 (OCR Register)  
#define SD_OCR_ADDR_OUT_OF_RANGE        ((unsigned int)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((unsigned int)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((unsigned int)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((unsigned int)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((unsigned int)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((unsigned int)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((unsigned int)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((unsigned int)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((unsigned int)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((unsigned int)0x00200000)
#define SD_OCR_CC_ERROR                 ((unsigned int)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((unsigned int)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((unsigned int)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((unsigned int)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((unsigned int)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((unsigned int)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((unsigned int)0x00004000)
#define SD_OCR_ERASE_RESET              ((unsigned int)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((unsigned int)0x00000008)
#define SD_OCR_ERRORBITS                ((unsigned int)0xFDFFE008)

//Masks for R6 Response 
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((unsigned int)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((unsigned int)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((unsigned int)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((unsigned int)0x80100000)
#define SD_HIGH_CAPACITY                ((unsigned int)0x40000000)
#define SD_STD_CAPACITY                 ((unsigned int)0x00000000)
#define SD_CHECK_PATTERN                ((unsigned int)0x000001AA)
#define SD_VOLTAGE_WINDOW_MMC           ((unsigned int)0x80FF8000)

#define SD_MAX_VOLT_TRIAL               ((unsigned int)0x0000FFFF)
#define SD_ALLZERO                      ((unsigned int)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((unsigned int)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((unsigned int)0x00010000)
#define SD_CARD_LOCKED                  ((unsigned int)0x02000000)
#define SD_CARD_PROGRAMMING             ((unsigned int)0x00000007)
#define SD_CARD_RECEIVING               ((unsigned int)0x00000006)
#define SD_DATATIMEOUT                  ((unsigned int)0xFFFFFFFF)
#define SD_0TO7BITS                     ((unsigned int)0x000000FF)
#define SD_8TO15BITS                    ((unsigned int)0x0000FF00)
#define SD_16TO23BITS                   ((unsigned int)0x00FF0000)
#define SD_24TO31BITS                   ((unsigned int)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((unsigned int)0x01FFFFFF)

#define SD_HALFFIFO                     ((unsigned int)0x00000008)
#define SD_HALFFIFOBYTES                ((unsigned int)0x00000020)

//Command Class Supported  
#define SD_CCCC_LOCK_UNLOCK             ((unsigned int)0x00000080)
#define SD_CCCC_WRITE_PROT              ((unsigned int)0x00000040)
#define SD_CCCC_ERASE                   ((unsigned int)0x00000020)
																	 
//CMD8指令
#define SDIO_SEND_IF_COND               ((unsigned int)0x00000008)
////////////////////////////////////////////////////////////////////////////////////////////////////
//相关函数定义
SD_Error SD_Init(void);
void SDIO_Clock_Set(unsigned char clkdiv);
void SDIO_Send_Cmd(unsigned char cmdindex, unsigned char waitrsp, unsigned int arg);
void SDIO_Send_Data_Cfg(unsigned int datatimeout, unsigned int datalen, unsigned char blksize, unsigned char dir);
SD_Error SD_PowerON(void);    
SD_Error SD_PowerOFF(void);
SD_Error SD_InitializeCards(void);
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);		  
SD_Error SD_EnableWideBusOperation(unsigned int wmode);
SD_Error SD_SetDeviceMode(unsigned int mode);
SD_Error SD_SelectDeselect(unsigned int addr); 
SD_Error SD_SendStatus(uint32_t *pcardstatus);
SDCardState SD_GetState(void);
SD_Error SD_ReadBlock(unsigned char *buf, long long addr, unsigned short blksize);  
SD_Error SD_ReadMultiBlocks(unsigned char *buf, long long  addr, unsigned short blksize, unsigned int nblks);  
SD_Error SD_WriteBlock(unsigned char *buf, long long addr,  unsigned short blksize);	
SD_Error SD_WriteMultiBlocks(unsigned char *buf, long long addr, unsigned short blksize, unsigned int nblks);
SD_Error SD_ProcessIRQSrc(void);
SD_Error CmdError(void);  
SD_Error CmdResp7Error(void);
SD_Error CmdResp1Error(unsigned char cmd);
SD_Error CmdResp3Error(void);
SD_Error CmdResp2Error(void);
SD_Error CmdResp6Error(unsigned char cmd, unsigned short*prca);  
SD_Error SDEnWideBus(unsigned char enx);	  
SD_Error IsCardProgramming(unsigned char *pstatus); 
SD_Error FindSCR(unsigned short rca, unsigned int *pscr);
unsigned char convert_from_bytes_to_power_of_two(unsigned short NumberOfBytes); 
void SD_DMA_Config(unsigned int*mbuf, unsigned int bufsize, unsigned char dir); 

unsigned char SD_ReadDisk(unsigned char*buf, unsigned int sector, unsigned char cnt); 	
unsigned char SD_WriteDisk(unsigned char*buf, unsigned int sector, unsigned char cnt);	
void SD_ShowSdcard_Info(void);

#endif 




