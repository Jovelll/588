///////////////////////////////////////////////////////////////////////////////
//
//	File name: drvScsi.h
//	Version: 1.0
//	Date: 2003/7/13
//  Description:
//
//
//	Author:
//	Email:
//	Phone: (03) 578-7888
//	Company: Faraday Tech. Corp.
///////////////////////////////////////////////////////////////////////////////
#ifndef SCSI_H
#define SCSI_H



/*
 * This header file contains public constants and structures used by
 * the scsi code for linux.
 */
//=================== 1.Condition Definition  ============================================================
//========================================================================================================
#define ScsiCmd_Fail_Retry		3
#define TestUnitReady_Fail_Retry  0x3
#define Scsi_Max_Transfer_Len    16384 //65536;;yuwen
#define SCSI_DbgPrint         printf
//#define printk                  printf
//=================== 2.Define Data Type  ================================================================
//========================================================================================================

#ifndef BOOLEAN
#define BOOLEAN    UINT8
#endif
#define INT32U     UINT32
#define INT16U     UINT16
#define INT8U      UINT8
/*
 *      SCSI opcodes
 */

#define TEST_UNIT_READY			0x00
#define REQUEST_SENSE			0x03
#define FORMAT_UNIT				0x04		//Option (unsupport)
#define INQUIRY               	0x12
#define MODE_SELECT				0x15		//(unsupport)
#define RESERVE					0x16		//Option (unsupport)
#define RELEASE					0x17		//Option (unsupport)
#define MODE_SENSE				0x1a
#define START_STOP				0x1b		// (unsupport)
#define ALLOW_MEDIUM_REMOVAL	0x1e
#define READ_CAPACITY			0x25
#define READ_FORMAT_CAPACITY	0x23
#define READ_10					0x28
#define WRITE_10				0x2a
#define VERIFY					0x2f		//(unsupport)
#define SYNCHRONIZE_CACHE		0x35		//Option (unsupport)
#define WRITE_BUFFER			0x3b		//Option (unsupport)
#define PERSISTENT_RESERVE_IN	0x5e		//Option (unsupport)
#define PERSISTENT_RESERVE_OUT	0x5f		//Option (unsupport)

#define SEND_DIAGNOSTIC		0x1d		//(unsupport)


/*
 *  Status codes
 */

#define GOOD					0x00
#define CHECK_CONDITION		0x01
#define CONDITION_GOOD			0x02
#define BUSY						0x04
#define INTERMEDIATE_GOOD		0x08
#define INTERMEDIATE_C_GOOD	0x0a
#define RESERVATION_CONFLICT	0x0c
#define COMMAND_TERMINATED	0x11
#define QUEUE_FULL				0x14

#define STATUS_MASK				0x3e

/*
 *  SENSE KEYS
 */

#define NO_SENSE				0x00
#define RECOVERED_ERROR		0x01
#define NOT_READY				0x02
#define MEDIUM_ERROR			0x03
#define HARDWARE_ERROR		0x04
#define ILLEGAL_REQUEST			0x05
#define UNIT_ATTENTION			0x06
#define DATA_PROTECT			0x07
#define BLANK_CHECK			0x08
#define COPY_ABORTED			0x0a
#define ABORTED_COMMAND		0x0b
#define VOLUME_OVERFLOW		0x0d
#define MISCOMPARE				0x0e


/*
 *  DEVICE TYPES
 */

#define TYPE_DISK           0x00
#define TYPE_TAPE           0x01
#define TYPE_PRINTER        0x02
#define TYPE_PROCESSOR      0x03    /* HP scanners use this */
#define TYPE_WORM           0x04    /* Treated as ROM by our system */
#define TYPE_ROM            0x05
#define TYPE_SCANNER        0x06
#define TYPE_MOD            0x07    /* Magneto-optical disk -
				     * - treated as TYPE_DISK */
#define TYPE_MEDIUM_CHANGER 0x08
#define TYPE_COMM           0x09    /* Communications device */
#define TYPE_ENCLOSURE      0x0d    /* Enclosure Services Device */
#define TYPE_NO_LUN         0x7f

