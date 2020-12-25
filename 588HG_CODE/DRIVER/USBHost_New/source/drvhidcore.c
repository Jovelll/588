#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "USBHostConfig.h"

#include "UsbHostDataDef.h"
#include "drvUsbMain.h"
//#include "sysinfo.h"
//#include "drvtimer.h"
//#include "R8051XC.h"
#include "drvusb.h"
//#include "msAPI_Timer.h"
#include "drvhid.h"
#include "drvhostlib.h"
#include "common.h"


#define DrvHidCore_DEBUG    1
#if ENABLE_DEBUG&&DrvHidCore_DEBUG
    #define DrvHidCore_printData(str, value)   printData(str, value)
    #define DrvHidCore_printMsg(str)           printMsg(str)
#else
    #define DrvHidCore_printData(str, value)
    #define DrvHidCore_printMsg(str)
#endif

#define DRV_HID_DBG(x)  x;

#define STR_C51_VERSION(x) "C51:"##  #x

#define C51_VERSION(x) STR_C51_VERSION(x)
char* xdata usb_hid_p1_c51version = C51_VERSION(__C51__);

U8 code USBHID_P1_VBuf[32] = {'M', 'S', 'V', 'C', '0', '0',             // 0, 0 fixed
                        'U', '3',                                   // Library ID
#ifdef   DTV_STANDARD_LIB
                        'A', '0', '0', '1', '0', '2',               // build number
#endif
                        '0', '0', '0', '0', '0', '0', '0', '0',     // change list 46058
                        '0', '0', '0', '0', '0', '0', '0', '0', '0',// A4: Saturn, A:LG
                        'T'};

extern void MDrv_USB_SetXdataWindow1(void);
extern void MDrv_USB_ReleaseXdataWindow1(void);
extern U8 xdata PORT_DEVICE[HOST20_INTERFACE_NUM_MAX];
/*
 * Read data value from item.
 */
extern UINT8 xdata pUsbData[512];
struct   axis_struct  xdata  axis[HOST20_INTERFACE_NUM_MAX][MAX_AXIS_NUM];
__u8    xdata axis_idx[HOST20_INTERFACE_NUM_MAX];
struct   button_struct  xdata buttons[HOST20_INTERFACE_NUM_MAX][MAX_BUTTON_NUM];
__u8    xdata button_idx[HOST20_INTERFACE_NUM_MAX];
struct   leds_struct  xdata leds[HOST20_INTERFACE_NUM_MAX];
__u8     kbd_report_cnt[HOST20_INTERFACE_NUM_MAX];
__u8     Get_Report_ID[HOST20_INTERFACE_NUM_MAX];

__u16  xdata gOffsetBit[HOST20_INTERFACE_NUM_MAX];
UINT8 xdata gState_Change[HOST20_INTERFACE_NUM_MAX];

UINT8 usage_temp;
long logical_max,logical_min;
long report_size,mouse_size;

struct mouse_struct xdata sMouseReport/*[HOST20_INTERFACE_NUM_MAX]*/;

__u32 item_udata(struct hid_item *item)
{
//    DO_PRINT("@@@ >>> Enter hid-core.c file --> item_udata function \n");

    switch (item->size) {
        case 1: return item->zdata.u8;
        case 2: return item->zdata.u16;
        case 4: return item->zdata.u32;
    }
    return 0;
}

static __s32 item_sdata(struct hid_item *item)
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


