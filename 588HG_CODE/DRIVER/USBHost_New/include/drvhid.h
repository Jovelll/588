
 typedef unsigned char   __u8;     // 1 byte
 typedef  char   __s8;     // 1 byte

/// data type unsigned short, data length 2 byte
 typedef unsigned int  __u16;    // 2 bytes
 typedef  int  __s16;    // 2 bytes

/// data type unsigned int, data length 4 byte
typedef unsigned long    __u32;    // 4 bytes
typedef  long    __s32;    // 4 bytes

/// data type signed char, data length 1 byte


/*
 * HID report item format
 */

#define HID_ITEM_FORMAT_SHORT   0
#define HID_ITEM_FORMAT_LONG    1

/*
 * Special tag indicating long items
 */

#define HID_ITEM_TAG_LONG   15

/*
 * HID report descriptor item type (prefix bit 2,3)
 */

#define HID_ITEM_TYPE_MAIN      0
#define HID_ITEM_TYPE_GLOBAL        1
#define HID_ITEM_TYPE_LOCAL     2
#define HID_ITEM_TYPE_RESERVED      3

/*
 * HID report descriptor main item tags
 */

#define HID_MAIN_ITEM_TAG_INPUT         8
#define HID_MAIN_ITEM_TAG_OUTPUT        9
#define HID_MAIN_ITEM_TAG_FEATURE       11
#define HID_MAIN_ITEM_TAG_BEGIN_COLLECTION  10
#define HID_MAIN_ITEM_TAG_END_COLLECTION    12

/*
 * HID report descriptor main item contents
 */

#define HID_MAIN_ITEM_CONSTANT      0x001
#define HID_MAIN_ITEM_VARIABLE      0x002
#define HID_MAIN_ITEM_RELATIVE      0x004
#define HID_MAIN_ITEM_WRAP      0x008
#define HID_MAIN_ITEM_NONLINEAR     0x010
#define HID_MAIN_ITEM_NO_PREFERRED  0x020
#define HID_MAIN_ITEM_NULL_STATE    0x040
#define HID_MAIN_ITEM_VOLATILE      0x080
#define HID_MAIN_ITEM_BUFFERED_BYTE 0x100

/*
 * HID report descriptor collection item types
 */

#define HID_COLLECTION_PHYSICAL     0
#define HID_COLLECTION_APPLICATION  1
#define HID_COLLECTION_LOGICAL      2

/*
 * HID report descriptor global item tags
 */

#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE      0
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM 1
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM 2
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM    3
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM    4
#define HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT   5
#define HID_GLOBAL_ITEM_TAG_UNIT        6
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE     7
#define HID_GLOBAL_ITEM_TAG_REPORT_ID       8
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT    9
#define HID_GLOBAL_ITEM_TAG_PUSH        10
#define HID_GLOBAL_ITEM_TAG_POP         11

/*
 * HID report descriptor local item tags
 */

#define HID_LOCAL_ITEM_TAG_USAGE        0
#define HID_LOCAL_ITEM_TAG_USAGE_MINIMUM    1
#define HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM    2
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX 3
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MINIMUM   4
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MAXIMUM   5
#define HID_LOCAL_ITEM_TAG_STRING_INDEX     7
#define HID_LOCAL_ITEM_TAG_STRING_MINIMUM   8
#define HID_LOCAL_ITEM_TAG_STRING_MAXIMUM   9
#define HID_LOCAL_ITEM_TAG_DELIMITER        10

/*
 * HID usage tables
 */

#define HID_USAGE_PAGE      0xffff0000

#define HID_UP_GENDESK      0x00010000
#define HID_UP_KEYBOARD     0x00070000
#define HID_UP_LED      0x00080000
#define HID_UP_BUTTON       0x00090000
#define HID_UP_CONSUMER     0x000c0000
#define HID_UP_DIGITIZER    0x000d0000
#define HID_UP_PID      0x000f0000

#define HID_USAGE       0x0000ffff