/*
 *  MESSAGE CODES
 */

#define COMMAND_COMPLETE    0x00
#define EXTENDED_MESSAGE    0x01
#define EXTENDED_MODIFY_DATA_POINTER    0x00
#define EXTENDED_SDTR                   0x01
#define EXTENDED_EXTENDED_IDENTIFY      0x02    /* SCSI-I only */
#define EXTENDED_WDTR                   0x03
#define SAVE_POINTERS       0x02
#define RESTORE_POINTERS    0x03
#define DISCONNECT          0x04
#define INITIATOR_ERROR     0x05
#define ABORT               0x06
#define MESSAGE_REJECT      0x07
#define NOP                 0x08
#define MSG_PARITY_ERROR    0x09
#define LINKED_CMD_COMPLETE 0x0a
#define LINKED_FLG_CMD_COMPLETE 0x0b
#define BUS_DEVICE_RESET    0x0c

#define INITIATE_RECOVERY   0x0f            /* SCSI-II only */
#define RELEASE_RECOVERY    0x10            /* SCSI-II only */

#define SIMPLE_QUEUE_TAG    0x20
#define HEAD_OF_QUEUE_TAG   0x21
#define ORDERED_QUEUE_TAG   0x22

/*
 * Here are some scsi specific ioctl commands which are sometimes useful.
 */
/* These are a few other constants  only used by scsi  devices */
/* Note that include/linux/cdrom.h also defines IOCTL 0x5300 - 0x5395 */

#define SCSI_IOCTL_GET_IDLUN		0x5382	/* conflicts with CDROMAUDIOBUFSIZ */

/* Used to turn on and off tagged queuing for scsi devices */

#define SCSI_IOCTL_TAGGED_ENABLE	0x5383
#define SCSI_IOCTL_TAGGED_DISABLE	0x5384

/* Used to obtain the host number of a device. */
#define SCSI_IOCTL_PROBE_HOST		0x5385

/* Used to get the bus number for a device */
#define SCSI_IOCTL_GET_BUS_NUMBER	0x5386

/* Used to get the PCI location of a device */
#define SCSI_IOCTL_GET_PCI			0x5387

/*
 *  Use these to separate status msg and our bytes
 *
 *  These are set by:
 *
 *      status byte = set from target device
 *      msg_byte    = return status from host adapter itself.
 *      host_byte   = set by low-level driver to indicate status.
 *      driver_byte = set by mid-level.
 */
#define status_byte(result) (((result) >> 1) & 0x1f)
#define msg_byte(result)    (((result) >> 8) & 0xff)
#define host_byte(result)   (((result) >> 16) & 0xff)
#define driver_byte(result) (((result) >> 24) & 0xff)
#define suggestion(result)  (driver_byte(result) & SUGGEST_MASK)

#define sense_class(sense)  (((sense) >> 4) & 0x7)
#define sense_error(sense)  ((sense) & 0xf)
#define sense_valid(sense)  ((sense) & 0x80);

#define NEEDS_RETRY     0x2001
#define SUCCESS         0x2002
#define FAILED          0x2003
#define QUEUED          0x2004
#define SOFT_ERROR      0x2005
#define ADD_TO_MLQUEUE  0x2006

#define IDENTIFY_BASE       0x80
#define IDENTIFY(can_disconnect, lun)   (IDENTIFY_BASE |\
		     ((can_disconnect) ?  0x40 : 0) |\
		     ((lun) & 0x07))



/*
 * FIXME(eric) - one of the great regrets that I have is that I failed to define
 * these structure elements as something like sc_foo instead of foo.  This would
 * make it so much easier to grep through sources and so forth.  I propose that
 * all new elements that get added to these structures follow this convention.
 * As time goes on and as people have the stomach for it, it should be possible to
 * go back and retrofit at least some of the elements here with with the prefix.
 */
 #define MAX_COMMAND_SIZE	16
 #define SCSI_SENSE_BUFFERSIZE 	64