static __u8 *fetch_item(__u8 *start, __u8 *end, struct hid_item *item)
{
    //printf("@@@ >>> Enter hid-core.c file --> fetch_item function \n");

    if ((end - start) > 0) {

        __u8 b = *start++;
        //DrvHidCore_printData("b:%x ",b);
        item->type = (b >> 2) & 3;
        item->tag  = (b >> 4) & 15;
        //DrvHidCore_printData("tag:%x ",item->tag);

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

static int open_collection(struct hid_parser *parser, unsigned type)
{
    struct hid_collection xdata *collection;
    unsigned xdata usage;

    usage = parser->local.usage[0];

    if (parser->collection_stack_ptr == HID_COLLECTION_STACK_SIZE) {
        DrvHidCore_printMsg("collection stack overflow\n");
        return -1;
    }

    collection = parser->collection_stack + parser->collection_stack_ptr++;
    collection->type = type;
    collection->usage = usage;

    return 0;
}

static int close_collection(struct hid_parser *parser)
{
    if (!parser->collection_stack_ptr) {
        DrvHidCore_printMsg("collection stack underflow\n");
        return -1;
    }
    parser->collection_stack_ptr--;
    return 0;
}

static int hid_chk_variable(struct hid_parser *parser, U8 report_type, U8 u8Interface)
{
    U8  xdata ii, usg_count, rpt_count;

    usg_count = parser->local.usage_index;
    rpt_count = parser->global.report_count;

#if 0
    DrvHidCore_printData("hid_chk_variable: type: %x\n", report_type);
    DrvHidCore_printData("usage count: %x", usg_count);
    DrvHidCore_printData(", size: %x", parser->global.report_size);
    DrvHidCore_printData(", count: %x\n", rpt_count);
#endif
    if (parser->global.usage_page == USEAGE_PAGE_BUTTON)
    {
        buttons[u8Interface][button_idx[u8Interface]].startbit=gOffsetBit[u8Interface];
        buttons[u8Interface][button_idx[u8Interface]].count=parser->global.report_size * rpt_count;
        DrvHidCore_printData("Usage Page (buttons): %x size: ", buttons[u8Interface][button_idx[u8Interface]].startbit);
        DrvHidCore_printData("%x\n", buttons[u8Interface][button_idx[u8Interface]].count);

        gOffsetBit[u8Interface]+=buttons[u8Interface][button_idx[u8Interface]].count;
        button_idx[u8Interface]++;
        goto Func_Done;
    }
    else if (parser->global.usage_page == USEAGE_PAGE_KEYBOARD)
    {
        kbd_report_cnt[u8Interface]+=parser->global.report_size * rpt_count;
        DrvHidCore_printData("Usage Page_%x (keyboard): size:", u8Interface);
        DrvHidCore_printData(" %x ", kbd_report_cnt[u8Interface]);
        goto Func_Done;
    }
    else if (parser->global.usage_page == USEAGE_PAGE_LEDS)
    {
        leds[u8Interface].count=parser->global.report_size * rpt_count;
        DrvHidCore_printData("Usage Page (leds): size %x", leds[u8Interface].count);
        goto Func_Done;
    }


    if (usg_count > 0)
    {
        for(ii=usg_count-rpt_count; ii<usg_count; ii++)
        {
            DrvHidCore_printData("local usage %x: ", ii);
            DrvHidCore_printData("%x\n", parser->local.usage[ii]);
            if (report_type == HID_INPUT_REPORT)
            {
                if (parser->local.usage[ii] == USEAGE_X)
                {
                     axis[u8Interface][axis_idx[u8Interface]].startbit=gOffsetBit[u8Interface];
                     axis[u8Interface][axis_idx[u8Interface]].size=parser->global.report_size;
                     axis[u8Interface][axis_idx[u8Interface]].type=TYPE_AXIS_X;
                     gOffsetBit[u8Interface]+=axis[u8Interface][axis_idx[u8Interface]].size;
                     DrvHidCore_printData("Usage(X): %x",axis[u8Interface][axis_idx[u8Interface]].startbit);
                     DrvHidCore_printData(", size: %x\n", axis[u8Interface][axis_idx[u8Interface]].size);
                     axis_idx[u8Interface]++;
                }
                else if (parser->local.usage[ii] == USEAGE_Y)
                {
                     axis[u8Interface][axis_idx[u8Interface]].startbit=gOffsetBit[u8Interface];
                     axis[u8Interface][axis_idx[u8Interface]].size=parser->global.report_size;
                     axis[u8Interface][axis_idx[u8Interface]].type=TYPE_AXIS_Y;
                     gOffsetBit[u8Interface]+=axis[u8Interface][axis_idx[u8Interface]].size;
                     DrvHidCore_printData("Usage(Y): %x",axis[u8Interface][axis_idx[u8Interface]].startbit);
                     DrvHidCore_printData(", size: %x\n", axis[u8Interface][axis_idx[u8Interface]].size);
                     mouse_size=report_size;
                     DrvHidCore_printData("report_size:%x\n",mouse_size);
                     axis_idx[u8Interface]++;
                }
                else if (parser->local.usage[ii] == USEAGE_WHEEL)
                {
                     axis[u8Interface][axis_idx[u8Interface]].startbit=gOffsetBit[u8Interface];
                     axis[u8Interface][axis_idx[u8Interface]].size=parser->global.report_size;
                     axis[u8Interface][axis_idx[u8Interface]].type=TYPE_AXIS_WHEEL;
                     gOffsetBit[u8Interface]+=axis[u8Interface][axis_idx[u8Interface]].size;
                     DrvHidCore_printData("Usage(WHEEL): %x",axis[u8Interface][axis_idx[u8Interface]].startbit);
                     DrvHidCore_printData(", size: %x\n", axis[u8Interface][axis_idx[u8Interface]].size);
                     axis_idx[u8Interface]++;
                }
                else
                {
                    DrvHidCore_printData("Unsupport usage: %x", gOffsetBit[u8Interface]);
                    DrvHidCore_printData(", size: %x\n", parser->global.report_size);
                    //gOffsetBit[u8Interface] += parser->global.report_size;
                }
            }
            else
            {
                DrvHidCore_printData("Unsupport usage: %x", gOffsetBit[u8Interface]);
                DrvHidCore_printData(", size: %x\n", parser->global.report_size);
                //gOffsetBit[u8Interface] += parser->global.report_size;
            }
        }
    }
    else
    {
        DrvHidCore_printData("Unsupport data: %x", gOffsetBit[u8Interface]);
        DrvHidCore_printData(", size: %x\n", parser->global.report_size * rpt_count);
        //gOffsetBit[u8Interface] += parser->global.report_size * rpt_count;
    }

Func_Done:
    return 0;
}

/*
 * Process a main item.
 */
 int hid_parser_main(struct hid_parser *parser, struct hid_item *item, U8 u8Interface)
{
    __u32 xdata adata;
    int xdata ret=0;

    //printMsg("hid_parser_main");

    adata = item_udata(item);
      // DrvHidCore_printData("TAG:%02bx\n",item->tag);
    switch (item->tag) {
        case HID_MAIN_ITEM_TAG_BEGIN_COLLECTION:
#if 1
                     if (adata==HID_COLLECTION_APPLICATION)
                        {
                            DrvHidCore_printMsg("application\n");
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
            ret = open_collection(parser, adata & 3);
            break;
        case HID_MAIN_ITEM_TAG_END_COLLECTION:
            ret = close_collection(parser);
            break;
        case HID_MAIN_ITEM_TAG_INPUT:
            ret = hid_chk_variable(parser, HID_INPUT_REPORT, u8Interface);
            //ret = hid_add_field(parser, HID_INPUT_REPORT, data);
            break;
        case HID_MAIN_ITEM_TAG_OUTPUT:
            ret = hid_chk_variable(parser, HID_OUTPUT_REPORT, u8Interface);
            //ret = hid_add_field(parser, HID_OUTPUT_REPORT, data);
            break;
        case HID_MAIN_ITEM_TAG_FEATURE:
            ret = hid_chk_variable(parser, HID_FEATURE_REPORT, u8Interface);
        //  ret = hid_add_field(parser, HID_FEATURE_REPORT, data);
            break;
        default:
            //DrvHidCore_printData("unknown main item tag 0x%02bx", item->tag);
            ret = 0;
    }

    memset(&parser->local, 0, sizeof(parser->local));   /* Reset the local parser environment */

    return ret;
}


/*
 * Process a global item.
 */

 int hid_parser_global(struct hid_parser *parser, struct hid_item *item, U8 u8Interface)
{
    //printMsg("hid_parser_global");
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
            parser->global.usage_page = item_udata(item);

                    DrvHidCore_printData("Find a usage page: %x\n", parser->global.usage_page);
#if 0
                     if (parser->global.usage_page==USEAGE_PAGE_BUTTON)
                        {
                            buttons.startbit=gOffsetBit;
                            DrvHidCore_printData("buttons start:%x\n",buttons.startbit);
                            buttons.count=parser->global.report_count;
                            DrvHidCore_printData("buttons count:%02bx\n",buttons.count);

                            gOffsetBit+=buttons.count;
                        }
#endif
            return 0;

        case HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
            parser->global.logical_minimum = item_sdata(item);
            if(usage_temp==48 ||usage_temp==49)
            {
                logical_min=parser->global.logical_minimum;             
            }      
            else if(usage_temp==56)
            {
                if(logical_min==0)
                    logical_min=parser->global.logical_minimum;    
            }
            DrvHidCore_printData("usage_temp:%d\n",usage_temp);
            DrvHidCore_printData("@@logical_minimum:%x\n",parser->global.logical_minimum);
            DrvHidCore_printData("logical_min:%x\n",logical_min);
           
            return 0;

        case HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
            if (parser->global.logical_minimum < 0)
                parser->global.logical_maximum = item_sdata(item);
            else
                parser->global.logical_maximum = item_udata(item);

            if(usage_temp==48 ||usage_temp==49)
            {
                logical_max=parser->global.logical_maximum;             
            } 
            else if(usage_temp==56)
            {
                if(logical_max==0)
                    logical_max=parser->global.logical_maximum;  
            }
            DrvHidCore_printData("usage_temp:%d\n",usage_temp);
            DrvHidCore_printData("@@logical_maximum:%x\n",parser->global.logical_maximum);
            DrvHidCore_printData("logical_max:%x\n",logical_max);           
            return 0;

        case HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM:
            parser->global.physical_minimum = item_sdata(item);  
            return 0;

        case HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM:
            if (parser->global.physical_minimum < 0)
                parser->global.physical_maximum = item_sdata(item);
            else
                parser->global.physical_maximum = item_udata(item);  
            return 0;

        case HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT:
            parser->global.unit_exponent = item_udata(item);  
            return 0;

        case HID_GLOBAL_ITEM_TAG_UNIT:
            parser->global.unit = item_udata(item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
            DrvHidCore_printData("##Report size:%x\n",item_udata(item));  
            report_size=item_udata(item);
            #if 0
            if(usage_temp==48 ||usage_temp==49)
            {
                report_size=item_udata(item);
                DrvHidCore_printData("@@Report size:%x\n",report_size); 
            }
            else if(usage_temp==56)
            {
                if(report_size==0)
                    report_size=item_udata(item);
            }            
            #endif
            if ((parser->global.report_size = item_udata(item)) > 32) {
            //  DrvHidCore_printData("invalid report_size %d", parser->global.report_size);
           
//            report_size=parser->global.report_size;
                return -1;
            }
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
            if ((parser->global.report_count = item_udata(item)) > HID_MAX_USAGES) {
                //DrvHidCore_printData("invalid report_count %d", parser->global.report_count);
                return -1;
            }
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_ID:
            if ((parser->global.report_id = item_udata(item)) == 0) {
                //DrvHidCore_printData("report_id 0 is invalid");
                return -1;
            }
            DrvHidCore_printData("Find a repoer id: %x\n", parser->global.report_id);
            Get_Report_ID[u8Interface]=1;
            return 0;

        default:
            //DrvHidCore_printData("unknown global tag 0x%02bx", item->tag);
            return -1;
    }
}

 int hid_parser_local(struct hid_parser *parser, struct hid_item *item)
{
    __u32 xdata adata;

      //printMsg("hid_parser_local");

    if (item->size == 0) {
        //printMsg("item data expected for local item");
        return -1;
    }

    adata = item_udata(item);
    //DrvHidCore_printData("local tag:%x\n",item->tag);
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
                    //printMsg("nested delimiters");
                    return -1;
                }
                parser->local.delimiter_depth++;
                parser->local.delimiter_branch++;
            } else {
                if (parser->local.delimiter_depth < 1) {
                    //printMsg("bogus close delimiter");
                    return -1;
                }
                parser->local.delimiter_depth--;
            }
            return 0;

        case HID_LOCAL_ITEM_TAG_USAGE:

            if (parser->local.delimiter_branch > 1) {
                //printMsg("alternative usage ignored");
                return 0;
            }

            //if (item->size <= 2)
                //adata = (parser->global.usage_page << 16) + adata;
            DrvHidCore_printData("Find a usage: idx: %x", parser->local.usage_index);
            DrvHidCore_printData(", val: %x\n", (__u16)(adata>>16));
            DrvHidCore_printData("%x\n", (__u16)adata);
            usage_temp=adata;
            parser->local.usage[parser->local.usage_index++] = adata;
                  //   DrvHidCore_printData("useage:%lx\n",adata);
#if 0
                     if (adata==USEAGE_X)
                        {
                             axis[axis_idx].startbit=gOffsetBit;
                             axis[axis_idx].size=parser->global.report_size;
                             axis[axis_idx].type=TYPE_AXIS_X;
                             gOffsetBit+=axis[axis_idx].size;
                             DrvHidCore_printData("Usage(X):%x\n",axis[axis_idx].startbit);

                             axis_idx++;
                        }
                     else if (adata==USEAGE_Y)
                        {
                             axis[axis_idx].startbit=gOffsetBit;
                             axis[axis_idx].size=parser->global.report_size;
                             axis[axis_idx].type=TYPE_AXIS_Y;
                             gOffsetBit+=axis[axis_idx].size;
                             DrvHidCore_printData("Usage(Y):%x\n",axis[axis_idx].startbit);

                             axis_idx++;
                        }
                     else if (adata==USEAGE_UNDEFINED)
                        {
                            DrvHidCore_printData("undefine usage :%02bx\n",parser->global.report_size);
                            gOffsetBit+=parser->global.report_size;
                        }
#endif
            return 0;// hid_add_usage(parser, data);

        case HID_LOCAL_ITEM_TAG_USAGE_MINIMUM:

            if (parser->local.delimiter_branch > 1) {
                //printMsg("alternative usage ignored");
                return -1;
            }

            if (item->size <= 2)
                adata = (parser->global.usage_page << 16) + adata;

            parser->local.usage_minimum = adata;
            //DrvHidCore_printData("logical minimum:%x\n",adata);
            return 0;

        case HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM:

            if (parser->local.delimiter_branch > 1) {
                //printMsg("alternative usage ignored");
                return -1;
            }

            if (item->size <= 2)
                adata = (parser->global.usage_page << 16) + adata;
            //DrvHidCore_printData("logical maximum:%x\n",adata);

            //for (n = parser->local.usage_minimum; n <= data; n++)
                //if (hid_add_usage(parser, n)) {
                //  printMsg("hid_add_usage failed\n");
                //  return -1;
                //}
            return 0;

        default:

            //DrvHidCore_printData("unknown local item tag 0x%x", item->tag);
            return -1;
    }
    return 0;
}
#if 0
__u8 hid_report_Analysis(struct hid_parser *parser)
{
    __u8 stk_idx=parser->collection_stack_ptr-1;
    __u8 ret=1;

    //DrvHidCore_printData("stk_idx:%x", stk_idx);
    //DrvHidCore_printData("usage:%x", parser->collection_stack[stk_idx].usage);
    if (parser->collection_stack[stk_idx].usage == (UINT8)HID_GD_MOUSE)
    {
        printMsg("USAGE MOUSE!!");
        ret=0;
    }
    else if ((parser->collection_stack[stk_idx].usage == (UINT8)HID_GD_KEYBOARD) ||
             (parser->collection_stack[stk_idx].usage == (UINT8)HID_GD_KEYPAD))
    {
        printMsg("USAGE KEYBOARD/KEYPAD!!");
        ret=0;
    }
    else if (parser->collection_stack[stk_idx].usage == (UINT8)HID_GD_JOYSTICK)
    {
        printMsg("USAGE JOYSTICK!!");
        ret=0;
    }
    return ret;
}
#endif
void hid_parse_report(__u8 *start, unsigned size, U8 u8Interface)
{
    __u8 *end;
    int xdata result;
    struct hid_item xdata gitem;
    struct hid_parser xdata gparser;

    DrvHidCore_printData("hid_parse_report %x\n",u8Interface);
  // printMsg("@@@ >>> Enter hid-core.c file --> hid_parse_report function \n");
    memset((void*)&gparser, 0, sizeof(struct hid_parser));
    //parser->device = device;
    end = start + size;
    while ((start = fetch_item(start, end, &gitem)) != 0)
    {
        if (gitem.format != HID_ITEM_FORMAT_SHORT)
        {
            DrvHidCore_printMsg("unexpected long global item");
            return ;
        }
        if (gitem.type==0)
            result=hid_parser_main(&gparser,&gitem,u8Interface);
        else if (gitem.type==1)
            result=hid_parser_global(&gparser,&gitem,u8Interface);
        else if (gitem.type==2)
            result=hid_parser_local(&gparser,&gitem);

        if (result)
        {
            //printf("item %u %u %u %u parsing failed\n",
                //gitem.format, (unsigned)gitem.size, (unsigned)gitem.type, (unsigned)gitem.tag);
            //hid_free_device(device);
            //kfree(parser);
            return ;
        }
        //hid_report_Analysis(&gparser);

        if (start == end) {
        //  if (parser.collection_stack_ptr) {
                //printMsg("unbalanced collection at end of report description");
                //return ;
            //}
            if (gparser.local.delimiter_depth) {
                //printMsg("unbalanced delimiter at end of report description");
                return ;
            }
            return;
        }
    }
    //DrvHidCore_printData("item fetching failed at offset %d\n", (int)(end - start));
    return;
}
#if 0
U16 Parse_Joystick_Report(__u8 *report)
{
__u8 xdata i,tmp;
__u16 xdata tmp2,result;
 //printf("t1");

         //   for (i=0 ; i < 8 ; i++)
        //printf("%02bx ", report[i]);
            for (i=0 ; i < MAX_AXIS_NUM ; i ++)
           {
               if (axis[i].size)                //this AXIS exist
                {
                        tmp=GetValue(report,axis[i].startbit,axis[i].size);
                        if (axis[i].val!=tmp )
                            {
                                 DRV_HID_DBG(DrvHidCore_printData(" axis idx %x", i));
                                 DRV_HID_DBG(DrvHidCore_printData(", offset:%x",axis[i].startbit));
                                 DRV_HID_DBG(DrvHidCore_printData(", val %x ", axis[i].val));
                                 DRV_HID_DBG(DrvHidCore_printData(" tmp:%x\n",tmp));
                                 axis[i].val=tmp;
                                 result=(axis[i].type << 12) | tmp;
                                 DRV_HID_DBG(DrvHidCore_printData("return axis:%x\n",result));

                                 return result;

                            }
                }

            }
// printf("t2");
        //printf("button start:%x",buttons.startbit);
            tmp2=GetValue(report,buttons[0].startbit,buttons[0].count);
           // printf("tmp2:%x\n",tmp2);
            if (buttons[0].val!=tmp2)
                {
                   DRV_HID_DBG(DrvHidCore_printData("buttons:%x ",buttons[0].val));
                   DRV_HID_DBG(DrvHidCore_printData("%x\n", tmp2));
                    buttons[0].val=tmp2;
                    tmp2 &=0x0fff;              //only maximum 12 bits allowed
                    result=(TYPE_BUTTON << 12) | tmp2;
                    DRV_HID_DBG(DrvHidCore_printData("return button:%x\n",result));
                    return result;

                }
      //DRV_HID_DBG(printf("return 0\n"));
      return 0;         //no event
}
#endif
U16 Parse_Mouse_Report(__u8 *report, struct mouse_struct *spMouseReport, U8 u8Interface)
{
    __u8 xdata i;
    __u16 xdata tmp;
    __u16 xdata tmp2,result=0;


    //for (i=0 ; i < 8 ; i++)
    //    DrvHidCore_printData("%x ", report[i]);
    for (i=0 ; i < MAX_AXIS_NUM ; i ++)
    {
        if (gState_Change[u8Interface])
        {
        if (axis[u8Interface][i].size)                //this AXIS exist
        {
            if(Get_Report_ID[u8Interface]==1)
                tmp=(__s32)GetValue(report,axis[u8Interface][i].startbit+8,axis[u8Interface][i].size);
            else
                tmp=(__s32)GetValue(report,axis[u8Interface][i].startbit,axis[u8Interface][i].size);
            if (axis[u8Interface][i].val!=tmp )
            {
                //DRV_HID_DBG(DrvHidCore_printData(" axis idx %x", i));
                //DRV_HID_DBG(DrvHidCore_printData(", offset:%x",axis[u8Interface][i].startbit));
                //DRV_HID_DBG(DrvHidCore_printData(", type %x ", axis[u8Interface][i].type));
                //DRV_HID_DBG(DrvHidCore_printData(", size %x ", axis[u8Interface][i].size));
                //DRV_HID_DBG(DrvHidCore_printData(" tmp:%x\n",tmp));
                axis[u8Interface][i].val=tmp;
                switch (axis[u8Interface][i].type)
                {
                    case TYPE_AXIS_X:
                        DrvHidCore_printData("X:%x", tmp);
                        spMouseReport->rel_x=tmp;
                        //rel_x=tmp;
                        break;
                    case TYPE_AXIS_Y:
                        DrvHidCore_printData("Y:%x", tmp);
                        spMouseReport->rel_y=tmp;
                        //rel_y=tmp;
                        break;
                    case TYPE_AXIS_WHEEL:
                        DrvHidCore_printData("WHEEL:%x", tmp);
                        spMouseReport->rel_wheel=tmp;
                        //rel_wheel=tmp;
                        break;
                }
                result=1;
            }
        }
        }

    }

    for (i=0 ; i < MAX_BUTTON_NUM ; i ++)
    {
        if (gState_Change[u8Interface])
        {
        if (buttons[u8Interface][i].count)                //this BUTTON exist
        {
        //DrvHidCore_printData("button start:%x",buttons.startbit);
        if(Get_Report_ID[u8Interface]==1)
            tmp2=GetValue(report,buttons[u8Interface][i].startbit+8,buttons[u8Interface][i].count);//offset 8bits
        else
            tmp2=GetValue(report,buttons[u8Interface][i].startbit,buttons[u8Interface][i].count);
        if (buttons[u8Interface][i].val!=tmp2)
        {
            //DRV_HID_DBG(DrvHidCore_printData("buttons:%x ",buttons[i].val));
            //DRV_HID_DBG(DrvHidCore_printData("%x\n", tmp2));
            buttons[u8Interface][i].val=tmp2;
            spMouseReport->button=tmp2;
            result=1;
        }
        }
        }
    }
    return result;
}

