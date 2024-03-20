//
//*******************************************
//**  Copyright  (C)  WCH  2004-2023       **
//**  Web:  http://wch.cn                  **
//*******************************************
//**  DLL for USB to uart chip CH34X/CH910X**
//**  C, VC6.0                             **
//*******************************************
//
// USB���߽ӿ�оƬ�˿ڹ����ӿڿ� V1.6
// �Ͼ��ߺ�΢����
// CH34X-Port-DLL  V1.6
// ���л���: Windows 98/ME, Windows 2000/XP,Windows 7/8/10/11
// support USB chip: CH34X/CH910X
// USB => Single/Multi serial port
//

#ifndef		_CH343_PORT_H
#define		_CH343_PORT_H

#ifdef __cplusplus
extern "C" {
#endif
	
//WCH�����ͺ�
//δ֪�ͺ�
#define USER_TYPE_Unknown   0xFF
//USBת����1
#define USER_TYPE_CH341     0x10
//USBת����2
#define USER_TYPE_CH340     0x20
#define USER_TYPE_CH340K    0x21
#define USER_TYPE_CH330     0x22
#define USER_TYPE_CH9340    0x23
#define USER_TYPE_CH9340K   0x24
#define USER_TYPE_CH9340C   0x25
#define USER_TYPE_CH34E     0x26
#define USER_TYPE_CH34X     0x27
//USBת����3
#define USER_TYPE_CH343K    0x30
#define USER_TYPE_CH343J    0x31
#define USER_TYPE_CH343G    0x32 //CH343G/P
#define USER_TYPE_CH343P    0x33
//USBת����4,CH9101U/H��ͬ
#define USER_TYPE_CH9101U   0x40
#define USER_TYPE_CH9101H   0x40
//USBת����4,CH9101R/Y��ͬ
#define USER_TYPE_CH9101R   0x41
#define USER_TYPE_CH9101Y   0x41
#define USER_TYPE_CH9101N   0x42
//USBת����5
#define USER_TYPE_CH9102F   0x50
#define USER_TYPE_CH9102X   0x51
//USBת����6
#define USER_TYPE_CH9103M   0x60

//USBת˫��
#define USER_TYPE_CH342F    0xA0
#define USER_TYPE_CH342K    0xA1
#define USER_TYPE_CH342J    0xA2
#define USER_TYPE_CH342G    0xA3
#define USER_TYPE_CH347T    0xA4
#define USER_TYPE_CH347F    0xA5

//USBת�Ĵ�
#define USER_TYPE_CH9344    0xD0
#define USER_TYPE_CH344L    0xD1
#define USER_TYPE_CH344Q    0xD2
#define USER_TYPE_CH9104L   0xD3

//USBת�˴�
#define USER_TYPE_CH348L     0xE0
#define USER_TYPE_CH348Q     0xE1

// Return value
#define     CH910x_SUCCESS                      0x00
#define     CH910x_INVALID_HANDLE               0x01
#define     CH910x_INVALID_PARAMETER            0x02
#define     CH910x_DEVICE_IO_FAILED             0x03
#define     CH910x_FUNCTION_NOT_SUPPORTED       0x04
#define     CH910x_NOT_INIT                     0x05

// GPIO mask bit definitions
#define     CH910x_GPIO_0         0x0001
#define     CH910x_GPIO_1         0x0002
#define     CH910x_GPIO_2         0x0004
#define     CH910x_GPIO_3         0x0008
#define     CH910x_GPIO_4         0x0010
#define     CH910x_GPIO_5         0x0020
#define     CH910x_GPIO_6         0x0040
#define     CH910x_GPIO_7         0x0080
#define     CH910x_GPIO_8         0x0100
#define     CH910x_GPIO_9         0x0200
#define     CH910x_GPIO_10        0x0400
#define     CH910x_GPIO_11        0x0800
#define     CH910x_GPIO_12        0x1000
#define     CH910x_GPIO_13        0x2000
#define     CH910x_GPIO_14        0x4000
#define     CH910x_GPIO_15        0x8000

//оƬ������Ϣ�ṹ
typedef struct _USBSER_Property
{
	UCHAR    ChipType;              //оƬ�ͺ�,USER_TYPE_CHxxx	
	CHAR     ChipTypeStr[32];       //оƬ�ͺ��ַ���
	CHAR     FwVerStr[32];          //�̼��汾�ַ���
	UCHAR    GpioCount;             //GPIO����,�������0�����ͺ���ר��GPIO�Ż��ý�;Ϊ0��ʾоƬ��֧��GPIO����
	BOOL     IsEmbbedEeprom;        //�Ƿ�����EEPROM,��֧�֣��ɽ����豸��Ϣ������	
	BOOL     IsSupportMcuBootCtrl;  //�Ƿ�֧��Modem������Ϊ103 MCU���ؿ���
	CHAR     ManufacturerString[64];//USB�������ڳ����ַ���
	CHAR     ProductString[64];     //USB�������ڲ�Ʒ�ַ���
	USHORT   bcdDevice;             //USB��������bcdDeviceֵ
	UCHAR    PortIndex;             //����Ϊ0����മ��ΪоƬ�ڼ����Ŵ���
	BOOL     IsSupportGPIOInit;     //�Ƿ�֧��GPIO�ϵ��ʼ������
	CHAR     PortName[32];          //���ں�
	ULONG    ResvD[8];              //Ԥ������
}ChipPropertyS,*pChipPropertyS;

//��CH340��CH341֮�������USB����оƬ����EEPROM
typedef struct _USERCFG_343
{
	UCHAR SIG;      //���� CH340B���ڲ�������Ϣ��Ч��־�������� 5BH��
	//���� CH340H/S���ⲿ����оƬ��Ч��־�������� 53H��
	//���� CH343??���ⲿ����оƬ��Ч��־�������� 53H��
	//����ֵ��������Ч
	UCHAR MODE;     //����ģʽ��������23H
	UCHAR CFG;      //оƬ���������ֽ�
	//BIT7��USB�豸���к��ַ�������λ��1=��Ч��0=��Ч��
	//BIT6��USB��Ʒ��Ϣ�ַ�������λ��1=��Ч��0=��Ч��
	//BIT5��USB������Ϣ�ַ�������λ��1=��Ч��0=��Ч��
	//BIT4��CDCģʽ���Ƿ�����Ӳ�����أ�1=���ã�0=��ֹ��
	//BIT3��оƬ�����Ƿ�����EEPROM��Ĭ�����ã����������оƬ�ϵ������EEPROM�е����ó�ʼ�����ŷ��򼰵�ƽ״̬���Լ�ʹ��״̬����������ֵ������˵���� 1=���ã�0=��ֹ��
	UCHAR WP;       //�ڲ�������Ϣд������־��Ϊ 57H ��ֻ��������ɸ�д
	UCHAR Vid[2];   //Vendor ID������ʶ���룬���ֽ��ں�����ֵ��
	UCHAR Pid[2];   //Product ID����Ʒʶ���룬���ֽ��ں�����ֵ
	UCHAR BCD[2];   //���ֽ��ں�,����ֵ
	UCHAR Power;    //Max Power��USB����������,��2mAΪ��λ������Դ����
	UCHAR Attributes;//	USB���緽ʽ��˯�߻��ѵȹ���������Ϣ
	//BIT7���������ó�1��
	//BIT6��1= Self-Powered��
	//0=Bus-Powered��
	//BIT5��1= Remote Wakeup Enable��
	//0= Remote Wakeup Disable��
	//BIT4-BIT0���������ó�0��
	UCHAR Reserved[4]; //�����ֽ�00H��FFH
	CHAR  SN[24];    //USB�豸���к��ַ���.���ֽ���ȫ���ֽ���(������16H)�����ֽڿ�ʼΪ��׼��USB�ַ�����������ʽ(��1���ֽ�Ϊ�ܳ��ȣ���2���ֽڹ̶�Ϊ03H����3���ֽڿ�ʼ����ΪUnicode�ַ�,ż��Ϊ0x00)���������������������ʹ�ó���Ĭ��˵����
	UCHAR PROD[40];  //USB��Ʒ��Ϣ�ַ���.���ֽ���ȫ���ֽ���(������28H)�����ֽڿ�ʼΪ��׼��USB�ַ�����������ʽ(��1���ֽ�Ϊ�ܳ��ȣ���2���ֽڹ̶�Ϊ03H����3���ֽڿ�ʼ����ΪUnicode�ַ�,ż��Ϊ0x00)���������������������ʹ�ó���Ĭ��˵����
	UCHAR MAU[40];   //USB������Ϣ�ַ���.���ֽ���ȫ���ֽ���(������28H)�����ֽڿ�ʼΪ��׼��USB�ַ�����������ʽ(��1���ֽ�Ϊ�ܳ��ȣ���2���ֽڹ̶�Ϊ03H����3���ֽڿ�ʼ����ΪUnicode�ַ�,ż��Ϊ0x00)���������������������ʹ�ó���Ĭ��˵����
}USERCFG_343,*pUSERCFG_343;

//CH340��CH341
typedef struct _USERCFG_340
{
	UCHAR SIG;        //���� CH340B���ڲ�������Ϣ��Ч��־�������� 5BH��
	//����ֵ��������Ч
	UCHAR MODE;       //����ģʽ��������23H 
	UCHAR CFG;        //оƬ���������ֽ�
	//λ 5 �������ò�Ʒ���к��ַ�����0=��Ч��1=��Ч
	UCHAR WP;         //�ڲ�������Ϣд������־��Ϊ 57H ��ֻ��������ɸ�д
	UCHAR Vid[2];     //Vendor ID������ʶ���룬���ֽ��ں�����ֵ������Ϊ 0000H �� 0FFFFH �� VID �� PID ʹ�ó���Ĭ��ֵ
	UCHAR Pid[2];     //Product ID����Ʒʶ���룬���ֽ��ں�����ֵ
	UCHAR Reserve1[2];//0x08-0x09������
	UCHAR Power;      //Max Power��USB����������,��2mAΪ��λ������Դ����
	UCHAR Reserve2[5];//0x0B-0x0F������
	CHAR  SN[8];      //Serial Number����Ʒ���к� ASCII �ַ���������Ϊ 8�����ֽڲ��� ASCII �ַ���21H��7FH����������к�
	UCHAR Reserve3[2];//0x18-0x19������
	UCHAR PROD[38];   //���� CH340B��Product String����Ʒ˵�� Unicode �ַ��������ֽ���ȫ���ֽ����������� 26H�������ֽ��� 03H��֮���� Unicode �ַ���������������������ʹ�ó���Ĭ��˵��	
}USERCFG_340,*pUSERCFG_340;

//USER_XX����������оƬCH34X��CH910X
//��ȡоƬ������Ϣ,����оƬ�ͺš�δ֪�ͺ��򷵻�USER_TYPE_Unknown
UCHAR   WINAPI   CH343PT_GetChipProperty(HANDLE	       iPortHandle,   //���ھ��ֵ
										pChipPropertyS ChipProperty); //����оƬGPIO��EEPROM�ȹ�����Ϣ

ULONG	WINAPI	CH343PT_GetVersion( );  // ���DLL�汾��,���ذ汾��


BOOL	WINAPI CH343PT_HandleIsCH34x(  // ����Ѿ��򿪵Ķ˿��Ƿ�ΪCH341�˿�
									 HANDLE		iPortHandle );  // ָ���Ѿ��򿪵Ķ˿ھ��


BOOL	WINAPI	CH343PT_NameIsCH34x(  // ���ָ�����ƵĶ˿��Ƿ�ΪCH341�˿�
								   PUCHAR		iPortName );  // ָ���˿�����,����"COM3","COM15"��,�ö˿ڱ���δ��ռ��(ָδ�����������)


typedef		VOID	( CALLBACK	* mPCH343PT_NOTIFY_ROUTINE ) (  // �˿��豸�¼�֪ͨ�ص�����
															  LONG			iDevIndexAndEvent );  // �˿��豸��ź��¼�����ǰ״̬(�ο�����˵��)
// iDevIndexAndEvent: ����˵�����豸�����¼�/�Ѿ�����, ����˵�����豸�γ��¼�/�Ѿ��γ�, �����ֵ���豸���(1��255)


BOOL	WINAPI	CH343PT_SetDevNotify(  // �趨�˿��豸�¼�֪ͨ����
									PCHAR						iDeviceID,  // ��ѡ����,ָ����\0��ֹ���ַ���,ָ������ص��豸��ID���߲�����ŵ�������,���ڴ��ڸò�������ΪNULL����"COM"
									mPCH343PT_NOTIFY_ROUTINE	iNotifyRoutine );  // ָ���˿��豸�¼��ص�����,ΪNULL��ȡ���¼�֪ͨ,�����ڼ�⵽�¼�ʱ���øó���
/* ���弴���豸��Ӧ�ó����̲ο�
	1. ������������Ĭ���ǽ�ֹ���ݴ���ģ�ֻ����ȷ����CH341�˿ڿ��ò��Ҵ򿪶˿ں�Ž������ݴ���
	2. ����CH343PtSetDevNotify���ò�μ��ӣ������������CH341�˿ڵĲ���¼�DLL�����Զ�����iNotifyRoutine��ָ�����ӳ�����ߺ���
	3. �Ӷ˿�1���˿�255��������20���ɣ��������CH343PtNameIsCH341ȷ���Ƿ�ΪCH341�˿ڣ�����������������˿ںŲ��򿪺�ʼ���䣬������ط�����Ϣ
	4. ���iNotifyRoutine�յ��¼�֪ͨ����ô�����ڱ����¼�������֪ͨ����������Ҳ�����ڸ��ӳ����д�����
	�����¼������������������˵����һ���˿ڲ��룬��Ҫ��ʱӦ��������CH343PtNameIsCH341�ж��Ƿ�ΪCH341�˿ڣ����������˿ںŲ��򿪺�ʼ���䣬
	����Ǹ�����˵����һ���˿ڰγ����ж���˿ںţ���0��ȥ�ø����ö˿ںţ��Ƿ��������Ѵ򿪵Ķ˿ں���ͬ������Ӧ�ü�ʱ�ر�
	5. ��ǰ�Ѿ��򿪵Ķ˿ں�Ӧ�ñ���Ϊȫ�ֱ���, ����˿�δ�򿪻��߱��رգ���ôӦ�ó���Ӧ��ֹͣ���ݴ���
	6. �������δ��CH343PtNameIsCH34Xȷ���Ƿ�ΪCH34x�˿ڣ���ô�ڶ˿��Ѿ��򿪺�Ҳ���Ե���CH343PtHandleIsCH34X�ж��Ƿ�ΪCH343�˿�
*/

//����оƬ����ģʽ:300bpsp;�ɽ��зǱ�׼���������ú����ÿռ��д
BOOL WINAPI	CH343PT_EnterConfigMode(HANDLE iPortH);

//�˳�оƬ����ģʽ:300bps
BOOL WINAPI	CH343PT_ExitConfigMode(HANDLE iPortH);

//ͨ�����ڶ�ȡоƬEERPOM���豸��������
BOOL    WINAPI	 CH343PT_ReadDevConfig(HANDLE          iPortH,     // ָ��CH341�豸���
									   PULONG          DataLen,    // Ҫ��ȡ�����ݳ���
									   PUCHAR          DataBuf);    // ���ݻ�����


//ͨ��������оƬEERPOM��д���豸����������,д��ǰ��ȷ��дλ����λ�ѹرա�
BOOL    WINAPI	 CH343PT_WriteDevConfig(HANDLE          iPortH,
										ULONG           BufferSize,
										PUCHAR          DataBuf);

//ͨ��������оƬ���õ�EEPROM��д���������ݣ�ÿ��ֻдһ���ֽ�
BOOL	WINAPI	CH343PT_WriteCfgEeprom_Byte(HANDLE         iPortH,     // ָ��CH341�豸���	
											ULONG			iAddr,      // ָ�����ݵ�Ԫ�ĵ�ַ
											UCHAR			iByte );    // ��д����ֽ�����
//EEPROM�ֽڶ�
BOOL	WINAPI	CH343PT_ReadCfgEeprom_Byte(  // ��I2C�ӿڶ�ȡһ���ֽ�����
										   HANDLE          iPortH,    // ָ��CH341�豸���	
										   ULONG			iAddr,     // ָ�����ݵ�Ԫ�ĵ�ַ
										   PUCHAR			oByte );   // ָ��һ���ֽڵ�Ԫ,���ڱ����ȡ���ֽ�����

/*USBת��оƬGPIO���úͲ�������,����USBת����ϵ��оƬCH910X*/
/* ***** ����ʹ��GPIO API, ��ֹ�޸�I/O����ʹ�������ű�Ϊ������ŵ����������������֮���·����оƬ ***** */

//��ʼ��GPIO,����ǰ������ִ��
UCHAR	WINAPI	CH910x_GpioInit(HANDLE		iPortH);


// ����GPIO�Ĺ��ܺ�I/O����
/* ***** ����ʹ�ø�API, ��ֹ�޸�I/O����ʹ�������ű�Ϊ������ŵ����������������֮���·����оƬ ***** */
UCHAR	WINAPI	CH910x_GpioConfig(
								  HANDLE			iPortH,         // ���ھ��ֵ
								  ChipPropertyS*  iChipProperty,
								  ULONG			iEnable,        // λʹ��:��λ��Ӧ���ź�,λ0��ӦGPIO0,λ1��ӦGPIO1;ĳλΪ1,��ʾiGpioFunc,iSetDirOut��Ӧλ��Ч											 
								  ULONG			iFuncSet,       // ��������:ĳλֵΪ0,��ʾ��Ӧ����ΪоƬĬ�Ϲ���ʹ��;Ϊ1,��ʾ��Ӧ������ΪGPIO����ʹ��;
								  ULONG			iSetDirOut);    // GPIO�ŷ�������:ĳλΪ0��ʾ��Ӧ���ŷ���Ϊ����;ĳλΪ1��ʾ��Ӧ���ŷ���Ϊ���

//��ȡGPIO��������:���Ź��ܡ����򡢵�ƽ��iFuncSet��iSetDirOut��iSetDataOutλ�����GPIO��Ŷ�Ӧ
UCHAR	WINAPI	CH910x_GetGpioConfig(
									 HANDLE			iPortH,         // ���ھ��ֵ
									 ChipPropertyS   *iChipProperty, // оƬ������Ϣ����ΪNULL
									 PULONG			FuncSet,        // ��������:ĳλֵΪ0,��ʾ��Ӧ����ΪоƬĬ�Ϲ���ʹ��;Ϊ1,��ʾ��Ӧ������ΪGPIO����ʹ��;
									 PULONG			SetDirOut,      // GPIO�ŷ�������:ĳλΪ0��ʾ��Ӧ���ŷ���Ϊ����;ĳλΪ1��ʾ��Ӧ���ŷ���Ϊ���
									 PULONG          SetDataOut);    // GPIO�ŵ�ƽֵ:1��ʾ�ߵ�ƽ��0��ʾ�͵�ƽ

// ��ȡ�豸������
BOOL	WINAPI	CH343PT_GetDeviceDescr(  // ��ȡ�豸������
									   HANDLE			iPortHandle,        // ָ��CH341�豸���
									   PVOID			oBuffer,            // ָ��һ���㹻��Ļ�����,���ڱ���������
									   PULONG			ioLength );          // ָ�򳤶ȵ�Ԫ,����ʱΪ׼����ȡ�ĳ���,���غ�Ϊʵ�ʶ�ȡ�ĳ���

// ����GPIO���ŵ�ƽ״̬
UCHAR	WINAPI	CH910x_GpioSet(  
							   HANDLE			iPortH,         // ���ھ��ֵ
							   ChipPropertyS*  iChipProperty,
							   ULONG			iEnable,        // ������Ч��־,λ�����GPIOx����Ŷ�Ӧ,λֵΪ1��ʾiSetDataOut�Ķ�Ӧλ������Ч,Ϊ0��ʾ������Ч;
							   ULONG			iSetDataOut );  // IO�������,���GPIO����Ϊ���,��ôĳλ��0ʱ��Ӧ��������͵�ƽ,ĳλ��1ʱ��Ӧ��������ߵ�ƽ

//��ȡGPIO���ŵ�ƽ״̬
UCHAR	WINAPI	CH910x_GpioGet(   
							   HANDLE			iPortH,      // ���ھ��ֵ
							   ChipPropertyS*   iChipProperty,
							   PULONG			iStatus );   // ����״̬,��GPIOx��Ŷ�Ӧ����λ�š���GPIO1����ӦiStatusֵ�е�bit1,����CH910x_GPIO_xxλ����ȡֵ

//ʹ��GPIO�ϵ��ʼ������
BOOL  WINAPI	CH910x_EnableGpioEepromConfig(
											  HANDLE		iPortH,             // ���ھ��ֵ
											  BOOL         iEnable); //TRUE:���ã�FALSE:����

// ����GPIO�ϵ�ʱ�ĳ�ʼ״̬:Ĭ�Ϲ��ܡ�GPIO����͵�ƽ״̬��������ֵ�������õ�EEPROM�ڣ��ϵ��оƬ�ᰴ������ֵ�������á�
// ������ֵ�����ʹ�ã�ȷ�Ϻ����ŷ���͵�ƽ����ֹ��·��
BOOL  WINAPI	CH910x_SetGpioEepromConfig(  										 
										   HANDLE		iPortH,             // ���ھ��ֵ
										   ULONG		iEnable,            // λʹ��:��λ��Ӧ���ź�,λ0��ӦGPIO0,λ1��ӦGPIO1;ĳλΪ1,��ʾiGpioFunc,iSetDirOut,iSetDataOut��Ӧλ��Ч											 
										   ULONG      iFuncSet,          // ��������:ĳλֵΪ0,��ʾ��Ӧ����ΪоƬĬ�Ϲ���ʹ��;Ϊ1,��ʾ��Ӧ������ΪGPIO����ʹ��;
										   ULONG		iSetDirOut,         // GPIO�ŷ�������:ĳλΪ0��ʾ��Ӧ���ŷ���Ϊ����;ĳλΪ1��ʾ��Ӧ���ŷ���Ϊ���
										   ULONG		iSetDataOut,        // GPIO����ŵ�ƽ״̬:ĳλΪ1��ʾ�ߵ�ƽ��Ϊ0��ʾ�͵�ƽ
										   BOOL       IsImmediatelySet,   // �Ƿ��������õ�EEPROM��
										   PUCHAR     EepromData);        // ���ɵ�GPIO�ϵ����õ�EEPROM����,����ΪNULL

//��ȡEeprom��GPIO��������:���Ź��ܡ����򡢵�ƽ��iFuncSet��iSetDirOut��iSetDataOutλ�����GPIO��Ŷ�Ӧ
UCHAR	WINAPI	CH910x_GetGpioEepromConfig(
										   HANDLE			iPortH,         // ���ھ��ֵ	
										   PULONG			iFuncSet,       // ��������:ĳλֵΪ0,��ʾ��Ӧ����ΪоƬĬ�Ϲ���ʹ��;Ϊ1,��ʾ��Ӧ������ΪGPIO����ʹ��;
										   PULONG			iSetDirOut,     // GPIO�ŷ�������:ĳλΪ0��ʾ��Ӧ���ŷ���Ϊ����;ĳλΪ1��ʾ��Ӧ���ŷ���Ϊ���
										   PULONG           iSetDataOut);   // GPIO�ŵ�ƽֵ:1��ʾ�ߵ�ƽ��0��ʾ�͵�ƽ


//��ȡGPIO���ã����ܺ�I/O���������Ĭ��ֵ
UCHAR	WINAPI	CH348_GetGpioConfig(
									HANDLE		    	iPortH,         // ���ھ��ֵ
									ChipPropertyS       *ChipProperty,  // оƬ����ֵ
									ULONGLONG			*FuncSet,       // ��������:ĳλֵΪ0,��ʾ��Ӧ����ΪоƬĬ�Ϲ���ʹ��;Ϊ1,��ʾ��Ӧ������ΪGPIO����ʹ��;
									ULONGLONG			*SetDirOut,     // GPIO�ŷ�������:ĳλΪ0��ʾ��Ӧ���ŷ���Ϊ����;ĳλΪ1��ʾ��Ӧ���ŷ���Ϊ���	
									ULONGLONG           *SetDataOut);   // IO�������,���GPIO����Ϊ���,��ôĳλ��0ʱ��Ӧ��������͵�ƽ,ĳλ��1ʱ��Ӧ��������ߵ�ƽ


// ����GPIO�Ĺ��ܺ�I/O����
//iEnable,iFuncSet,iSetDirOutΪ64λ������λ�����GPIO���źŶ�Ӧ��
//����ʹ��ǰ�ȵ���CH348_GetGpioConfig��ȡiFuncSet,iSetDirOut��ֵ���ٽ�������
UCHAR	WINAPI	CH348_GpioConfig(
								 HANDLE		    	iPortH,         // ���ھ��ֵ
								 ChipPropertyS*      iChipProperty,  // оƬ����ֵ
								 ULONGLONG			iEnable,        // λʹ��:��λ��Ӧ���ź�,λ0��ӦGPIO0,λ1��ӦGPIO1;ĳλΪ1,��ʾiGpioFunc,iSetDirOut��Ӧλ��Ч											 
								 ULONGLONG			iFuncSet,       // ��������:ĳλֵΪ0,��ʾ��Ӧ����ΪоƬĬ�Ϲ���ʹ��;Ϊ1,��ʾ��Ӧ������ΪGPIO����ʹ��;
								 ULONGLONG			iSetDirOut,     // GPIO�ŷ�������:ĳλΪ0��ʾ��Ӧ���ŷ���Ϊ����;ĳλΪ1��ʾ��Ӧ���ŷ���Ϊ���	
								 ULONGLONG           iSetDataOut);    // IO�������,���GPIO����Ϊ���,��ôĳλ��0ʱ��Ӧ��������͵�ƽ,ĳλ��1ʱ��Ӧ��������ߵ�ƽ

// ����ĳ��GPIO���ŵ�ƽ״̬,��Ӧ�����迪��GPIO����
UCHAR	WINAPI	CH348_GpioSet(  
							  HANDLE			iPortH,         // ���ھ��ֵ
							  ChipPropertyS*   iChipProperty,  // оƬ����ֵ
							  UCHAR	        iGpioIndex,     // GPIO�����,0-63
							  UCHAR		    iGpioLevel );   // GPIO��ƽ��1Ϊ�ߵ�ƽ��1���͵�ƽ

// ���ö��GPIO���ŵ�ƽ״̬,��Ӧ�����迪��GPIO����
UCHAR	WINAPI	CH348_GpioMSet(  
							   HANDLE			iPortH,          // ���ھ��ֵ
							   ChipPropertyS*   iChipProperty,   // оƬ����ֵ
							   ULONGLONG        iGpioMIndex,     // GPIO������룬λ0-63
							   ULONGLONG		iGpioMLevel);     // GPIO��ƽ���룬����iGpioMIndexλ�ö�Ӧ��1���ߵ�ƽ��0���͵�ƽ

// ��ȡĳ��GPIO���ŵ�ƽ״̬,��Ӧ�����迪��GPIO����
UCHAR	WINAPI	CH348_GpioGet(  
							  HANDLE			iPortH,         // ���ھ��ֵ
							  ChipPropertyS*   iChipProperty,  // оƬ����ֵ
							  UCHAR	        iGpioIndex,     // GPIO����� 0-63
							  PUCHAR		    GpioLevel );     // GPIO��ƽ��1Ϊ�ߵ�ƽ��1���͵�ƽ

// ��ȡ���GPIO���ŵ�ƽ״̬,��Ӧ�����迪��GPIO����
UCHAR	WINAPI	CH348_GpioMGet(  
							   HANDLE			iPortH,          // ���ھ��ֵ
							   ChipPropertyS*   iChipProperty,   // оƬ����ֵ
							   ULONGLONG        iGpioMIndex,     // GPIO������룬λ0-63
							   ULONGLONG*	    GpioMLevel );    // GPIO��ƽ���룬����iGpioMIndexλ�ö�Ӧ��1���ߵ�ƽ��0���͵�ƽ
//�����õ�EEPROM��д��һ���ֽ���������
BOOL	WINAPI	CH348_WriteCfgEeprom_Byte(HANDLE        iPortH,     // ���ھ��ֵ
										  ChipPropertyS *ChipProperty,  // оƬ����ֵ
										  UCHAR			iAddr,      // ָ�����ݵ�Ԫ�ĵ�ַ
										  UCHAR			iByte );    // ��д����ֽ�����

//�����õ�EEPROM�ڶ���һ���ֽ���������
BOOL	WINAPI	CH348_ReadCfgEeprom_Byte(HANDLE         iPortH,    // ���ھ��ֵ	
										 ChipPropertyS  *ChipProperty,  // оƬ����ֵ
										 UCHAR			iAddr,     // ָ�����ݵ�Ԫ�ĵ�ַ
										 PUCHAR			oByte );   // ָ��һ���ֽڵ�Ԫ,���ڱ����ȡ���ֽ�����
//ͨ�����ڶ�ȡоƬEERPOM���豸��������
BOOL    WINAPI	 CH348_ReadDevConfig(
									 HANDLE          iPortH,     // ���ھ��ֵ
									 ChipPropertyS  *ChipProperty,  // оƬ����ֵ
									 PULONG          DataLen,    // Ҫ��ȡ�����ݳ���
									 PUCHAR          DataBuf);    // ���ݻ�����
//ͨ��������оƬEERPOM��д���豸����������,д��ǰ��ȷ��дλ����λ�ѹرա�
BOOL    WINAPI	 CH348_WriteDevConfig(
									  HANDLE          iPortH,				// ���ھ��ֵ
									  ChipPropertyS  *ChipProperty,  // оƬ����ֵ
									  ULONG           BufferSize,    // Ҫд������ݳ��� 
									  PUCHAR          DataBuf);       // ���ݻ�����
//����������оƬCH340/1
ULONG	WINAPI	CH341PtGetVersion( );  // ���DLL�汾��,���ذ汾��

BOOL	WINAPI	CH341PtHandleIsCH341(  // ����Ѿ��򿪵Ķ˿��Ƿ�ΪCH341�˿�
									 HANDLE		iPortHandle );  // ָ���Ѿ��򿪵Ķ˿ھ��


BOOL	WINAPI	CH341PtNameIsCH341(  // ���ָ�����ƵĶ˿��Ƿ�ΪCH341�˿�
								   PUCHAR		iPortName );  // ָ���˿�����,����"COM3","COM15"��,�ö˿ڱ���δ��ռ��(ָδ�����������)


typedef		VOID	( CALLBACK	* mPCH341PT_NOTIFY_ROUTINE ) (  // �˿��豸�¼�֪ͨ�ص�����
															  LONG			iDevIndexAndEvent );  // �˿��豸��ź��¼�����ǰ״̬(�ο�����˵��)
// iDevIndexAndEvent: ����˵�����豸�����¼�/�Ѿ�����, ����˵�����豸�γ��¼�/�Ѿ��γ�, �����ֵ���豸���(1��255)


BOOL	WINAPI	CH341PtSetDevNotify(  // �趨�˿��豸�¼�֪ͨ����
									PCHAR						iDeviceID,  // ��ѡ����,ָ����\0��ֹ���ַ���,ָ������ص��豸��ID���߲�����ŵ�������,���ڴ��ڸò�������ΪNULL����"COM"
									mPCH341PT_NOTIFY_ROUTINE	iNotifyRoutine );  // ָ���˿��豸�¼��ص�����,ΪNULL��ȡ���¼�֪ͨ,�����ڼ�⵽�¼�ʱ���øó���
																				   /* ���弴���豸��Ӧ�ó����̲ο�
																				   1. ������������Ĭ���ǽ�ֹ���ݴ���ģ�ֻ����ȷ����CH341�˿ڿ��ò��Ҵ򿪶˿ں�Ž������ݴ���
																				   2. ����CH341PtSetDevNotify���ò�μ��ӣ������������CH341�˿ڵĲ���¼�DLL�����Զ�����iNotifyRoutine��ָ�����ӳ�����ߺ���
																				   3. �Ӷ˿�1���˿�255��������20���ɣ��������CH341PtNameIsCH341ȷ���Ƿ�ΪCH341�˿ڣ�����������������˿ںŲ��򿪺�ʼ���䣬������ط�����Ϣ
																				   4. ���iNotifyRoutine�յ��¼�֪ͨ����ô�����ڱ����¼�������֪ͨ����������Ҳ�����ڸ��ӳ����д�����
																				   �����¼������������������˵����һ���˿ڲ��룬��Ҫ��ʱӦ��������CH341PtNameIsCH341�ж��Ƿ�ΪCH341�˿ڣ����������˿ںŲ��򿪺�ʼ���䣬
																				   ����Ǹ�����˵����һ���˿ڰγ����ж���˿ںţ���0��ȥ�ø����ö˿ںţ��Ƿ��������Ѵ򿪵Ķ˿ں���ͬ������Ӧ�ü�ʱ�ر�
																				   5. ��ǰ�Ѿ��򿪵Ķ˿ں�Ӧ�ñ���Ϊȫ�ֱ���, ����˿�δ�򿪻��߱��رգ���ôӦ�ó���Ӧ��ֹͣ���ݴ���
																				   6. �������δ��CH341PtNameIsCH341ȷ���Ƿ�ΪCH341�˿ڣ���ô�ڶ˿��Ѿ��򿪺�Ҳ���Ե���CH341PtHandleIsCH341�ж��Ƿ�ΪCH341�˿�
*/


//����ָ��ID��ȡ�豸����������Ϣ�����ش��豸�����Ƿ��Ѱ�װ
BOOL	WINAPI	CH343PT_GetUsbDevDrvInfor(USHORT PID,             //оƬUSB PID
										  BOOL   *DrvIsInstalled, //�Ƿ���Ҫ��װ����
										  PCHAR  DriverInfo);     //оƬ������Ϣ,��ΪNULL

BOOL	WINAPI	CH343PT_StartFwUpate(HANDLE			iPortH,        // ָ��CH341�豸���
									 ULONG			FwSize);       // �̼�����

//��ȡ�豸BCD��
BOOL	WINAPI	CH343PT_GetUsbComDevBcd(HANDLE			iPortH, 
                                        USHORT          *oDevBCD);

BOOL	WINAPI	CH343PT_GetStringDescr(  // ��ȡ���к��ַ���
									   HANDLE			iPortHandle,        // ָ��CH341�豸���
									   UCHAR            StringType,         //1:�����ַ���,2:��Ʒ�ַ���,3:���к��ַ���
									   USHORT           LangID,
									   PVOID			oBuffer,            // ָ��һ���㹻��Ļ�����,���ڱ���������
									   PULONG			ioLength ) ;         // ָ�򳤶ȵ�Ԫ,����ʱΪ׼����ȡ�ĳ���,���غ�Ϊʵ�ʶ�ȡ�ĳ���

/*
���ߴ���ISP�Զ����ط���--CH342/3
1.������Χ�����ܵ��߼����Ƶ�·��ʹ��USBת����оƬCH342/3������ʵ��MCU BOOT��APP��ģʽ��IO�����л��͹̼�����.
2.������Ϊ������������Ҫ��д����ͼ�����������CH340����Ϊ��Ӳ��������������ԭ������������ο���
3.�����˿ɼ��CH342/3��USB������Զ��������أ��γ��Զ��رն˿ڡ�
4.��ʵ��CH32xxxx/STM32F103�ȵ�Ƭ����ʱ����ƺʹ���ͨѶ��
5.֧�ִ˹��ܵ�USBת�����ͺ�:CH343G/CH343P/CH342F
/*
��������
1.�򿪴���
2.����MCU����ģʽ
CH343PT_SetMcuStartupMode
McuSeries: 103MCU/CH32xxxxMCU������ʽ:BOOT0�ߵ�ƽ��Ч��RST�͵�ƽ��Ч
PinCombMode:0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
3.��ϵͳ��(BOOT)����MCU
CH343PT_McuStartupFromSystem();
4.����ISP���غ�����������
5.��Flash������MCU�������û�����
CH343PT_McuStartupFromFlash();
6.�رմ���
*/

#define LEVEL_LOW    0
#define LEVEL_HIGH   1
#define IOFUNC_GPI   0
#define IOFUNC_GPO   1
/*
McuSeries: 103MCU/CH32xxxxMCU������ʽ:BOOT0�ߵ�ƽ��Ч��RST�͵�ƽ��Ч
PinCombMode:����������ϣ�0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
StartupMode	BOOT0	BOOT1	˵��
0         0	     X	    FLASH������(����APP����)
1         1	     0	    ϵͳ�洢������(����BOOT����,��������)
2         1	     1	    SRAM������
*/
//MCU����ģʽ����
UCHAR	WINAPI	WINAPI	CH343PT_SetMcuStartupMode(HANDLE PortH,     //�������
												  UCHAR McuSeries,  //MCUϵ�� 0:103 MCU,CH32xxxxϵ��оƬ������ʽ
												  UCHAR PinCombMode,
												  //����������ϣ�0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
												  //              1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
												  ULONG ResetHoldTime); //��λ����ʱ��
//��Flash������MCU
UCHAR	WINAPI	CH343PT_McuStartupFromFlash();
//��ϵͳ��(BOOT)����MCU
UCHAR	WINAPI	CH343PT_McuStartupFromSystem();
//��SRAM������MCU
UCHAR	WINAPI	CH343PT_McuStartupFromSram();

//����DTR/RTS�ڴ���δ��ʱ�ĵ�ƽ����ֵ��ֻ����������ʱ��Ч
BOOL	WINAPI	CH343PT_SetModemOutLevelOnClosed(HANDLE PortH,
												 BOOL   EnRtsKeepSetOnClosed, //TRUE:ΪTTL�͵�ƽ;FALSE:Ϊ�ߵ�ƽ��ΪĬ��ֵ
												 BOOL   EnDtrKeepSetOnClosed);//TRUE,ΪTTL�͵�ƽ;FALSE:Ϊ�ߵ�ƽ��ΪĬ��ֵ

/*
���ߴ���ISP�Զ�����Ӳ��������---CH340
CH340E/C֧��DTR��ǿģʽ������������Χ��·�Ķ�ģʽMCU����,���÷�ʽ������CH340DS1.PDF��5.3��
1.��·����ɿ��������ϵ硢���硢�򿪡��رյȲ�������Ӱ��MCU����ģʽ����ͳһ�����ص�·��Ҫ2�����������ʹ���Ա���BOOT��̬�͵�ƽ�Լ���ֹ�����ж���ʱMCU��λ�����BOOT����ģʽ��
2.����MCU���������������޸ġ�
3.֧��һ�����ء�����USB���Ȳ�����Ժ�CH343PT��ο⺯��������������ʵ�ֲ�����Զ����ء�
4.�ṩMSOP10��ESSOP10��SOP8�ȷ�װ�����ɶ��Ʒ�װ��
5.������ΪоƬӲ��������������ο�����������CH342/3������
*/
//����ǿ��չ
#define DTR_EXFUNC_NULL     0  
//CH340EоƬTNOW�Ÿ���DTR����̬�͵�ƽ������STM32Fxxx/CH32V/Fxxxϵ��MCU���ؿ��ƣ�������Χ��·
#define DTR_EXFUNC_MODE_L   1  
//CH340EоƬTNOW�Ÿ���DTR/CH340C OUT�Ÿ���DTR����̬�ߵ�ƽ������STM32Fxxx/CH32V/Fxxxϵ��MCU���ؿ��ƣ�������Χ��·
#define DTR_EXFUNC_MODE_H   2
//��ȡоƬDTR����ģʽ,CH340E/C֧��DTR��ǿģʽ������������Χ��·�Ķ�ģʽMCU���ء�
BOOL	WINAPI	CH343PT_GetDTRMode(HANDLE			iPortH,          // ���ھ��
							       UCHAR			*DTRExFuncMode); // ģʽDTR_EXFUNC_xxx

#ifdef __cplusplus
}
#endif

#endif		// _CH343_PORT_H