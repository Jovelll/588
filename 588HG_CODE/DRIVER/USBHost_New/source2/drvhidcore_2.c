#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT



#include <stdio.h>
#include <string.h>
#include "USBHostConfig_2.h"

#include "UsbHostDataDef.h"
#include "drvUsbMain_2.h"
//#include "sysinfo.h"
//#include "drvtimer.h"
//#include "R8051XC.h"
#include "drvusb.h"
//#include "msAPI_Timer.h"
#include "drvhid_2.h"
#include "drvhostlib_2.h"
#include "common.h"

#define DrvHidCore2_DEBUG    1
#if ENABLE_DEBUG && DrvHidCore2_DEBUG
    #define DrvHidCore2_printData(str, value)   printData(str, value)
    #define DrvHidCore2_printMsg(str)           printMsg(str)
#else
    #define DrvHidCore2_printData(str, value)
    #define DrvHidCore2_printMsg(str)
#endif

#define DRV_HID_DBG(x)  x;

#define STR_C51_VERSION(x) "C51:"##  #x

#define C51_VERSION(x) STR_C51_VERSION(x)
char* xdata usb_hid_p2_c51version = C51_VERSION(__C51__);

U8 code USBHID_P2_VBuf[32] = {'M', 'S', 'V', 'C', '0', '0',             // 0, 0 fixed
                        'U', '3',                                   // Library ID
#ifdef   DTV_STANDARD_LIB
                        'A', '0', '0', '1', '0', '2',               // build number
#endif
                        '0', '0', '0', '0', '0', '0', '0', '0',     // change list 46058
                        '0', '0', '0', '0', '0', '0', '0', '0', '0',// A4: Saturn, A:LG
                        'T'};

extern void MDrv_USB_SetXdataWindow1_Port2(void);
extern void MDrv_USB_ReleaseXdataWindow1_Port2(void);

/*
 * Read data value from item.
 */
extern UINT8 xdata pUsbData_Port2[512];
struct   axis_struct  xdata  axis_Port2[HOST20_INTERFACE_NUM_MAX][MAX_AXIS_NUM];
__u8    xdata axis_idx_Port2[HOST20_INTERFACE_NUM_MAX];
struct   button_struct  xdata buttons_Port2[HOST20_INTERFACE_NUM_MAX][MAX_BUTTON_NUM];
__u8    xdata button_idx_Port2[HOST20_INTERFACE_NUM_MAX];
struct   leds_struct  xdata leds_Port2[HOST20_INTERFACE_NUM_MAX];
__u8     kbd_report_cnt_Port2[HOST20_INTERFACE_NUM_MAX];
__u8     Get_Report_ID_Port2[HOST20_INTERFACE_NUM_MAX];

__u16  xdata gOffsetBit_Port2[HOST20_INTERFACE_NUM_MAX];
UINT8 xdata gState_Change_Port2[HOST20_INTERFACE_NUM_MAX];
UINT8 usage_port2_temp;
long logical_max_port2,logical_min_port2;
long report_size_port2,mouse_size_port2;
struct mouse_struct xdata sMouseReport_Port2;

__u32 item_udata_Port2(struct hid_item *item)
{
//    DO_PRINT("@@@ >>> Enter hid-core.c file --> item_udata function \n");

    switch (item->size) {
        case 1: return item->zdata.u8;
        case 2: return item->zdata.u16;
        case 4: return item->zdata.u32;
    }
    return 0;
}

static __s32 item_sdata_Port2(struct hid_item *item)
{
    //DO_PRINT("@@@ >>> Enter hid-core.c file --> item_sdata function \n");

    switch (item->size) {
        case 1: return item->zdata.s8;
        case 2: return item->zdata.s16;
        case 4: return item->zdata.s32;
    }
    return 0;
}


/*
 * Parse a report description into a hid_device structure. Reports are
 * enumerated, fields are attached to these reports.
 */


static __u8 *fetch_item_Port2(__u8 *start, __u8 *end, struct hid_item *item)
{
    //printf("@@@ >>> Enter hid-core.c file --> fetch_item function \n");

    if ((end - start) > 0) {

        __u8 b = *start++;
        //DrvHidCore2_printData("b:%x ",b);
        item->type = (b >> 2) & 3;
        item->tag  = (b >> 4) & 15;
        //DrvHidCore2_printData("tag:%x ",item->tag);

        if (item->tag == HID_ITEM_TAG_LONG) {

            item->format = HID_ITEM_FORMAT_LONG;

            if ((end - start) >= 2) {

                item->size = *start++;
                item->tag  = *start++;

                if ((end - start) >= item->size) {
                    item->zdata.longdata = start;
                    start += item->size;
                    return start;
                }
            }
        } else {

            item->format = HID_ITEM_FORMAT_SHORT;
            item->size = b & 3;
            switch (item->size) {

                case 0:
                    return start;

                case 1:
                    if ((end - start) >= 1) {
                        item->zdata.u8 = *start++;
                                          //printf("U8:%02bx\n",item->zdata.u8);
                                        //  printf("return %x\n",(U16)start);

                        return start;
                    }
                    break;

                case 2:
                    if ((end - start) >= 2) {
                                          item->zdata.u16 = (*start)+ (*(start+1)<<8);
                                          start+=2;
                        return start;
                    }

                case 3:
                    item->size++;
                    if ((end - start) >= 4) {
                                          item->zdata.u16 = (*start)+ (*(start+1)<<8)+(*(start+2)<<16) +(*(start+3)<<24);
                                          start+=4;
                        return start;
                    }
            }
        }
    }
    return NULL;
}