__u32 GetValue(__u8 *buf, __u16 offset,__u8 size)
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
code UINT8  GET_REPORT_DESCRIPTOR[]        = {0x81,0x06,0x00,0x22,0x00,0x00,0x65,0x00};
code UINT8  SET_IDLE[]                     = {0x21,0x0A,0x00,0x00,0x00,0x00,0x00,0x00};

BOOLEAN GetReportDescriptor(UINT8 index)
{
    UINT8 xdata bCMD[8];
    UINT16 wReportLen;

   DrvHidCore_printData("GetReportDescriptor %d", index);
   memcpy(bCMD,SET_IDLE,8);
   bCMD[4]=index;
   if ( flib_Host20_Issue_Control (1,bCMD,0,OTGH_NULL)>0)
   {
       DrvHidCore_printData("set_idle %d fail", index);
       //return FALSE;
   }

   wReportLen=(UINT16)sAttachDevice.saCD[0].sInterface[index].sClass[0].bLengthHighByte << 8 |
              (UINT16)sAttachDevice.saCD[0].sInterface[index].sClass[0].bLengthLowByte;
   DrvHidCore_printData("Report Len:%x", wReportLen);
   memcpy(bCMD,GET_REPORT_DESCRIPTOR,8);
   bCMD[6]=sAttachDevice.saCD[0].sInterface[index].sClass[0].bLengthLowByte+0x40;
   bCMD[7]=sAttachDevice.saCD[0].sInterface[index].sClass[0].bLengthHighByte;
   wReportLen+=0x40;
   bCMD[4]=index;

   if ( flib_Host20_Issue_Control (1,bCMD,wReportLen,pUsbData)>0)
       return FALSE;

   DrvHidCore_printData("[0]:%x", pUsbData[0]);
   DrvHidCore_printData("[1]:%x", pUsbData[1]);
   DrvHidCore_printData("[2]:%x", pUsbData[2]);
   DrvHidCore_printData("[3]:%x", pUsbData[3]);
//if(PORT_DEVICE[index]==1)
{
   wReportLen-=0x40; 
   hid_parse_report(pUsbData, wReportLen, index);
}
   return TRUE;

}