/*
 *  the return of the status word will be in the following format :
 *  The low byte is the status returned by the SCSI command,
 *  with vendor specific bits masked.
 *
 *  The next byte is the message which followed the SCSI status.
 *  This allows a stos to be used, since the Intel is a little
 *  endian machine.
 *
 *  The final byte is a host return code, which is one of the following.
 *
 *  IE
 *  lsb     msb
 *  status  msg host code
 *
 *  Our errors returned by OUR driver, NOT SCSI message.  Or'd with
 *  SCSI message passed back to driver <IF any>.
 */


#define DID_OK          0x00	/* NO error                                */
#define DID_NO_CONNECT  0x01	/* Couldn't connect before timeout period  */
#define DID_BUS_BUSY    0x02	/* BUS stayed busy through time out period */
#define DID_TIME_OUT    0x03	/* TIMED OUT for other reason              */
#define DID_BAD_TARGET  0x04	/* BAD target.                             */
#define DID_ABORT       0x05	/* Told to abort for some other reason     */
#define DID_PARITY      0x06	/* Parity error                            */
#define DID_ERROR       0x07	/* Internal error                          */
#define DID_RESET       0x08	/* Reset by somebody.                      */
#define DID_BAD_INTR    0x09	/* Got an interrupt we weren't expecting.  */
#define DID_PASSTHROUGH 0x0a	/* Force command past mid-layer            */
#define DID_SOFT_ERROR  0x0b	/* The low level driver just wish a retry  */
#define DRIVER_OK       0x00	/* Driver status                           */

/*
 * These are the values that the SCpnt->sc_data_direction and
 * SRpnt->sr_data_direction can take.  These need to be set
 * The SCSI_DATA_UNKNOWN value is essentially the default.
 * In the event that the command creator didn't bother to
 * set a value, you will see SCSI_DATA_UNKNOWN.
 */
#define SCSI_DATA_UNKNOWN       0
#define SCSI_DATA_WRITE         1
#define SCSI_DATA_READ          2
#define SCSI_DATA_NONE          3

/*
 *  SCSI Architecture Model (SAM) Status codes. Taken from SAM-3 draft
 *  T10/1561-D Revision 4 Draft dated 7th November 2002.
 */
#define SAM_STAT_GOOD            0x00
#define SAM_STAT_CHECK_CONDITION 0x02
#define SAM_STAT_CONDITION_MET   0x04
#define SAM_STAT_BUSY            0x08
#define SAM_STAT_INTERMEDIATE    0x10
#define SAM_STAT_INTERMEDIATE_CONDITION_MET 0x14
#define SAM_STAT_RESERVATION_CONFLICT 0x18
#define SAM_STAT_COMMAND_TERMINATED 0x22	/* obsolete in SAM-3 */
#define SAM_STAT_TASK_SET_FULL   0x28
#define SAM_STAT_ACA_ACTIVE      0x30
#define SAM_STAT_TASK_ABORTED    0x40


/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-indent-level: 4
 * c-brace-imaginary-offset: 0
 * c-brace-offset: -4
 * c-argdecl-indent: 4
 * c-label-offset: -4
 * c-continued-statement-offset: 4
 * c-continued-brace-offset: 0
 * indent-tabs-mode: nil
 * tab-width: 8
 * End:
 */
#define CBW_TAG							0x01020304		// temp

#define CBW_SIGNATE					0x43425355
#define CSW_SIGNATE						0x53425355
#define CSW_STATUS_CMD_PASS			0x00
#define CSW_STATUS_CMD_FAIL			0x01
#define CSW_STATUS_PHASE_ERROR		0x02

#define CBW_FLAG_IN					0x80
#define CBW_FLAG_OUT					0x00

#define CB_LENGTH_TEST_UNIT_READY			0x06
#define CB_LENGTH_REQUEST_SENSE			0x0C
#define CB_LENGTH_INQUIRY               			0x06
#define CB_LENGTH_MODE_SENSE				0x06
//#define CB_LENGTH_ALLOW_MEDIUM_REMOVAL	0x1e
#define CB_LENGTH_READ_CAPACITY			0x0A
#define CB_LENGTH_READ_10					0x0A
#define CB_LENGTH_WRITE_10					0x0A