static int open_collection_Port2(struct hid_parser *parser, unsigned type)
{
    struct hid_collection xdata *collection;
    unsigned xdata usage;

    usage = parser->local.usage[0];

    if (parser->collection_stack_ptr == HID_COLLECTION_STACK_SIZE) {
        DrvHidCore2_printMsg("collection stack overflow\n");
        return -1;
    }

    collection = parser->collection_stack + parser->collection_stack_ptr++;
    collection->type = type;
    collection->usage = usage;

    return 0;
}

static int close_collection_Port2(struct hid_parser *parser)
{
    if (!parser->collection_stack_ptr) {
        DrvHidCore2_printMsg("collection stack underflow\n");
        return -1;
    }
    parser->collection_stack_ptr--;
    return 0;
}

static int hid_chk_variable_Port2(struct hid_parser *parser, U8 report_type, U8 u8Interface)
{
    U8  xdata ii, usg_count, rpt_count;

    usg_count = parser->local.usage_index;
    rpt_count = parser->global.report_count;

#if 0
    DrvHidCore2_printData("hid_chk_variable: type: %x\n", report_type);
    DrvHidCore2_printData("usage count: %x", usg_count);
    DrvHidCore2_printData(", size: %x", parser->global.report_size);
    DrvHidCore2_printData(", count: %x\n", rpt_count);
#endif
    if (parser->global.usage_page == USEAGE_PAGE_BUTTON)
    {
        buttons_Port2[u8Interface][button_idx_Port2[u8Interface]].startbit=gOffsetBit_Port2[u8Interface];
        buttons_Port2[u8Interface][button_idx_Port2[u8Interface]].count=parser->global.report_size * rpt_count;
        DrvHidCore2_printData("Usage Page (buttons): %x size: ", buttons_Port2[u8Interface][button_idx_Port2[u8Interface]].startbit);
        DrvHidCore2_printData("%x\n", buttons_Port2[u8Interface][button_idx_Port2[u8Interface]].count);

        gOffsetBit_Port2[u8Interface]+=buttons_Port2[u8Interface][button_idx_Port2[u8Interface]].count;
        button_idx_Port2[u8Interface]++;
        goto Func_Done;
    }
    else if (parser->global.usage_page == USEAGE_PAGE_KEYBOARD)
    {
        kbd_report_cnt_Port2[u8Interface]+=parser->global.report_size * rpt_count;
        DrvHidCore2_printData("Usage Page (keyboard): size: %x ", kbd_report_cnt_Port2[u8Interface]);
        goto Func_Done;
    }
    else if (parser->global.usage_page == USEAGE_PAGE_LEDS)
    {
        leds_Port2[u8Interface].count=parser->global.report_size * rpt_count;
        DrvHidCore2_printData("Usage Page (leds): size %x", leds_Port2[u8Interface].count);
        goto Func_Done;
    }


    if (usg_count > 0)
    {
        for(ii=usg_count-rpt_count; ii<usg_count; ii++)
        {
            DrvHidCore2_printData("local usage %x: ", ii);
            DrvHidCore2_printData("%x\n", parser->local.usage[ii]);
            if (report_type == HID_INPUT_REPORT)
            {
                if (parser->local.usage[ii] == USEAGE_X)
                {
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].startbit=gOffsetBit_Port2[u8Interface];
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size=parser->global.report_size;
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].type=TYPE_AXIS_X;
                     gOffsetBit_Port2[u8Interface]+=axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size;
                     DrvHidCore2_printData("Usage(X): %x",axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].startbit);
                     DrvHidCore2_printData(", size: %x\n", axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size);
                     axis_idx_Port2[u8Interface]++;
                }
                else if (parser->local.usage[ii] == USEAGE_Y)
                {
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].startbit=gOffsetBit_Port2[u8Interface];
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size=parser->global.report_size;
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].type=TYPE_AXIS_Y;
                     gOffsetBit_Port2[u8Interface]+=axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size;
                     DrvHidCore2_printData("Usage(Y): %x",axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].startbit);
                     DrvHidCore2_printData(", size: %x\n", axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size);
                     axis_idx_Port2[u8Interface]++;
                     mouse_size_port2=report_size_port2;
                     DrvHidCore2_printData("report_size:%x\n",mouse_size_port2);                     
                }
                else if (parser->local.usage[ii] == USEAGE_WHEEL)
                {
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].startbit=gOffsetBit_Port2[u8Interface];
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size=parser->global.report_size;
                     axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].type=TYPE_AXIS_WHEEL;
                     gOffsetBit_Port2[u8Interface]+=axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size;
                     DrvHidCore2_printData("Usage(WHEEL): %x",axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].startbit);
                     DrvHidCore2_printData(", size: %x\n", axis_Port2[u8Interface][axis_idx_Port2[u8Interface]].size);
                     axis_idx_Port2[u8Interface]++;
                }
                else
                {
                    DrvHidCore2_printData("Unsupport usage: %x", gOffsetBit_Port2[u8Interface]);
                    DrvHidCore2_printData(", size: %x\n", parser->global.report_size);
                    gOffsetBit_Port2[u8Interface]+= parser->global.report_size;
                }
            }
            else
            {
                DrvHidCore2_printData("Unsupport usage: %x", gOffsetBit_Port2[u8Interface]);
                DrvHidCore2_printData(", size: %x\n", parser->global.report_size);
                gOffsetBit_Port2[u8Interface]+= parser->global.report_size;
            }
        }
    }
    else
    {
        DrvHidCore2_printData("Unsupport data: %x", gOffsetBit_Port2[u8Interface]);
        DrvHidCore2_printData(", size: %x\n", parser->global.report_size * rpt_count);
        gOffsetBit_Port2[u8Interface]+= parser->global.report_size * rpt_count;
    }