U8 drvUSBHost_HID_Initial(U8 u8Interface)
{
   UINT8 i;
   DRV_HID_DBG(DrvHidCore_printData("HID device_%x\n",u8Interface));
   gOffsetBit[u8Interface]=0;
   axis_idx[u8Interface]=0;
   button_idx[u8Interface]=0;
   kbd_report_cnt[u8Interface]=0;
   leds[u8Interface].count=0;
   leds[u8Interface].val=0;
   Get_Report_ID[u8Interface]=0;
   //report_size=0;
   //mouse_size=0;
   for (i=0 ; i < MAX_AXIS_NUM ; i++)
   {
        axis[u8Interface][i].size=0;
        axis[u8Interface][i].val=0;
   }

   for (i=0 ; i < MAX_BUTTON_NUM ; i++)
   {
        buttons[u8Interface][i].count = 0;
        buttons[u8Interface][i].startbit = 0;
        buttons[u8Interface][i].val = 0;
   }
//if(PORT_DEVICE[u8Interface]==1)
   GetReportDescriptor(u8Interface);
//if(u8Interface==2)
//   flib_Host20_Interrupt_Init(3,0);
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
  printMsg("HID RAW:");
  for (i=0 ; i <4 ; i++)
    {
       ReportData[i]=pUsbData[i];
       DrvHidCore_printData("%x ",ReportData[i]);
    }
  //printMsg("\n");
 #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif
  return ReportData;
}
#endif