#define DATA_LENGTH_INQUIRY			36
#define DATA_LENGTH_MODE_SENSE		32 //192 , yuwen
#define DATA_LENGTH_REQUEST_SENSE		18
#define DATA_LENGTH_READ_CAPACITY		8






//=================== 3.Structure Definition =============================================================
//========================================================================================================


	//typedef struct CommandBlockWrapper
	//{
	//	UINT32 u32Signature;
	//	INT32U u32Tag;
	//	INT32U u32DataTransferLength;
	//	INT8U u8Flags;
	//	INT8U u8LUN;
	//	INT8U u8CBLength;
	//	INT8U u8CB[16];
	//} CBW;

	//typedef struct CommandStatusWrapper
	//{
	//	INT32U u32Signature;
	//	INT32U u32Tag;
	//	INT32U u32DataResidue;
	//	INT8U u8Status;
	//} CSW;

	typedef enum
	{
		SCSI_OPSTATE_TEST_UNIT_READY = 0,
		SCSI_OPSTATE_REQUEST_SENSE,
		SCSI_OPSTATE_INQUIRY,
		SCSI_OPSTATE_MODE_SELECT,
		SCSI_OPSTATE_MODE_SENSE,
		SCSI_OPSTATE_ALLOW_MEDIUM_REMOVAL,
		SCSI_OPSTATE_READ_CAPACITY,
		SCSI_OPSTATE_READ_10,
		SCSI_OPSTATE_WRITE_10
	} ScsiOpState;


	typedef struct ScsiDevice
	{
		// Get by INQUIRY command
		UINT8 u8DeviceType;
		//UINT8 u8VendorID[8];      //Colin, moved to LUN device
		//UINT8 u8ProductID[16];
		//UINT8 u8ProductVer[4];
	} Scsi_Device;

//	extern Scsi_Device tScsi_Device;
#define BUFFER_LOGI  0
#define BUFFER_PHY	1
struct scsi_cmnd
{

	unsigned char lun;				//no use??
//	unsigned int channel;				//no use
	unsigned char cmd_len;
//	unsigned char old_cmd_len;				//no use
	unsigned char sc_data_direction;
//	unsigned char sc_old_data_direction;				//no use

	/* These elements define the operation we are about to perform */
//    unsigned char cmnd[MAX_COMMAND_SIZE];
	U32 request_bufflen;				/* Actual request size */

//	struct timer_list eh_timeout;			/* Used to time out the command. */
	U8 *request_buffer;					/* Actual requested buffer */
    U32 phy_buf_adr;
	U8  buffer_type;
	/* These elements define the operation we ultimately want to perform */
//	unsigned char data_cmnd[MAX_COMMAND_SIZE];				//no use
//	unsigned short old_use_sg;				//no use
										/* We save  use_sg here when requesting
					 					* sense info */
	//unsigned short use_sg;				/* Number of pieces of scatter-gather */
//	unsigned short sglist_len;					//no use
										/* size of malloc'd scatter-gather list */
//	unsigned short abort_reason;				//no use
										/* If the mid-level code requests an
					 					* abort, this is the reason. */
//	unsigned bufflen;						//no use
										/* Size of data buffer */
//	void *buffer;							//no use
										/* Data buffer */

//	unsigned underflow;					//no use
										/* Return error if less than
				   						this amount is transferred */
//	unsigned old_underflow;				//no use
										/* save underflow here when reusing the
				 						* command for error handling */

//	unsigned transfersize;					//no use
									/* How much we are guaranteed to
				   						transfer with each SCSI transfer
				   						(ie, between disconnect /
				   						reconnects.   Probably == sector
				   						size */

	//int resid;
									/* Number of bytes requested to be
				  						transferred less actual number
				   						transferred (0 if not supported) */

//	struct request *request;				//no use
									/* The command we are
				   	   					working on */