Func_Done:
    return 0;
}

/*
 * Process a main item.
 */
 int hid_parser_main_Port2(struct hid_parser *parser, struct hid_item *item, U8 u8Interface)
{
    __u32 xdata adata;
    int xdata ret=0;

    //DrvHidCore2_printMsg("hid_parser_main_2");

    adata = item_udata_Port2(item);
      // DrvHidCore2_printData("TAG:%02bx\n",item->tag);
    switch (item->tag) {
        case HID_MAIN_ITEM_TAG_BEGIN_COLLECTION:
#if 1
                     if (adata==HID_COLLECTION_APPLICATION)
                        {
                            DrvHidCore2_printMsg("application\n");
                            #if 0
                            gOffsetBit=0;
                            axis_idx=0;
                            for (i=0 ; i < MAX_AXIS_NUM ; i ++)
                            {
                                axis[i].size=0;
                                axis[i].val=0;
                            }
                            #endif
                        }
#endif
            ret = open_collection_Port2(parser, adata & 3);
            break;
        case HID_MAIN_ITEM_TAG_END_COLLECTION:
            ret = close_collection_Port2(parser);
            break;
        case HID_MAIN_ITEM_TAG_INPUT:
            ret = hid_chk_variable_Port2(parser, HID_INPUT_REPORT, u8Interface);
            //ret = hid_add_field(parser, HID_INPUT_REPORT, data);
            break;
        case HID_MAIN_ITEM_TAG_OUTPUT:
            ret = hid_chk_variable_Port2(parser, HID_OUTPUT_REPORT, u8Interface);
            //ret = hid_add_field(parser, HID_OUTPUT_REPORT, data);
            break;
        case HID_MAIN_ITEM_TAG_FEATURE:
            ret = hid_chk_variable_Port2(parser, HID_FEATURE_REPORT, u8Interface);
        //  ret = hid_add_field(parser, HID_FEATURE_REPORT, data);
            break;
        default:
            //DrvHidCore2_printData("unknown main item tag 0x%02bx", item->tag);
            ret = 0;
    }

    memset(&parser->local, 0, sizeof(parser->local));   /* Reset the local parser environment */

    return ret;
}