void  MDrv_Get_Mouse_Staus(U8 u8Interface)
{
  U8 i=0;
  //__u16 xdata tmp;
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();        //switch window 1
  #endif

  flib_Host20_Issue_Interrupt_qh2((U16)pUsbData, 30,u8Interface);
#if 0
  if (gState_Change[u8Interface])
  {
    DrvHidCore_printData("%x\n",pUsbData[0]);
    DrvHidCore_printData("%x\n",pUsbData[1]);
    DrvHidCore_printData("%x\n",pUsbData[2]);
    DrvHidCore_printData("%x\n",pUsbData[3]);
    DrvHidCore_printData("%x\n",pUsbData[4]);
    DrvHidCore_printData("%x\n",pUsbData[5]);
    DrvHidCore_printData("%x\n",pUsbData[6]);
    DrvHidCore_printData("%x\n",pUsbData[7]);
    DrvHidCore_printData("%x\n",pUsbData[8]);
    DrvHidCore_printData("%x\n",pUsbData[9]);
  }
#endif    
  Parse_Mouse_Report(pUsbData, &sMouseReport/*[u8Interface]*/, u8Interface);
  
  if (gState_Change[u8Interface])
  {
    if(mouse_size==0x8)
    {
        if((sMouseReport.rel_x & BIT7)==0x0080)
        {
            sMouseReport.rel_x = sMouseReport.rel_x | 0xFF00;
            //DRV_HID_DBG(DrvHidCore_printData("X ==> %x", sMouseReport.rel_x/*sMouseReport[u8Interface].rel_x*/));
        }
        if((sMouseReport.rel_y & BIT7)==0x0080)
        {
            sMouseReport.rel_y = sMouseReport.rel_y | 0xFF00;
            //DRV_HID_DBG(DrvHidCore_printData("Y ==> %x", sMouseReport.rel_y/*sMouseReport[u8Interface].rel_y*/));
        }
    }
    else if(mouse_size==0xC)
    {
        if((sMouseReport.rel_x & BIT11)==0x0800)
        {
            sMouseReport.rel_x = sMouseReport.rel_x | 0xF000;
            //DRV_HID_DBG(DrvHidCore_printData("X ==> %x", sMouseReport.rel_x/*sMouseReport[u8Interface].rel_x*/));
        } 
        if((sMouseReport.rel_y & BIT11)==0x0800)
        {
            sMouseReport.rel_y = sMouseReport.rel_y | 0xF000;
            //DRV_HID_DBG(DrvHidCore_printData("Y ==> %x", sMouseReport.rel_y/*sMouseReport[u8Interface].rel_y*/));
        }        
    }
    else if(mouse_size==0x10)
    {
        
    }
    else
    {    //DRV_HID_DBG(DrvHidCore_printData("ERROR:report size ==> 0x%x",report_size));
    }
    
    //DRV_HID_DBG(DrvHidCore_printData("report size ==> %x",report_size));
    //DRV_HID_DBG(DrvHidCore_printData("Mouse Report_%x:",u8Interface));
    //DRV_HID_DBG(DrvHidCore_printData("Logical Max ==> %x",logical_max));
    //DRV_HID_DBG(DrvHidCore_printData("Button ==> %x", sMouseReport.button/*sMouseReport[u8Interface].button*/));
    //DRV_HID_DBG(DrvHidCore_printData("X ==> %x", sMouseReport.rel_x/*sMouseReport[u8Interface].rel_x*/));
    //DRV_HID_DBG(DrvHidCore_printData("Y ==> %x", sMouseReport.rel_y/*sMouseReport[u8Interface].rel_y*/));
    //DRV_HID_DBG(DrvHidCore_printData("WHEEL ==> %x", sMouseReport.rel_wheel/*sMouseReport[u8Interface].rel_wheel*/));
    #if defined(_DUMMY_LAPTOP_)
    {   extern void MHL_SendMousePacket(BYTE u8Port,struct mouse_struct *pMouse);
        MHL_SendMousePacket(1,&sMouseReport);
    }
    #endif

  }
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif

}

void  MDrv_Get_Keyboard_Staus(U8 u8Interface)
{
//  __u16 xdata tmp;
  __u8 i;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();        //switch window 1
  #endif

  flib_Host20_Issue_Interrupt((U16)pUsbData,8 , u8Interface);
  //Parse_Mouse_Report(pUsbData, &sMouseReport);
  if (gState_Change[u8Interface])
  {
    DrvHidCore_printData("Keyboard HID RAW_%x:",u8Interface);
    for (i=0 ; i <8/*counter*/ ; i++)
    {
       DrvHidCore_printData("%x ",pUsbData[i]);
    }
    #if defined(_DUMMY_LAPTOP_)
    {   extern void MHL_SendKeyboardPacket(BYTE u8Port,BYTE *pu8Buffer);
        MHL_SendKeyboardPacket(1,pUsbData);
    }
    #endif

  }
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif

}
#else
BYTE code drvhidcore[] = {0};
void mshidcoreDummy(void)
{
    BYTE xdata x = drvhidcore[0];
}
#endif