#define HID_GD_POINTER      0x00010001
#define HID_GD_MOUSE        0x00010002
#define HID_GD_JOYSTICK     0x00010004
#define HID_GD_GAMEPAD      0x00010005
#define HID_GD_KEYBOARD     0x00010006
#define HID_GD_KEYPAD       0x00010007
#define HID_GD_HATSWITCH    0x00010039

/*
 * HID report types --- Ouch! HID spec says 1 2 3!
 */

#define HID_INPUT_REPORT    0
#define HID_OUTPUT_REPORT   1
#define HID_FEATURE_REPORT  2

/*
 * HID device quirks.
 */

#define HID_QUIRK_INVERT    0x01
#define HID_QUIRK_NOTOUCH   0x02
#define HID_QUIRK_IGNORE    0x04
#define HID_QUIRK_NOGET     0x08

#define  USEAGE_X                   0x30
#define  USEAGE_Y                   0x31
#define  USEAGE_Z                   0x32
#define  USEAGE_WHEEL          0x38

#define  USEAGE_UNDEFINED   0
#define  MAX_AXIS_NUM          6
#define  MAX_BUTTON_NUM        2


#define  USEAGE_PAGE_KEYBOARD  7
#define  USEAGE_PAGE_LEDS      8
#define  USEAGE_PAGE_BUTTON         9
/*
 * We parse each description item into this structure. Short items data
 * values are expanded to 32-bit signed int, long items contain a pointer
 * into the data area.
 */

struct hid_item {
    __u16      format;
    __u16      size;
    __u16      type;
    __u16      tag;
    union {
        __u8   u8;
        __s8   s8;
        __u16  u16;
        __s16  s16;
        __u32  u32;
        __s32  s32;
        __u8  *longdata;
    } zdata;
};
struct hid_global {
    __u32        usage_page;
    __s32       logical_minimum;
    __s32       logical_maximum;
    __s32       physical_minimum;
    __s32       physical_maximum;
    __u16        unit_exponent;
    __u16        unit;
    __u16        report_id;
    __u16        report_size;
    __u16        report_count;
};
/*
 * This is the collection stack. We climb up the stack to determine
 * application and function of each field.
 */

struct hid_collection {
    __u16    type;
    __u16    usage;
};

/*
 * This is the local enviroment. It is resistent up the next main-item.
 */

#define HID_MAX_DESCRIPTOR_SIZE     4096
#define HID_MAX_USAGES          16
#define HID_MAX_APPLICATIONS        16

struct hid_local {
    __u16    usage[HID_MAX_USAGES]; /* usage array */
    __u16    usage_index;
    __u16    usage_minimum;
    __u16    delimiter_depth;
    __u16    delimiter_branch;
};

#define HID_GLOBAL_STACK_SIZE 4
#define HID_COLLECTION_STACK_SIZE 4
struct hid_parser {
    struct hid_global     global;
    struct hid_global     global_stack[HID_GLOBAL_STACK_SIZE];
    unsigned              global_stack_ptr;
    struct hid_local      local;
    struct hid_collection collection_stack[HID_COLLECTION_STACK_SIZE];
    unsigned              collection_stack_ptr;
    //struct hid_device    *device;
};
#define   TYPE_NOEVENT        0
#define   TYPE_AXIS_X           1
#define   TYPE_AXIS_Y           2
#define   TYPE_AXIS_WHEEL     3
#define   TYPE_BUTTON         4

struct axis_struct{
        __u16   size;
        __u16    val;
        __u16  startbit;
        __u16    type;
};
struct  button_struct {                 //for all one bit button
        __u16   count;
        __u16    val;
        __u16  startbit;
};
struct mouse_struct{
        __u16   button;
        __s16   rel_x;
        __s16   rel_y;
        __s16   rel_wheel;
};
struct  leds_struct {
        __u16   count;
        __u16   val;
};

__u32 GetValue(__u8 *buf, __u16 offset,__u8 size);