/*
 * Process a global item.
 */

 int hid_parser_global_Port2(struct hid_parser *parser, struct hid_item *item, U8 u8Interface)
{
    //DrvHidCore2_printMsg("hid_parser_global_2");
    switch (item->tag) {

        case HID_GLOBAL_ITEM_TAG_PUSH:

            //if (parser->global_stack_ptr == HID_GLOBAL_STACK_SIZE) {
            //  printf("global enviroment stack overflow");
                //return -1;
            //}

            //memcpy(parser->global_stack + parser->global_stack_ptr++,
                //&parser->global, sizeof(struct hid_global));
            return 0;

        case HID_GLOBAL_ITEM_TAG_POP:

            //if (!parser->global_stack_ptr) {
            //  printf("global enviroment stack underflow");
            //  return -1;
            //}

            //memcpy(&parser->global, parser->global_stack + --parser->global_stack_ptr,
                //sizeof(struct hid_global));
            return 0;

        case HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
            parser->global.usage_page = item_udata_Port2(item);

                    DrvHidCore2_printData("Find a usage page: %x\n", parser->global.usage_page);
#if 0
                     if (parser->global.usage_page==USEAGE_PAGE_BUTTON)
                        {
                            buttons.startbit=gOffsetBit;
                            DrvHidCore2_printData("buttons start:%x\n",buttons.startbit);
                            buttons.count=parser->global.report_count;
                            DrvHidCore2_printData("buttons count:%02bx\n",buttons.count);

                            gOffsetBit+=buttons.count;
                        }
#endif
            return 0;

        case HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
            parser->global.logical_minimum = item_sdata_Port2(item);
            if(usage_port2_temp==48 ||usage_port2_temp==49)
            {
                logical_min_port2=parser->global.logical_minimum;             
            }      
            else if(usage_port2_temp==56)
            {
                if(logical_min_port2==0)
                    logical_min_port2=parser->global.logical_minimum;    
            }
            DrvHidCore2_printData("usage:%d\n",usage_port2_temp);
            DrvHidCore2_printData("@@logical_minimum:%x\n",parser->global.logical_minimum);
            DrvHidCore2_printData("logical_min:%x\n",logical_min_port2);            
            return 0;

        case HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
            if (parser->global.logical_minimum < 0)
                parser->global.logical_maximum = item_sdata_Port2(item);
            else
                parser->global.logical_maximum = item_udata_Port2(item);

            if(usage_port2_temp==48 ||usage_port2_temp==49)
            {
                logical_max_port2=parser->global.logical_maximum;             
            } 
            else if(usage_port2_temp==56)
            {
                if(logical_max_port2==0)
                    logical_max_port2=parser->global.logical_maximum;  
            }
            DrvHidCore2_printData("usage:%d\n",usage_port2_temp);
            DrvHidCore2_printData("@@logical_maximum:%x\n",parser->global.logical_maximum);
            DrvHidCore2_printData("logical_max:%x\n",logical_max_port2);              
            return 0;

        case HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM:
            parser->global.physical_minimum = item_sdata_Port2(item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM:
            if (parser->global.physical_minimum < 0)
                parser->global.physical_maximum = item_sdata_Port2(item);
            else
                parser->global.physical_maximum = item_udata_Port2(item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT:
            parser->global.unit_exponent = item_udata_Port2(item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_UNIT:
            parser->global.unit = item_udata_Port2(item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
            DrvHidCore2_printData("##Report size:%x\n",item_udata_Port2(item));  
            report_size_port2=item_udata_Port2(item);
            #if 0
            if(usage_port2_temp==48 ||usage_port2_temp==49)
            {
                report_size_port2=item_udata_Port2(item);
                DrvHidCore2_printData("@@Report size:%x\n",report_size_port2); 
            }
            else if(usage_port2_temp==56)
            {
                if(report_size_port2==0)
                    report_size_port2=item_udata_Port2(item);
            }              
            #endif
            if ((parser->global.report_size = item_udata_Port2(item)) > 32) {
            //  DrvHidCore2_printData("invalid report_size %d", parser->global.report_size);
                return -1;
            }
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
            if ((parser->global.report_count = item_udata_Port2(item)) > HID_MAX_USAGES) {
                //DrvHidCore2_printData("invalid report_count %d", parser->global.report_count);
                return -1;
            }
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_ID:
            if ((parser->global.report_id = item_udata_Port2(item)) == 0) {
                //DrvHidCore2_printData("report_id 0 is invalid");
                return -1;
            }
            DrvHidCore2_printData("Find a repoer id: %x\n", parser->global.report_id);
            Get_Report_ID_Port2[u8Interface]=1;
            return 0;

        default:
            //DrvHidCore2_printData("unknown global tag 0x%02bx", item->tag);
            return -1;
    }
}

 int hid_parser_local_Port2(struct hid_parser *parser, struct hid_item *item)
{
    __u32 xdata adata;

      //DrvHidCore2_printMsg("hid_parser_local_2");

    if (item->size == 0) {
        //DrvHidCore2_printMsg("item data expected for local item");
        return -1;
    }

    adata = item_udata_Port2(item);
    //DrvHidCore2_printData("local tag:%x\n",item->tag);
    switch (item->tag) {

        case HID_LOCAL_ITEM_TAG_DELIMITER:

            if (adata) {
                /*
                 * We treat items before the first delimiter
                 * as global to all usage sets (branch 0).
                 * In the moment we process only these global
                 * items and the first delimiter set.
                 */
                if (parser->local.delimiter_depth != 0) {
                    //DrvHidCore2_printMsg("nested delimiters");
                    return -1;
                }
                parser->local.delimiter_depth++;
                parser->local.delimiter_branch++;
            } else {
                if (parser->local.delimiter_depth < 1) {
                    //DrvHidCore2_printMsg("bogus close delimiter");
                    return -1;
                }
                parser->local.delimiter_depth--;
            }
            return 0;

        case HID_LOCAL_ITEM_TAG_USAGE:

            if (parser->local.delimiter_branch > 1) {
                //DrvHidCore2_printMsg("alternative usage ignored");
                return 0;
            }

            //if (item->size <= 2)
                //adata = (parser->global.usage_page << 16) + adata;
            DrvHidCore2_printData("Find a usage: idx: %x", parser->local.usage_index);
            DrvHidCore2_printData(", val: %x\n", (__u16)(adata>>16));
            DrvHidCore2_printData("%x\n", (__u16)adata);
            parser->local.usage[parser->local.usage_index++] = adata;
            usage_port2_temp=adata;
                  //   DrvHidCore2_printData("useage:%lx\n",adata);
#if 0
                     if (adata==USEAGE_X)
                        {
                             axis[axis_idx].startbit=gOffsetBit;
                             axis[axis_idx].size=parser->global.report_size;
                             axis[axis_idx].type=TYPE_AXIS_X;
                             gOffsetBit+=axis[axis_idx].size;
                             DrvHidCore2_printData("Usage(X):%x\n",axis[axis_idx].startbit);

                             axis_idx++;
                        }
                     else if (adata==USEAGE_Y)
                        {
                             axis[axis_idx].startbit=gOffsetBit;
                             axis[axis_idx].size=parser->global.report_size;
                             axis[axis_idx].type=TYPE_AXIS_Y;
                             gOffsetBit+=axis[axis_idx].size;
                             DrvHidCore2_printData("Usage(Y):%x\n",axis[axis_idx].startbit);

                             axis_idx++;
                        }
                     else if (adata==USEAGE_UNDEFINED)
                        {
                            DrvHidCore2_printData("undefine usage :%02bx\n",parser->global.report_size);
                            gOffsetBit+=parser->global.report_size;
                        }
#endif
            return 0;// hid_add_usage(parser, data);

        case HID_LOCAL_ITEM_TAG_USAGE_MINIMUM:

            if (parser->local.delimiter_branch > 1) {
                //DrvHidCore2_printMsg("alternative usage ignored");
                return -1;
            }

            if (item->size <= 2)
                adata = (parser->global.usage_page << 16) + adata;

            parser->local.usage_minimum = adata;
                    //  DrvHidCore2_printData("logical minimum:%02bx\n",adata);
            return 0;

        case HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM:

            if (parser->local.delimiter_branch > 1) {
                //DrvHidCore2_printMsg("alternative usage ignored");
                return -1;
            }

            if (item->size <= 2)
                adata = (parser->global.usage_page << 16) + adata;
                    //  DrvHidCore2_printData("logical maximum:%02bx\n",adata);

            //for (n = parser->local.usage_minimum; n <= data; n++)
                //if (hid_add_usage(parser, n)) {
                //  DrvHidCore2_printMsg("hid_add_usage failed\n");
                //  return -1;
                //}
            return 0;

        default:

            //DrvHidCore2_printData("unknown local item tag 0x%x", item->tag);
            return -1;
    }
    return 0;
}
#if 0
__u8 hid_report_Analysis_Port2(struct hid_parser *parser)
{
    __u8 stk_idx=parser->collection_stack_ptr-1;
    __u8 ret=1;

    //DrvHidCore2_printData("stk_idx:%x", stk_idx);
    //DrvHidCore2_printData("usage:%x", parser->collection_stack[stk_idx].usage);
    if (parser->collection_stack[stk_idx].usage == (UINT8)HID_GD_MOUSE)
    {
        DrvHidCore2_printMsg("USAGE MOUSE!!");
        ret=0;
    }
    else if ((parser->collection_stack[stk_idx].usage == (UINT8)HID_GD_KEYBOARD) ||
             (parser->collection_stack[stk_idx].usage == (UINT8)HID_GD_KEYPAD))
    {
        DrvHidCore2_printMsg("USAGE KEYBOARD/KEYPAD!!");
        ret=0;
    }
    else if (parser->collection_stack[stk_idx].usage == (UINT8)HID_GD_JOYSTICK)
    {
        DrvHidCore2_printMsg("USAGE JOYSTICK!!");
        ret=0;
    }
    return ret;
}
#endif
void hid_parse_report_Port2(__u8 *start, unsigned size, U8 u8Interface)
{
    __u8 *end;
    int xdata result;
    struct hid_item xdata gitem;
    struct hid_parser xdata gparser;


  // DrvHidCore2_printMsg("@@@ >>> Enter hid-core.c file --> hid_parse_report function \n");

    memset((void*)&gparser, 0, sizeof(struct hid_parser));
    //parser->device = device;

    end = start + size;
    while ((start = fetch_item_Port2(start, end, &gitem)) != 0)
    {

        if (gitem.format != HID_ITEM_FORMAT_SHORT)
        {
            DrvHidCore2_printMsg("unexpected long global item");
            return ;
        }
        if (gitem.type==0)
            result=hid_parser_main_Port2(&gparser,&gitem,u8Interface);
        else if (gitem.type==1)
            result=hid_parser_global_Port2(&gparser,&gitem,u8Interface);
        else if (gitem.type==2)
            result=hid_parser_local_Port2(&gparser,&gitem);

        if (result)
        {
            //printf("item %u %u %u %u parsing failed\n",
                //gitem.format, (unsigned)gitem.size, (unsigned)gitem.type, (unsigned)gitem.tag);
            //hid_free_device(device);
            //kfree(parser);
            return ;
        }
        //hid_report_Analysis_Port2(&gparser);

        if (start == end) {
        //  if (parser.collection_stack_ptr) {
                //DrvHidCore2_printMsg("unbalanced collection at end of report description");
                //return ;
            //}
            if (gparser.local.delimiter_depth) {
                //DrvHidCore2_printMsg("unbalanced delimiter at end of report description");
                return ;
            }
            return;
        }
    }

    //DrvHidCore2_printData("item fetching failed at offset %d\n", (int)(end - start));
    return;
}
#if 0
U16 Parse_Joystick_Report_Port2(__u8 *report)
{
__u8 xdata i,tmp;
__u16 xdata tmp2,result;
 //printf("t1");

         //   for (i=0 ; i < 8 ; i++)
        //printf("%02bx ", report[i]);
            for (i=0 ; i < MAX_AXIS_NUM ; i ++)
           {
               if (axis_Port2[i].size)                //this AXIS exist
                {
                        tmp=GetValue_Port2(report,axis_Port2[i].startbit,axis_Port2[i].size);
                        if (axis_Port2[i].val!=tmp )
                            {
                                 DRV_HID_DBG(DrvHidCore2_printData(" axis idx %x", i));
                                 DRV_HID_DBG(DrvHidCore2_printData(", offset:%x",axis_Port2[i].startbit));
                                 DRV_HID_DBG(DrvHidCore2_printData(", val %x ", axis_Port2[i].val));
                                 DRV_HID_DBG(DrvHidCore2_printData(" tmp:%x\n",tmp));
                                 axis_Port2[i].val=tmp;
                                 result=(axis_Port2[i].type << 12) | tmp;
                                 DRV_HID_DBG(DrvHidCore2_printData("return axis:%x\n",result));

                                 return result;

                            }
                }

            }
// printf("t2");
        //printf("button start:%x",buttons.startbit);
            tmp2=GetValue_Port2(report,buttons_Port2[0].startbit,buttons_Port2[0].count);
           // printf("tmp2:%x\n",tmp2);
            if (buttons_Port2[0].val!=tmp2)
                {
                   DRV_HID_DBG(DrvHidCore2_printData("buttons:%x ",buttons_Port2[0].val));
                   DRV_HID_DBG(DrvHidCore2_printData("%x\n", tmp2));
                    buttons_Port2[0].val=tmp2;
                    tmp2 &=0x0fff;              //only maximum 12 bits allowed
                    result=(TYPE_BUTTON << 12) | tmp2;
                    DRV_HID_DBG(DrvHidCore2_printData("return button:%x\n",result));
                    return result;

                }
      //DRV_HID_DBG(printf("return 0\n"));
      return 0;         //no event
}
#endif
U16 Parse_Mouse_Report_Port2(__u8 *report, struct mouse_struct *spMouseReport, U8 u8Interface)
{
    __u8 xdata i;
    __u16 xdata tmp;
    __u16 xdata tmp2,result=0;

    //for (i=0 ; i < 8 ; i++)
    //    DrvHidCore2_printData("%x ", report[i]);
    for (i=0 ; i < MAX_AXIS_NUM ; i ++)
    {
        if (gState_Change_Port2[u8Interface])
        {
        if (axis_Port2[u8Interface][i].size)                //this AXIS exist
        {
            if(Get_Report_ID_Port2[u8Interface]==1)
                tmp=GetValue_Port2(report,axis_Port2[u8Interface][i].startbit+8,axis_Port2[u8Interface][i].size);
            else
                tmp=GetValue_Port2(report,axis_Port2[u8Interface][i].startbit,axis_Port2[u8Interface][i].size);
            if (axis_Port2[u8Interface][i].val!=tmp )
            {
                //DRV_HID_DBG(DrvHidCore2_printData(" axis idx %x", i));
                //DRV_HID_DBG(DrvHidCore2_printData(", offset:%x",axis[i].startbit));
                //DRV_HID_DBG(DrvHidCore2_printData(", val %x ", axis[i].val));
                //DRV_HID_DBG(DrvHidCore2_printData(" tmp:%x\n",tmp));
                axis_Port2[u8Interface][i].val=tmp;
                switch (axis_Port2[u8Interface][i].type)
                {
                    case TYPE_AXIS_X:
                        DrvHidCore2_printData("X:%x", tmp);
                        spMouseReport->rel_x=tmp;
                        break;
                    case TYPE_AXIS_Y:
                        DrvHidCore2_printData("Y:%x", tmp);
                        spMouseReport->rel_y=tmp;
                        break;
                    case TYPE_AXIS_WHEEL:
                        DrvHidCore2_printData("WHEEL:%x", tmp);
                        spMouseReport->rel_wheel=tmp;
                        break;
                }
                result=1;
            }
        }
        }

    }

    for (i=0 ; i < MAX_BUTTON_NUM ; i ++)
    {
        if (gState_Change_Port2[u8Interface])
        {
        if (buttons_Port2[u8Interface][i].count)                //this BUTTON exist
        {
        //DrvHidCore2_printData("button start:%x",buttons.startbit);
        if(Get_Report_ID_Port2[u8Interface]==1)
            tmp2=GetValue_Port2(report,buttons_Port2[u8Interface][i].startbit+8,buttons_Port2[u8Interface][i].count);
        else
            tmp2=GetValue_Port2(report,buttons_Port2[u8Interface][i].startbit,buttons_Port2[u8Interface][i].count);
        // printf("tmp2:%x\n",tmp2);
        if (buttons_Port2[u8Interface][i].val!=tmp2)
        {
            //DRV_HID_DBG(DrvHidCore2_printData("buttons:%x ",buttons[i].val));
            //DRV_HID_DBG(DrvHidCore2_printData("%x\n", tmp2));
            buttons_Port2[u8Interface][i].val=tmp2;
            spMouseReport->button=tmp2;
            result=1;
        }
        }
        }
    }
    return result;
}

__u32 GetValue_Port2(__u8 *buf, __u16 offset,__u8 size)
{

        __u8 *ptr;
        __u32 xdata x,y;
        __u8 xdata i;

        ptr=buf+offset/8;
        x= *ptr + (*(ptr+1)<<8) + (*(ptr+2)<< 16) + (*(ptr+3) <<24);
        x>>=offset % 8;
        y=0;
        for (i=0 ; i < size ; i++)
       {
            y=y << 1;
            y|=1;

       }
        x= x & y;       //mask unwanted bits
        return x;
}
code UINT8  GET_REPORT_DESCRIPTOR_PORT2[]        = {0x81,0x06,0x00,0x22,0x00,0x00,0x65,0x00};
code UINT8  SET_IDLE_PORT2[]                     = {0x21,0x0A,0x00,0x00,0x00,0x00,0x00,0x00};
extern  Host20_Attach_Device_Structure xdata *psAttachDevice_Port2;
extern UINT8 flib_Host20_Issue_Control_Port2(UINT8 bEdNum,UINT8* pbCmd,UINT16 hwDataSize,UINT8* pbData);
BOOLEAN GetReportDescriptor_Port2(UINT8 index)
{
    UINT8 xdata bCMD[8];
    UINT16 wReportLen;

    UINT8 xdata *bptr1=0xF000;
    UINT8 xdata *bptr2=pUsbData_Port2;

   DrvHidCore2_printData("GetReportDescriptor_2 %d", index);
   memcpy(bCMD,SET_IDLE_PORT2,8);
   bCMD[4]=index;
   if ( flib_Host20_Issue_Control_Port2(1,bCMD,0,OTGH_NULL)>0)
   {
       DrvHidCore2_printData("set_idle %d fail", index);
       //return FALSE;
   }

   wReportLen=(UINT16)psAttachDevice_Port2->saCD[0].sInterface[index].sClass[0].bLengthHighByte << 8 |
              (UINT16)psAttachDevice_Port2->saCD[0].sInterface[index].sClass[0].bLengthLowByte;
   DrvHidCore2_printData("Report Len:%x", wReportLen);
   memcpy(bCMD,GET_REPORT_DESCRIPTOR_PORT2,8);
   bCMD[6]=psAttachDevice_Port2->saCD[0].sInterface[index].sClass[0].bLengthLowByte+0x40;
   bCMD[7]=psAttachDevice_Port2->saCD[0].sInterface[index].sClass[0].bLengthHighByte;
   wReportLen+=0x40;
   bCMD[4]=index;
   if ( flib_Host20_Issue_Control_Port2(1,bCMD,wReportLen,pUsbData_Port2)>0)
       return FALSE;


   DrvHidCore2_printData("[0]:%x", pUsbData_Port2[0]);
   DrvHidCore2_printData("[1]:%x", pUsbData_Port2[1]);
   DrvHidCore2_printData("[2]:%x", pUsbData_Port2[2]);
   DrvHidCore2_printData("[3]:%x", pUsbData_Port2[3]);

   wReportLen-=0x40;
   hid_parse_report_Port2(pUsbData_Port2, wReportLen, index);

   return TRUE;

}

extern void  flib_Host20_Interrupt_Init_Port2(UINT8 bAddr, U8 u8Interface, U8 QH_number);
U8 drvUSBHost_HID_Initial_Port2(U8 u8Interface)
{
   UINT8 i;
   DRV_HID_DBG(DrvHidCore2_printMsg("HID device_2\n"));

   gOffsetBit_Port2[u8Interface]=0;
   axis_idx_Port2[u8Interface]=0;
   button_idx_Port2[u8Interface]=0;
   kbd_report_cnt_Port2[u8Interface]=0;
   leds_Port2[u8Interface].count=0;
   leds_Port2[u8Interface].val=0;
   Get_Report_ID_Port2[u8Interface]=0;
   //report_size_port2=0;
   //mouse_size_port2=0;
   for (i=0 ; i < MAX_AXIS_NUM ; i++)
   {
        axis_Port2[u8Interface][i].size=0;
        axis_Port2[u8Interface][i].val=0;
   }

   for (i=0 ; i < MAX_BUTTON_NUM ; i++)
   {
        buttons_Port2[u8Interface][i].count = 0;
        buttons_Port2[u8Interface][i].startbit = 0;
        buttons_Port2[u8Interface][i].val = 0;
   }
   //for (i=0;i<psAttachDevice_Port2->saCD[0].bINTERFACE_NUMBER;i++)
   {
       GetReportDescriptor_Port2(u8Interface);

       //flib_Host20_Interrupt_Init_Port2(3);
       //  printf("HID init ok\n");
       //   while (1)
       //   {
       //    flib_Host20_Issue_Interrupt((U16)pUsbData,8);
       //   Parse_Joystick_Report(pUsbData);
       //   }
   }
   //flib_Host20_Interrupt_Init_Port2(4,u8Interface);
   return 0;
}

#if 0
__u16  MDrv_GET_JOYSTICK_STATUS(void)
{
__u16 xdata tmp;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();        //switch window 1
  #endif
// printf("x1");

     flib_Host20_Issue_Interrupt((U16)pUsbData,8);
// printf("x2");

     tmp=Parse_Joystick_Report(pUsbData);
    //DRV_HID_DBG(  printf("tmp:%x\n",tmp));

 #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif
     //DRV_HID_DBG(printf("Get Joystick status return:%x\n",tmp));
     return tmp;
}
#endif

#if 0
__u8 xdata ReportData[8];
__u8*  MDrv_Get_HID_Report_Raw_Data(void)
{
__u8 xdata i;
#ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();        //switch window 1
 #endif
  flib_Host20_Issue_Interrupt((U16)pUsbData,8);
  DrvHidCore2_printMsg("HID RAW:");
  for (i=0 ; i <4 ; i++)
    {
       ReportData[i]=pUsbData[i];
       DrvHidCore2_printData("%x ",ReportData[i]);
    }
  //DrvHidCore2_printMsg("\n");
 #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif
  return ReportData;
}
#endif
extern void  flib_Host20_Issue_Interrupt_Port2(UINT16 buf,UINT16 hwSize, U8 u8Interface);
extern void  flib_Host20_Issue_Interrupt_qh2_Port2(UINT16 buf,UINT16 hwSize, U8 u8Interface);
void  MDrv_Get_Mouse_Staus_Port2(U8 u8Interface)
{
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();        //switch window 1
  #endif

  flib_Host20_Issue_Interrupt_qh2_Port2((U16)pUsbData_Port2,30, u8Interface);

  Parse_Mouse_Report_Port2(pUsbData_Port2, &sMouseReport_Port2, u8Interface);
  if (gState_Change_Port2[u8Interface])
  {
    if(mouse_size_port2==0x8)
    {
        if((sMouseReport_Port2.rel_x & BIT7)==0x0080)
        {
            sMouseReport_Port2.rel_x = sMouseReport_Port2.rel_x | 0xFF00;
            //DRV_HID_DBG(DrvHidCore_printData("X ==> %x", sMouseReport.rel_x/*sMouseReport[u8Interface].rel_x*/));
        }
        if((sMouseReport_Port2.rel_y & BIT7)==0x0080)
        {
            sMouseReport_Port2.rel_y = sMouseReport_Port2.rel_y | 0xFF00;
            //DRV_HID_DBG(DrvHidCore_printData("Y ==> %x", sMouseReport.rel_y/*sMouseReport[u8Interface].rel_y*/));
        }
    }
    else if(mouse_size_port2==0xC)
    {
        if((sMouseReport_Port2.rel_x & BIT11)==0x0800)
        {
            sMouseReport_Port2.rel_x = sMouseReport_Port2.rel_x | 0xF000;
            //DRV_HID_DBG(DrvHidCore_printData("X ==> %x", sMouseReport.rel_x/*sMouseReport[u8Interface].rel_x*/));
        } 
        if((sMouseReport_Port2.rel_y & BIT11)==0x0800)
        {
            sMouseReport_Port2.rel_y = sMouseReport_Port2.rel_y | 0xF000;
            //DRV_HID_DBG(DrvHidCore_printData("Y ==> %x", sMouseReport.rel_y/*sMouseReport[u8Interface].rel_y*/));
        }        
    }
    else if(mouse_size_port2==0x10)
    {
        
    }
    else
    {    //DRV_HID_DBG(DrvHidCore2_printData("ERROR:report size ==> 0x%x",report_size_port2));
    }

    //DRV_HID_DBG(DrvHidCore2_printData("report size ==> %x",report_size_port2));
    //DRV_HID_DBG(DrvHidCore2_printMsg("Mouse Report:"));
    //DRV_HID_DBG(DrvHidCore2_printData("Logical Max ==> %x",logical_max_port2));
    //DRV_HID_DBG(DrvHidCore2_printData("Button ==> %x", sMouseReport_Port2.button));
    //DRV_HID_DBG(DrvHidCore2_printData("X ==> %x", sMouseReport_Port2.rel_x));
    //DRV_HID_DBG(DrvHidCore2_printData("Y ==> %x", sMouseReport_Port2.rel_y));
    //DRV_HID_DBG(DrvHidCore2_printData("WHEEL ==> %x", sMouseReport_Port2.rel_wheel));
    #if defined(_DUMMY_LAPTOP_)
    {   extern void MHL_SendMousePacket(BYTE u8Port,struct mouse_struct *pMouse);
        MHL_SendMousePacket(2,&sMouseReport_Port2);
    }
    #endif

  }
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif

}

void  MDrv_Get_Keyboard_Staus_Port2(U8 u8Interface)
{
  __u8 i;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();        //switch window 1
  #endif

  flib_Host20_Issue_Interrupt_Port2((U16)pUsbData_Port2,8, u8Interface);

  //Parse_Mouse_Report_Port2(pUsbData_Port2, &sMouseReport_Port2);
  if (gState_Change_Port2[u8Interface])
  {
    DrvHidCore2_printMsg("Keyboard HID RAW:");
    for (i=0 ; i <8 ; i++)
    {
       DrvHidCore2_printData("%x ",pUsbData_Port2[i]);
    }
    #if defined(_DUMMY_LAPTOP_)
    {   extern void MHL_SendKeyboardPacket(BYTE u8Port,BYTE *pu8Buffer);
        MHL_SendKeyboardPacket(2,pUsbData_Port2);
    }
    #endif
  }
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif

}
#else
BYTE code drvhidcore_2[] = {0};
void mshidcore_2Dummy(void)
{
    BYTE xdata x = drvhidcore_2[0];
}
#endif