	//unsigned char sense_buffer[SCSI_SENSE_BUFFERSIZE];		/* obtained by REQUEST SENSE
		//				 								* when CHECK CONDITION is
		//				 								* received on original command
		//				 								* (auto-sense) */

	//unsigned flags;

	/*
	 * Used to indicate that a command which has timed out also
	 * completed normally.  Typically the completion function will
	 * do nothing but set this flag in this instance because the
	 * timeout handler is already running.
	 */
//	unsigned done_late:1;				//no use

	/* Low-level done function - can be used by low-level driver to point
	 *        to completion function.  Not used by mid/upper level code. */
	//void (*scsi_done) (struct scsi_cmnd *);

	/*
	 * The following fields can be written to by the host specific code.
	 * Everything else should be left alone.
	 */

//	Scsi_Pointer SCp;					/* Scratchpad used by some host adapters */

//	unsigned char *host_scribble;				//no use
									/* The host adapter is allowed to
									* call scsi_malloc and get some memory
									* and hang it here.     The host adapter
									* is also expected to call scsi_free
									* to release this memory.  (The memory
									* obtained by scsi_malloc is guaranteed
									* to be at an address < 16Mb). */

	U8 result;						/* Status code from lower level driver */

	unsigned char tag;					/* SCSI-II queued command tag */
//	unsigned long pid;					/* Process ID, starts at 0 */
};

typedef struct scsi_cmnd Scsi_Cmnd;


struct LUN_Device
{
	UINT8 u8LunNum;
	/* file system : YPING add */
//	FAT_FileSysStruct FileSys;

	// Get by READ CAPACITY command
	UINT32 u32BlockSize;
	UINT32 u32BlockTotalNum;

	// Get by TEST UNIT READY command
	UINT8 bDeviceReady;
       UINT8 bDeviceValid;
	// Save Write Protection information
	UINT8 bWriteProtect;

	UINT8 u8VendorID[8];
	UINT8 u8ProductID[16];
	UINT8 u8ProductVer[4];
};


struct us_data {

	Scsi_Cmnd	    *srb;		 /* current srb		*/
	Scsi_Device 	device;
	UINT8			max_lun;
	// For one Device but have many LUN
	BOOLEAN bFileSystemInit;
	struct LUN_Device* Mass_stor_device;
};



//=================== 4.Extern Function Definition =======================================================
//========================================================================================================

//extern void vSCSI_REQUEST_SENSE(void);
extern BOOLEAN vSCSI_REQUEST_SENSE_Port2(U8);
extern BOOLEAN bSCSI_INQUIRY_Port2(U8);
extern BOOLEAN bSCSI_READ_CAPACITY_Port2(U8);
extern BOOLEAN bSCSI_TEST_UNIT_READY_Port2(U8);

extern BOOLEAN bInit_USB_LUN_Port2(U8);


extern BOOLEAN bSCSI_Initial_Port2(void);

extern BOOLEAN bSCSI_ERASE( struct LUN_Device* LunDevice, UINT32 u32BlockAddr, UINT32 u32BlockNum,
							UINT32 u32SectorSize);
//extern BOOLEAN bSCSI_Read_10(U8 lun,  UINT32 u32BlockAddr, UINT32 u32BlockNum,UINT32 u8Buffer);
//extern BOOLEAN bSCSI_Write_10(U8 lun, UINT32 u32BlockAddr, UINT32 u32BlockNum, UINT32 u8Buffer);
extern BOOLEAN bSCSI_Read_Write10_Port2(BOOLEAN DoWrite, U8 lun,  INT32U u32BlockAddr, INT32U u32BlockNum,U32 u8Buffer);
extern BOOLEAN  vSCSI_EJECT_DEVICE_Port2(U8 lun);

extern BOOLEAN bSCSI_MODE_SENSE_Port2(U8);
//=================== 5.Extern Variable Definition =======================================================
//========================================================================================================
extern struct us_data Mass_stor_us1_port2;

//extern struct us_data *Mass_stor_us;
extern struct us_data us_data_Signal;
//extern INT8U u8Drive;
extern struct LUN_Device Mass_stor_device_1_port2;






#endif

