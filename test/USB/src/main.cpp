// USB-HID - application that discovers attached USB devices to CH376s
// Copyright 2019 - Sourceror (Mario Smit)

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include <thread>
#include <chrono>

#define DEV_ADDRESS 1
#define BOOT_PROTOCOL 0
#define HID_CLASS 0x03
#define HID_BOOT 0x01
#define HID_KEYBOARD 0x01
#define HID_MOUSE 0x02

//#define   B115200 0010002
//#define   B230400 0010003
#define   B460800 0010004
#define   B500000 0010005
#define   B576000 0010006
#define   B921600 0010007
#define  B1000000 0010010
#define  B1152000 0010011
#define  B1500000 0010012
#define  B2000000 0010013
#define  B2500000 0010014
#define  B3000000 0010015
#define  B3500000 0010016
#define  B4000000 0010017
#define BAUDRATE B1000000
int serial=-1;

#define CH375_CMD_SET_USB_SPEED 0x04
#define CH375_CMD_RESET_ALL 0x05
#define CH375_CMD_CHECK_EXIST 0x06
#define CH375_CMD_SET_USB_MODE 0x15
#define CH375_CMD_GET_STATUS 0x22
#define CH375_CMD_SET_USB_ADDR 0x13
#define CH375_CMD_SET_ENDP6 0x1C
#define CH375_CMD_SET_ENDP7 0x1D
#define CH375_CMD_RD_USB_DATA 0x27
//#define CH375_CMD_RD_USB_DATA 0x28
//#define CH375_CMD_WR_USB_DATA7 0x2B
#define CH375_CMD_WR_HOST_DATA 0x2C
#define CH376_CMD_SET_FILE_NAME 0x2F
#define CH376_CMD_DISK_CONNECT 0x30
#define CH376_CMD_DISK_MOUNT 0x31
#define CH376_CMD_OPEN_FILE 0x32
#define CH376_CMD_FILE_CLOSE 0x36
#define CH375_CMD_SET_ADDRESS 0x45
#define CH375_CMD_GET_DESCR 0x46
#define CH375_CMD_SET_CONFIG 0x49
#define CH375_CMD_ISSUE_TOKEN 0x4F
#define CH375_CMD_ISSUE_TKN_X 0x4E
#define CH375_CMD_ABORT_NAK 0x17
#define CH376_CMD_SET_RETRY 0x0B

#define CH375_USB_MODE_HOST 0x06
#define CH375_USB_MODE_HOST_RESET 0x07

#define CH375_USB_INT_SUCCESS 0x14
#define CH375_USB_INT_CONNECT 0x15
#define CH375_USB_INT_DISCONNECT 0x16
#define CH375_USB_INT_BUF_OVER 0x17
#define CH375_USB_INT_USB_READY 0x18

#define CH375_CMD_RET_SUCCESS 0x51
#define CH375_CMD_RET_ABORT 0x52

#define CH375_USB_DEVICE_DESCRIPTOR 0x01
#define CH375_USB_CONFIGURATION_DESCRIPTOR 0x02
#define CH375_USB_INTERFACE_DESCRIPTOR 0x04
#define CH375_USB_ENDPOINT_DESCRIPTOR 0x05

//--- PIDs
#define CH_PID_SETUP 0x0D
#define CH_PID_IN  0x09
#define CH_PID_OUT 0x01

typedef struct __attribute__((packed)) _USB_DEVICE_DESCRIPTOR {
     uint8_t bLength;
     uint8_t bDescriptorType;
     uint16_t bcdUSB;
     uint8_t bDeviceClass;
     uint8_t bDeviceSubClass;
     uint8_t bDeviceProtocol;
     uint8_t bMaxPacketSize0;
     uint16_t idVendor;
     uint16_t idProduct;
     uint16_t bcdDevice;
     uint8_t iManufacturer;
     uint8_t iProduct;
     uint8_t iSerial;
     uint8_t bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;
USB_DEVICE_DESCRIPTOR device;

 typedef struct __attribute__((packed)) _USB_CONFIG_DESCRIPTOR {
     uint8_t bLength;
     uint8_t bDescriptorType;
     uint16_t wTotalLength;
     uint8_t bNumInterfaces;
     uint8_t bConfigurationvalue;
     uint8_t iConfiguration;
     uint8_t bmAttributes;
     uint8_t bMaxPower;
 } USB_CONFIG_DESCRIPTOR;

typedef struct __attribute__((packed)) _USB_HID_DESCRIPTOR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t hid_version;
    uint8_t country_code;
    uint8_t num_descriptors;
    uint8_t descriptor_type;
    uint16_t descriptor_length;
} USB_HID_DESCRIPTOR;

 typedef struct __attribute__((packed)) _USB_INTERF_DESCRIPTOR {
     uint8_t bLength;
     uint8_t bDescriptorType;
     uint8_t bInterfaceNumber;
     uint8_t bAlternateSetting;
     uint8_t bNumEndpoints;
     uint8_t bInterfaceClass;
     uint8_t bInterfaceSubClass;
     uint8_t bInterfaceProtocol;
     uint8_t iInterface;
 } USB_INTERF_DESCRIPTOR;

 typedef struct __attribute__((packed)) _USB_ENDPOINT_DESCRIPTOR {
     uint8_t bLength;
     uint8_t bDescriptorType;
     uint8_t bEndpointAddress;
     uint8_t bmAttributes;
     uint16_t wMaxPacketSize;
     uint8_t bInterval;
 } USB_ENDPOINT_DESCRIPTOR;

typedef struct __attribute__((packed)) _USB_ETHERNET_DESCRIPTOR {
     uint8_t bLength;
     uint8_t bDescriptorType;
     uint8_t bDescriptorSubType;
     uint8_t iMACAddress;
     uint32_t bmEthernetStatistics;
     uint16_t wMaxSegmentSize;
     uint16_t wNumberMCFilters;
     uint8_t bNumberPowerFilters;
 } USB_ETHERNET_DESCRIPTOR;

typedef struct __attribute__((packed)) _USB_ETHERNET_HEADER_DESCRIPTOR {
     uint8_t bLength;
     uint8_t bDescriptorType;
     uint8_t bDescriptorSubType;
     uint16_t bcdCDC;
 } USB_ETHERNET_HEADER_DESCRIPTOR;

typedef struct __attribute__((packed)) _USB_ETHERNET_UNION_DESCRIPTOR {
     uint8_t bLength;
     uint8_t bDescriptorType;
     uint8_t bDescriptorSubType;
     uint8_t bControlInterface;
     uint8_t bSubordinateInterface0;
     uint8_t bSubordinateInterface1;
     uint8_t bSubordinateInterface2;
     uint8_t bSubordinateInterface3;
 } USB_ETHERNET_UNION_DESCRIPTOR;

typedef struct __attribute__((packed)) _USB_HUB_DESCRIPTOR 
{
     uint8_t bLength;
     uint8_t bDescriptorType;
     uint8_t bNrPorts;
     uint16_t wHubCharacteristics;
     uint8_t bPwrOn2PwrGood;
     uint8_t bHubContrCurrent;
     uint8_t DeviceRemovable;
     uint8_t PortPwrCtrlMask;
} USB_HUB_DESCRIPTOR;

struct _hid_info 
{
    uint8_t mouse_endpoint,mouse_millis,mouse_interface=255;
    uint16_t mouse_packetsize;
    uint8_t keyboard_endpoint,keyboard_millis,keyboard_interface=255;
    uint16_t keyboard_packetsize;
    uint8_t hid_configuration_id=0;
}* hid_info=NULL;
struct _ethernet_info 
{
    uint8_t ethernet_data_interface=255,ethernet_control_interface=255;
    uint8_t ethernet_interrupt_endpoint,ethernet_bulk_in_endpoint,ethernet_bulk_out_endpoint;
    uint8_t ethernet_interrupt_millis;
    uint16_t ethernet_interrupt_packetsize,ethernet_bulk_in_packetsize,ethernet_bulk_out_packetsize;
    uint8_t ethernet_configuration_id=0;
    uint8_t ethernet_alternate_setting;
}* ethernet_info=NULL;
struct _hub_info
{
    uint8_t hub_interface=255;
    uint8_t hub_configuration_id=0;
    uint8_t hub_interrupt_endpoint;
    uint16_t hub_interrupt_packetsize;
    uint8_t hub_interrupt_millis;
}* hub_info=NULL;

void print_buffer (uint8_t* data, uint16_t length)
{
    for (int i=0;i<length;i++)
    {
        if ((i%16)==0)
            printf ("\n");
        if ((i%4)==0)
            printf (" ");
        printf ("%02x ",*(data+i));
    }
    printf ("\n");
}

void error (char const * msg)
{
    printf ("ERROR: %s\n",msg);
    exit(0);
}

void init_serial ()
{
    const char device[] = "/dev/tty.usbmodem123451";
    serial = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(serial == -1)
      error( "failed to open port" );
    if(!isatty(serial))
      error( "not serial" );
    fcntl(serial, F_SETFL, 0);
    
    struct termios  config;
    if(tcgetattr(serial, &config) < 0)
      error("cannot get serial attributes");
    
    bzero(&config, sizeof(config));
    config.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    config.c_iflag = IGNPAR;
    config.c_oflag = 0;
    
    /* set input mode (non-canonical, no echo,...) */
    config.c_lflag = 0;
     
    config.c_cc[VTIME]    = 5;
    config.c_cc[VMIN]     = 0;
    
    tcflush(serial, TCIFLUSH);
    tcsetattr(serial, TCSANOW, &config);
}

// LOW_LEVEL serial communication to CH376
///////////////////////////////////////////////////////////////////////////
const uint8_t WR_COMMAND = 1;
const uint8_t RD_STATUS = 2;
const uint8_t WR_DATA = 3;
const uint8_t RD_DATA = 4;
const uint8_t RD_INT = 5;
const uint8_t RD_DATA_MULTIPLE = 6;
const uint8_t WR_DATA_MULTIPLE = 7;

void writeCommand (uint8_t command)
{
    uint8_t cmd[] = {WR_COMMAND,command};
    write (serial,cmd,sizeof(cmd));
}
void writeData (uint8_t data)
{
    uint8_t cmd[] = {WR_DATA,data};
    write (serial,cmd,sizeof(cmd));
}
void writeDataMultiple (uint8_t* buffer,uint8_t len)
{
    for (int i=0;i<len;i++)
    {
        writeData (buffer[i]);
    }
    writeData (0);
}
ssize_t readData (uint8_t* new_value)
{
    uint8_t cmd[] = {RD_DATA};
    write (serial,cmd,sizeof(cmd));
    return read (serial,new_value,1);
}
ssize_t readDataMultiple (uint8_t* buffer,uint8_t len)
{
    int i;
    uint8_t cmd[] = {RD_DATA_MULTIPLE,len};
    write (serial,cmd,sizeof(cmd));
    for (i=0;i<len;i++)
    {
        uint8_t value;
        uint8_t bytes = read (serial,&value,1);
        if (bytes==0)
            break;
        *(buffer+i)=value;
    }
    return i;
}
ssize_t readStatus (uint8_t* new_value)
{
    uint8_t cmd[] = {RD_STATUS};
    write (serial,cmd,sizeof(cmd));
    return read (serial,new_value,1);
}
ssize_t readInterrupt (uint8_t* new_value)
{
    uint8_t cmd[] = {RD_INT};
    write (serial,cmd,sizeof(cmd));
    return read (serial,new_value,1);
}

// CHECK STATUS
///////////////////////////////////////////////////////////////////////////
uint8_t waitInterrupt ()
{
    uint8_t status,interrupt;
    int i=0;
    ssize_t bytes;
    while ((bytes=readInterrupt(&interrupt)))
    {
        if (interrupt&0x80)
            break;
    }
    writeCommand(CH375_CMD_GET_STATUS);
    bytes = readData (&status);
    if (bytes)
        return status;
    return 0;
}
void check_exists ()
{
    uint8_t value = 190;
    uint8_t new_value;
    //char cmd[] = {0x57,0xAB,CH375_CMD_CHECK_EXIST,value};
    writeCommand (CH375_CMD_CHECK_EXIST);
    writeData(value);
    ssize_t bytes = readData (&new_value);
    value = value ^ 255;
    if (bytes!=1 || (new_value != value))
        error ("Device does not exist\n");
}

// CH376 built-in commands
///////////////////////////////////////////////////////////////////////////
void reset_all ()
{
    writeCommand (CH375_CMD_RESET_ALL);
}
void set_target_device_address (uint8_t address)
{
    writeCommand (CH375_CMD_SET_USB_ADDR);
    writeData(address);
    usleep (2000);
}

bool set_usb_host_mode (uint8_t mode)
{
    writeCommand(CH375_CMD_SET_USB_MODE);
    writeData(mode);
    
    uint8_t value;
    for(int i=0; i!=200; i++ )
    {
        readData(&value);
        if ( value == CH375_CMD_RET_SUCCESS )
            return true;
        usleep(1000);
    }
    return false;
}

//Bits 7 and 6:
//  0x: Don't retry NAKs
//  10: Retry NAKs indefinitely (default)
//  11: Retry NAKs for 3s
//Bits 5-0: Number of retries after device timeout
//Default after reset and SET_USB_MODE is 8Fh
void set_retry (uint8_t mode)
{
    writeCommand (CH376_CMD_SET_RETRY);
    writeData (0x25); // fixed value, required
    writeData (mode);
}
void set_speed (uint8_t speed)
{
    writeCommand (CH375_CMD_SET_USB_SPEED);
    writeData(speed);
}
void set_address (uint8_t address)
{
     writeCommand (CH375_CMD_SET_ADDRESS);
     writeData(address);
     if (waitInterrupt ()!=CH375_USB_INT_SUCCESS)
         error ("ERROR: address not set\n");
}
void set_configuration (uint8_t configuration)
{
     writeCommand(CH375_CMD_SET_CONFIG);
     writeData(configuration);
     if (waitInterrupt ()!=CH375_USB_INT_SUCCESS)
         error ("ERROR: configuration not set\n");
}
bool get_device_descriptor ()
{
    uint8_t value;
    ssize_t bytes;
    
    writeCommand(CH375_CMD_GET_DESCR);
    writeData(CH375_USB_DEVICE_DESCRIPTOR);
    if (waitInterrupt ()!=CH375_USB_INT_SUCCESS)
        error ("ERROR: USB device descriptor not read 1\n");
    writeCommand(CH375_CMD_RD_USB_DATA);
    // read length of return package
    value = 0;
    bytes = readData(&value);
    if (bytes==1 && value == sizeof (USB_DEVICE_DESCRIPTOR))
    {
        bytes = readDataMultiple ((uint8_t*)&device,sizeof (USB_DEVICE_DESCRIPTOR));
        if (bytes!=value)
            error ("ERROR: USB device descriptor not read 2\n");
    }
    else
        error ("ERROR: USB device descriptor not read 3\n");
    return true;
}

// Higher level USB read/write
///////////////////////////////////////////////////////////////////////////
void write_usb_data (uint8_t* message,uint8_t length)
{
    writeCommand (CH375_CMD_WR_HOST_DATA);
    writeData(length);
    for (int i=0;i<length;i++)
        writeData(message[i]);
}
void issue_token (uint8_t endpoint_number, uint8_t pid, uint8_t in_toggle, uint8_t out_toggle)
{
    writeCommand(CH375_CMD_ISSUE_TKN_X);
    writeData(in_toggle<<7 | out_toggle<<6);
    writeData(endpoint_number << 4 | pid);
}
ssize_t read_usb_data (uint8_t* pBuffer)
{
    uint8_t value = 0;
    writeCommand(CH375_CMD_RD_USB_DATA);
    ssize_t bytes = readData(&value);
    if (bytes==0)
        error ("no data available");
    if (value==0)
        return 0;
    bytes = readDataMultiple(pBuffer, value);
    if (bytes<value)
        error("did not receive enough bytes");
    return bytes;
}

// USB data packet input
///////////////////////////////////////////////////////////////////////////
int data_in_transfer (uint16_t length, uint8_t target_device_address, uint8_t endpoint_number, uint8_t endpoint_packetsize, uint8_t& endpoint_toggle,uint8_t* &result)
{
    uint16_t remaining_data_length = length;
    result = (uint8_t*) malloc (length);
    bzero(result, length);
    uint8_t* pTemp = result;
    uint8_t status;
    int bytes_read=0;
    
    set_target_device_address(target_device_address);
    
    while (remaining_data_length>0)
    {
        issue_token(endpoint_number, CH_PID_IN, endpoint_toggle, 0);
        endpoint_toggle = endpoint_toggle ^ 1;
        if ((status=waitInterrupt())!=CH375_USB_INT_SUCCESS)
        {
            if ((status&0x2f)==0b00101010) // 2A
            {
                //printf (">> NAK <<\n");
                free (result);
                result = NULL;
                return 0;
            }
            if ((status&0x2f)==0b00101110) // 2E
            {
                printf (">> STALL <<\n");
                free (result);
                result = NULL;
                return 0;
            }
            if ((status&0x23)==0b00100000) // 20
            {
                printf (">> OVER TIME <<\n");
                free (result);
                result = NULL;
                return 0;
            }
            printf (">> UNEXPECTED 0x%x <<\n",status);
            endpoint_toggle = endpoint_toggle ^ 1; // extra toggle, maybe out of sync?
        }; 
        bytes_read = read_usb_data (pTemp);
        remaining_data_length -= bytes_read;
        pTemp += bytes_read;
        
        if (bytes_read<endpoint_packetsize)
            break;
    }
    return length-remaining_data_length;
}

// USB data packet output
///////////////////////////////////////////////////////////////////////////
void data_out_transfer (uint8_t* pBuffer,uint16_t length, uint8_t target_device_address, uint8_t endpoint_number, uint8_t endpoint_packetsize, uint8_t endpoint_toggle)
{
    uint8_t* pTemp = pBuffer;
    uint16_t remaining_data_length = length;
    set_target_device_address(target_device_address);
    
    while (remaining_data_length>0)
    {
        int amountwritten=endpoint_packetsize<remaining_data_length?endpoint_packetsize:remaining_data_length;
        write_usb_data(pTemp, amountwritten);
        pTemp+=amountwritten;
        remaining_data_length-=amountwritten;
        endpoint_toggle = endpoint_toggle ^ 1;
        
        issue_token(endpoint_number, CH_PID_OUT, 0, endpoint_toggle);
        while ((waitInterrupt()&0x2f)==0x2a); // absorb NAK
    }
}

#define IN 1
#define OUT 0
// USB command execution via SETUP, DATA, STATUS stages
///////////////////////////////////////////////////////////////////////////
bool execute_control_transfer (uint8_t target_device_address,uint8_t message[8],uint8_t* senddata, uint8_t endpoint_packet_size,uint8_t endpoint,uint8_t* &result)
{
    uint16_t requested_length = (*(message + 6))+((*(message +7))<<8);
    uint8_t data_direction = message[0]&0b10000000?IN:OUT;

    //print_buffer (message,8);
    set_target_device_address(target_device_address);
    
    //Setup stage
    write_usb_data (message,8);
    //uint8_t endpoint = 0;
    issue_token (endpoint, CH_PID_SETUP,0,0);
    uint8_t interrupt = waitInterrupt();
    if (interrupt != CH375_USB_INT_SUCCESS)
        return false;//error ("execute_control_transfer 0");
    
    //Data stage
    uint8_t endpoint_toggle = 1;
    if (data_direction==IN)
    {
        int bytes_read = data_in_transfer(requested_length, target_device_address, 0, endpoint_packet_size, endpoint_toggle,result);
        if (result==NULL)
            return false;
    }
    else
        data_out_transfer (senddata,requested_length, target_device_address, 0, endpoint_packet_size, endpoint_toggle);
    
    // Status stage
    if (data_direction==OUT || requested_length==0)
    {
        issue_token(endpoint, CH_PID_IN, 1, 0);
        while ((waitInterrupt()&0x2f)==0x2a); // absorb NAK
        uint8_t tmp[endpoint_packet_size];
        bzero (tmp,endpoint_packet_size);
        ssize_t bytes = read_usb_data((uint8_t*)&tmp);
        //print_buffer (tmp,endpoint_packet_size);
    }
    else
    {
        issue_token(endpoint, CH_PID_OUT, 0, 1);
        uint8_t interrupt;
        while (true)
        {
            interrupt = waitInterrupt();
            if ((interrupt&0x2f)==0x2a) // absorb NAK
                continue;
            break;
        };
        if (interrupt!=CH375_USB_INT_SUCCESS)
            return false;//error ("ERROR: execute_control_transfer 2");
    }
    return true;
}

// DESCRIPTOR commands
///////////////////////////////////////////////////////////////////////////
uint8_t max_packet_size;
bool get_device_descriptor2 (uint8_t target_device_address,uint8_t* buffer, int& buffer_length)
{
    uint8_t cmd[] = {0x80,6,0,1,0,0,18,0};
    uint8_t* data=NULL;
    max_packet_size = 8;

    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    if (result)
    {
        print_buffer (data,*(data));
        memcpy(buffer, data, *(data));
        memcpy(&device, data, *(data));
        buffer_length=*(data);
        max_packet_size = device.bMaxPacketSize0;
    }
    free(data);
    return result;
}
bool get_configuration_descriptor2 (uint8_t target_device_address,uint8_t config_id,uint8_t* buffer,int &buffer_length)
{
    uint8_t cmd[] = {0x80,6,config_id,2,0,0,sizeof(USB_CONFIG_DESCRIPTOR),0};
    uint8_t* data=NULL;
    bool result2 = false;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    if (result)
    {
        USB_CONFIG_DESCRIPTOR* config = (USB_CONFIG_DESCRIPTOR*) data;
        uint8_t cmd2[] = {0x80,6,config_id,2,0,0,(uint8_t)config->wTotalLength,0};
        uint8_t* data2=NULL;
        result2 = execute_control_transfer(target_device_address,cmd2,NULL,max_packet_size,0,data2);
        if (result2)
        {
            memcpy (buffer, data2,config->wTotalLength);
            buffer_length += config->wTotalLength;
            print_buffer (data2,config->wTotalLength);
        }
        free (data2);
    }
    free (data);
    return result && result2;
}

std::string utf16_to_utf8(std::u16string const& s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff>, char16_t> cnv;
    std::string utf8 = cnv.to_bytes(s);
    if(cnv.converted() < s.size())
        throw std::runtime_error("incomplete conversion");
    return utf8;
}

const char no_data[] = "{no data}";
std::string get_string2 (uint8_t target_device_address,uint8_t string_id)
{
    if (string_id==0)
        return no_data;
    
    uint8_t cmd[] = {0x80,6,string_id,3,0,0,255,0};
    uint8_t* data=NULL;
    std::string str8;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    if (result)
    {
        uint8_t len = data[0];
        uint8_t type = data[1];
        std::u16string str = (char16_t*) (data+2);
        str8 = utf16_to_utf8(str);
    }
    free (data);
    return str8;
}

// USB HID COMMANDS
///////////////////////////////////////////////////////////////////////////
bool set_protocol2 (uint8_t target_device_address,uint8_t protocol_id,uint8_t interface)
{
    uint8_t cmd[] = {0x21,0x0B,protocol_id,0,interface,0,0,0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    free (data);
    return result;
}
bool set_idle2 (uint8_t target_device_address,uint8_t duration,uint8_t report_id, uint8_t interface)
{
    uint8_t cmd[] = {0x21,0x0A,report_id,duration,interface,0,0,0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    free (data);
    return result;
}

// USB COMMANDS
///////////////////////////////////////////////////////////////////////////
bool set_address2 (uint8_t target_device_address)
{
    uint8_t cmd[] = {0x00,0x05,target_device_address,0,0,0,0,0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(0,cmd,NULL,max_packet_size,0,data);
    free (data);
    return result;
}
bool set_configuration2 (uint8_t target_device_address,uint8_t configuration_id)
{
    uint8_t cmd[] = {0x00,0x09,configuration_id,0,0,0,0,0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    free (data);
    return result;
}
bool set_interface2 (uint8_t target_device_address,uint8_t interface_id,uint8_t alternative_setting)
{
    uint8_t cmd[] = {0x01,11,alternative_setting,0,interface_id,0,0,0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    free (data);
    return result;
}
bool set_packet_filter2 (uint8_t target_device_address,uint8_t interface_id,uint8_t packet_filter)
{
    uint8_t cmd[] = {0b00100001,0x43,packet_filter,0,interface_id,0,0,0}; // set ethernet packet filter
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    free (data);
    return result;
    }

///////////////////////////////////////////////////////////////////////////
// USB HUB commands
bool get_hub_descriptor2 (uint8_t target_device_address,uint8_t* buffer, int& buffer_length)
{
    //bmRequestType, bRequest, wValue, wIndex, wLength
    uint8_t cmd[] = {0b10100000,6,0,0x29,0,0,sizeof(USB_HUB_DESCRIPTOR),0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    if (result)
    {
        print_buffer (data,*(data));
        memcpy(buffer, data, *(data));
        buffer_length+=*(data);
    }
    free(data);
    return result;
}
uint32_t get_hub_portstatus (uint8_t target_device_address,uint8_t portnr)
{
    //bmRequestType, bRequest, wValue, wIndex, wLength
    uint8_t cmd[] = {0b10100011,0,0,0,portnr,0,4,0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    uint32_t retval = 0;
    if (result)
    {
        //print_buffer (data,4);
        memcpy (&retval,data,4);

        uint16_t portstatus = 0;
        memcpy (&portstatus,data,2);
        printf ("Port %d status: ",portnr);
        std::string status="";
        if (portstatus & 0b1)
            status += "connected,";
        if (portstatus & 0b10)
            status += "enabled,";
        if (portstatus & 0b100)
            status += "suspended,";
        if (portstatus & 0b1000)
            status += "reset,";
        if (portstatus & 0b100000000)
            status += "powered,";
        if (portstatus & 0b1000000000)
            status += "low-speed,";
        if (portstatus & 0b10000000000)
            status += "high-speed,";
        if (portstatus & 0b1000000000000)
            status += "indicator control";
        printf ("%s\n",status.c_str());

        uint16_t portchange = 0;
        memcpy (&portchange,data+2,2);
        printf ("Port %d change: ",portnr);
        status="";
        if (portchange & 0b1)
            status += "C_PORT_CONNECTION,";
        if (portchange & 0b10)
            status += "C_PORT_ENABLE,";
        if (portchange & 0b100)
            status += "C_PORT_SUSPEND,";
        if (portchange & 0b1000)
            status += "C_PORT_OVER_CURRENT,";
        if (portchange & 0b10000)
            status += "C_PORT_RESET,";
        printf ("%s\n",status.c_str());

    }
    free(data);
    return retval;
}
bool set_hub_port_feature2 (uint8_t target_device_address,uint8_t feature_selector,uint8_t port,uint8_t value)
{
    //bmRequestType, bRequest, wValue, wIndex, wLength
    uint8_t cmd[] = {0b00100011,0x03,feature_selector,0,port,value,0,0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    free (data);
    return result;
}
bool clear_hub_port_feature2 (uint8_t target_device_address,uint8_t feature_selector,uint8_t port,uint8_t value)
{
    //bmRequestType, bRequest, wValue, wIndex, wLength
    uint8_t cmd[] = {0b00100011,0x01,feature_selector,0,port,value,0,0};
    uint8_t* data=NULL;
    bool result = execute_control_transfer(target_device_address,cmd,NULL,max_packet_size,0,data);
    free (data);
    return result;
}
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// USB HID BOOT DEVICE INTERRUPT READOUT
bool read_boot_mouse (uint8_t target_device_address,uint8_t mouse_endpoint_id,uint8_t mouse_millis,uint16_t mouse_in_packetsize)
{
    uint8_t endpoint_toggle = 0;
    while (true)
    {
        uint8_t* buffer=NULL;
        int bytes_read = data_in_transfer(mouse_in_packetsize, target_device_address,mouse_endpoint_id, max_packet_size, endpoint_toggle,buffer);
        if (bytes_read==0 || buffer==NULL)
            return false;
        uint8_t buttonstate = buffer[0];
        uint8_t x = buffer[1];
        uint8_t y = buffer[2];
        printf ("left: %d, right: %d, middle: %d - X: %d, Y: %d\n",buttonstate&0x01,buttonstate&0x02,buttonstate&0x04,x,y);
        free (buffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(mouse_millis));
        if (buttonstate&0x02) //right button
            return false;
    }
    return true;
}
bool read_boot_keyboard (uint8_t target_device_address,uint8_t endpoint_id,uint8_t millis,uint16_t in_packetsize)
{
    uint8_t endpoint_toggle = 0;
    while (true)
    {
        uint8_t* buffer = NULL;
        int bytes_read = data_in_transfer(in_packetsize, target_device_address,endpoint_id, max_packet_size, endpoint_toggle,buffer);
        if (bytes_read==0 || buffer==NULL)
            return false;
        uint8_t modifier_keys = buffer[0];
        uint8_t reserved = buffer[1];
        uint8_t keycode1 = buffer[2];
        uint8_t keycode2 = buffer[3];
        uint8_t keycode3 = buffer[4];
        uint8_t keycode4 = buffer[5];
        uint8_t keycode5 = buffer[6];
        uint8_t keycode6 = buffer[7];
        printf ("modifier_keys: %02X, keycode1:%02X, keycode2:%02X, keycode3:%02X, keycode4:%02X, keycode5:%02X, keycode6:%02X\n",modifier_keys,keycode1,keycode2,keycode3,keycode4,keycode5,keycode6);
        free (buffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        if (keycode1==0x14) // Q for quit
            return false;
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////
// USB ETHERNET DEVICE INTERRUPT READOUT
bool check_network_connection (uint8_t target_device_address,uint8_t endpoint_id,uint8_t millis,uint16_t in_packetsize)
{
    uint8_t endpoint_toggle = 0;
    int count = 500;
    bool result = false;

    struct _ecm_notification_event
    {
        uint8_t bmRequestType;
        uint8_t bNotificationCode;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
    } ecm_notification_event;
    while (--count > 0 && !result)
    {
        uint8_t* buffer = NULL; 
        int bytes_read = data_in_transfer(in_packetsize, target_device_address,endpoint_id, max_packet_size, endpoint_toggle,buffer);
        if (bytes_read==0 || buffer==NULL)
            return false;
        memcpy (&ecm_notification_event,buffer,sizeof (_ecm_notification_event));
        //print_buffer (buffer,in_packetsize);
        if (ecm_notification_event.bmRequestType == 0b10100001) // device to host, class, endpoint communication
        {
            switch (ecm_notification_event.bNotificationCode)
            {
                case 0x00: // NETWORK_CONNECTION
                    //if (ecm_notification_event.wValue==0)
                    //    printf ("network cable DISCONNECTED on interface %d\n",*(buffer+4));
                    if (ecm_notification_event.wValue==1)
                        result = true;
                    //    printf ("network cable CONNECTED on interface %d\n",*(buffer+4));
                    break;
                case 0x01: // RESPONSE_AVAILABLE
                    //printf ("response available on interface %d\n",*(buffer+4));
                    break;
                case 0x2A: // CONNECTION_SPEED_CHANGE
                    //long speed_down = *(buffer+8) + *(buffer+9)*256+ *(buffer+10)*65536 + *(buffer+11)*4294967296;
                    //long speed_up = *(buffer+12) + *(buffer+13)*256+ *(buffer+14)*65536 + *(buffer+15)*4294967296;
                    //printf ("connection speed change on interface %d, %ld Mbps down / %ld Mbps up\n",*(buffer+4),speed_down/(8*1024*1024),speed_up/(8*1024*1024));
                    break;
            }
        }
        free (buffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    }
    return result;
}

void dump_in_packets (uint8_t target_device_address,uint8_t endpoint_id,uint16_t in_packetsize)
{
    uint8_t endpoint_toggle = 0;
    int count = 20;
    in_packetsize=1514;

    //set_retry (0x0f);    
    while (count)
    {
        uint8_t* buffer = NULL;
        int bytes_read = data_in_transfer(in_packetsize, target_device_address,endpoint_id, max_packet_size, endpoint_toggle,buffer);
        if (buffer==NULL)
            continue;
        
        printf ("\n%d - dst: %02x:%02x:%02x:%02x:%02x:%02x - src: %02x:%02x:%02x:%02x:%02x:%02x - type: %04x",
            count,
            *(buffer+0),*(buffer+1),*(buffer+2),*(buffer+3),*(buffer+4),*(buffer+5), // dst
            *(buffer+6),*(buffer+7),*(buffer+8),*(buffer+9),*(buffer+10),*(buffer+11), // src
            ((*(buffer+12))<<8)+((*(buffer+13))));// EtherType

        if (!(bytes_read == 0 || buffer==NULL))
        {
            print_buffer (buffer,bytes_read);
            free (buffer);   
        }
        printf ("length: %d\n",bytes_read);
        count --;
    }
    //set_retry (0x8f);
}
///////////////////////////////////////////////////////////////////////////
// USB HUB DEVICE INTERRUPT READOUT
uint8_t get_hub_change (uint8_t target_device_address,uint8_t endpoint_id,uint8_t millis,uint16_t in_packetsize)
{
    uint8_t value = 0;
    uint8_t endpoint_toggle = 0;
    uint8_t* buffer = NULL;
    int bytes_read = data_in_transfer(in_packetsize, target_device_address,endpoint_id, max_packet_size, endpoint_toggle,buffer);
    if (bytes_read == 0 || buffer==NULL)
        return 0;
    //print_buffer (buffer,in_packetsize);
    value = buffer[0];
    free (buffer);   
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    return value;
}

bool get_descriptors (uint8_t address, uint8_t* buffer, int& buffer_length)
{
    uint8_t* buffer_ptr;

    bool result = get_device_descriptor2(0,buffer, buffer_length);
    if (!result)
    {
        // 2-set to 1.5 Mhz low-speed mode, 0-set to 12 Mhz high-speed mode (default)
        set_speed(2);
        result = get_device_descriptor2(address,buffer, buffer_length);
    }
    if (!result)
        error("unable to get device descriptor");

    if (!set_address2 (address))
            error ("error setting address");

    for (int i=0;i<device.bNumConfigurations;i++)
    {
        buffer_ptr = buffer+buffer_length;
        result = get_configuration_descriptor2(address,i,buffer_ptr,buffer_length);
        if (!result)
            error("unable to get config descriptor");
    }
    return true;
}
void print_descriptors (uint8_t device_address, uint8_t* buffer, int buffer_length)
{ 
    USB_DEVICE_DESCRIPTOR* device;
    USB_CONFIG_DESCRIPTOR* config;
    USB_INTERF_DESCRIPTOR* interface;
    USB_ENDPOINT_DESCRIPTOR* endpoint;
    USB_HID_DESCRIPTOR* hid;
    USB_ETHERNET_HEADER_DESCRIPTOR* header;
    USB_ETHERNET_UNION_DESCRIPTOR* unione;
    USB_ETHERNET_DESCRIPTOR* ethernet;
    USB_HUB_DESCRIPTOR* hub;

    uint8_t *ptr = buffer;
    while (ptr<buffer+buffer_length)
    {
        std::string endpoint_type;
        std::string endpoint_address_type;
        std::string interface_class_name;
        std::string interface_protocol_name;
        std::string interface_subclass_name;
        std::string device_class_name;
        printf ("\n");
        switch (*(ptr+1))
        {
            case 0x01: // DEVICE_DESCRIPTOR
                device = (USB_DEVICE_DESCRIPTOR*) ptr;
                printf ("Device Descriptor:\n");
                printf ("  bLength\t\t\t%d\n",device->bLength);
                printf ("  bDescriptorType\t\t%d\n",device->bDescriptorType);
                printf ("  bcdUSB\t\t\t%d.%d\n",(device->bcdUSB & 0xff00)>>8, device->bcdUSB & 0xff);
                if (device->bDeviceClass==0xff)
                    device_class_name = "Vendor Specific Class";
                if (device->bDeviceClass==0x02)
                    device_class_name = "Communications";
                if (device->bDeviceClass==0x03)
                    device_class_name = "HID";
                if (device->bDeviceClass==0x09)
                    device_class_name = "Hub";
                if (device->bDeviceClass==0x0A)
                    device_class_name = "CDC Data";
                printf ("  bDeviceClass\t\t\t%d\t%s\n",device->bDeviceClass,device_class_name.c_str());
                printf ("  bDeviceSubClass\t\t%d\n",device->bDeviceSubClass);
                printf ("  bDeviceProtocol\t\t%d\n",device->bDeviceProtocol);
                printf ("  bMaxPacketSize0\t\t%d\n",device->bMaxPacketSize0);
                printf ("  idVendor\t\t\t0x%04x\n",device->idVendor);
                printf ("  idProduct\t\t\t0x%04x\n",device->idProduct);
                printf ("  bcdDevice\t\t\t%d.%d\n",(device->bcdDevice & 0xff00)>>8, device->bcdDevice & 0xff);
                printf ("  iManufacturer\t\t\t%d\n",device->iManufacturer);
                printf ("  iProduct\t\t\t%d\t%s\n",device->iProduct,get_string2 (device_address,device->iProduct).c_str());
                printf ("  iSerial\t\t\t%d\t%s\n",device->iSerial,get_string2 (device_address,device->iSerial).c_str());
                printf ("  bNumConfigurations\t\t%d\n",device->bNumConfigurations);
                ptr += *ptr;
                break;
            case 0x02: // CONFIG_DESCRIPTOR
                config = (USB_CONFIG_DESCRIPTOR*) ptr;
                printf ("  Configuration Descriptor:\n");
                printf ("    bLength\t\t\t%d\n",config->bLength);
                printf ("    bDescriptorType\t\t%d\n",config->bDescriptorType);
                printf ("    wTotalLength\t\t%d\n",config->wTotalLength);
                printf ("    bNumInterfaces\t\t%d\n",config->bNumInterfaces);
                printf ("    bConfigurationvalue\t\t%d\n",config->bConfigurationvalue);
                printf ("    iConfiguration\t\t%d\t%s\n",config->iConfiguration,get_string2 (device_address,config->iConfiguration).c_str());
                printf ("    bmAttributes\t\t%d\n",config->bmAttributes);
                printf ("    bMaxPower\t\t\t%d\n",config->bMaxPower);
                device->bNumConfigurations--;
                ptr += *ptr;
                break;
            case 0x04: // interface descriptor
                interface = (USB_INTERF_DESCRIPTOR*) ptr;
                printf ("    Interface Descriptor:\n");
                printf ("      bLength\t\t\t%d\n",interface->bLength);
                printf ("      bDescriptorType\t\t%d\n",interface->bDescriptorType);
                printf ("      bInterfaceNumber\t\t%d\n",interface->bInterfaceNumber);
                printf ("      bAlternateSetting\t\t%d\n",interface->bAlternateSetting);
                printf ("      bNumEndpoints\t\t%d\n",interface->bNumEndpoints);
                if (interface->bInterfaceClass==0xff)
                    interface_class_name = "Vendor Specific Class";
                if (interface->bInterfaceClass==0x02)
                    interface_class_name = "Communications";
                if (interface->bInterfaceClass==0x03)
                    interface_class_name = "HID";
                if (interface->bInterfaceClass==0x09)
                    interface_class_name = "Hub";
                if (interface->bInterfaceClass==0x0A)
                    interface_class_name = "CDC Data";
                printf ("      bInterfaceClass\t\t%d\t%s\n",interface->bInterfaceClass,interface_class_name.c_str());
                if (interface->bInterfaceClass==0x03 && interface->bInterfaceSubClass==0x01)
                    interface_subclass_name = "Boot";
                if (interface->bInterfaceClass==0x02 && interface->bInterfaceSubClass==0x06)
                {
                    interface_subclass_name = "Ethernet Networking Control Model";
                    ethernet_info = new _ethernet_info();
                    ethernet_info->ethernet_control_interface = interface->bInterfaceNumber;
                }
                if (interface->bInterfaceClass==0x0A && interface->bInterfaceSubClass==0x00)
                {
                    ethernet_info->ethernet_data_interface = interface->bInterfaceNumber;
                    if (interface->bAlternateSetting!=0)
                        ethernet_info->ethernet_alternate_setting = interface->bAlternateSetting;
                }
                printf ("      bInterfaceSubClass\t%d\t%s\n",interface->bInterfaceSubClass,interface_subclass_name.c_str());
                if (interface->bInterfaceClass==0x03 && interface->bInterfaceProtocol==0x01)
                {
                    interface_protocol_name = "Keyboard";
                    if (!hid_info) hid_info = new _hid_info();
                    hid_info->keyboard_interface = interface->bInterfaceNumber;
                }
                if (interface->bInterfaceClass==0x03 && interface->bInterfaceProtocol==0x02)
                {
                    interface_protocol_name = "Mouse";
                    if (!hid_info) hid_info = new _hid_info();
                    hid_info->mouse_interface = interface->bInterfaceNumber;
                }
                if (interface->bInterfaceClass==0x09 && interface->bInterfaceProtocol==0x00)
                {
                    interface_protocol_name = "No TT";
                    if (!hub_info) hub_info = new _hub_info();
                    hub_info->hub_interface = interface->bInterfaceNumber;
                }
                if (interface->bInterfaceClass==0x09 && interface->bInterfaceProtocol==0x01)
                {
                    interface_protocol_name = "Single TT";
                    if (!hub_info) hub_info = new _hub_info();
                    hub_info->hub_interface = interface->bInterfaceNumber;
                }
                if (interface->bInterfaceClass==0x09 && interface->bInterfaceProtocol==0x02)
                {
                    interface_protocol_name = "Multi TT";
                    if (!hub_info) hub_info = new _hub_info();
                    hub_info->hub_interface = interface->bInterfaceNumber;
                }
                printf ("      bInterfaceProtocol\t%d\t%s\n",interface->bInterfaceProtocol,interface_protocol_name.c_str());
                printf ("      iInterface\t\t%d\t%s\n",interface->iInterface,get_string2 (device_address,interface->iInterface).c_str());
                ptr += *ptr;
                break;
            case 0x05: // endpoint descriptor
                endpoint = (USB_ENDPOINT_DESCRIPTOR*) ptr;
                printf ("    Endpoint Descriptor:\n");
                printf ("      bLength\t\t\t%d\n",endpoint->bLength);
                printf ("      bDescriptorType\t\t%d\n",endpoint->bDescriptorType);
                endpoint_address_type = endpoint->bEndpointAddress&0b10000000 ? "IN" : "OUT";
                printf ("      bEndpointAddress\t\t%d\t%s\n",endpoint->bEndpointAddress&0b01111111,endpoint_address_type.c_str());
                if ((endpoint->bmAttributes&0b00000011) == 0b00)
                    endpoint_type = "Control";
                if ((endpoint->bmAttributes&0b00000011) == 0b01)
                    endpoint_type = "Isochronous";
                if ((endpoint->bmAttributes&0b00000011) == 0b10)
                    endpoint_type = "Bulk";
                if ((endpoint->bmAttributes&0b00000011) == 0b11)
                    endpoint_type = "Interrupt";
                printf ("      bmAttributes\t\t%d\t%s\n",endpoint->bmAttributes,endpoint_type.c_str());
                printf ("      wMaxPacketSize\t\t%d\n",endpoint->wMaxPacketSize);
                printf ("      bInterval\t\t\t%d\n",endpoint->bInterval);
                if (interface->bInterfaceClass==0x03 && interface->bInterfaceProtocol==0x01) // keyboard
                {
                    hid_info->keyboard_endpoint = endpoint->bEndpointAddress&0b01111111;
                    hid_info->keyboard_packetsize = endpoint->wMaxPacketSize;
                    hid_info->keyboard_millis = endpoint->bInterval;
                    hid_info->hid_configuration_id = config->bConfigurationvalue;
                }
                if (interface->bInterfaceClass==0x03 && interface->bInterfaceProtocol==0x02) // mouse
                {
                    hid_info->mouse_endpoint = endpoint->bEndpointAddress&0b01111111;
                    hid_info->mouse_packetsize = endpoint->wMaxPacketSize;
                    hid_info->mouse_millis = endpoint->bInterval;
                    hid_info->hid_configuration_id = config->bConfigurationvalue;
                }
                if (interface->bInterfaceClass==0x02 && interface->bInterfaceSubClass==0x06) // ethernet control
                {
                    if ((endpoint->bmAttributes&0b00000011) == 0b11) // interrupt
                    {
                        ethernet_info->ethernet_interrupt_endpoint = endpoint->bEndpointAddress&0b01111111;
                        ethernet_info->ethernet_interrupt_millis = endpoint->bInterval;
                        ethernet_info->ethernet_interrupt_packetsize = endpoint->wMaxPacketSize;
                    }
                    ethernet_info->ethernet_configuration_id = config->bConfigurationvalue;
                }
                if (interface->bInterfaceClass==0x0a && interface->bInterfaceSubClass==0x00) // ethernet data
                {
                    if ((endpoint->bmAttributes&0b00000011) == 0b10 && endpoint->bEndpointAddress&0b10000000) // bulk IN
                    {
                        ethernet_info->ethernet_bulk_in_endpoint = endpoint->bEndpointAddress&0b01111111;
                        ethernet_info->ethernet_bulk_in_packetsize = endpoint->wMaxPacketSize;
                    }      
                    if ((endpoint->bmAttributes&0b00000011) == 0b10 && !(endpoint->bEndpointAddress&0b10000000)) // bulk OUT
                    {
                        ethernet_info->ethernet_bulk_out_endpoint = endpoint->bEndpointAddress&0b01111111;
                        ethernet_info->ethernet_bulk_out_packetsize = endpoint->wMaxPacketSize;
                    }                     
                }
                if (interface->bInterfaceClass==0x09 && (endpoint->bmAttributes&0b00000011) == 0b11) // interrupt
                {
                    hub_info->hub_interrupt_endpoint = endpoint->bEndpointAddress&0b01111111;
                    hub_info->hub_interrupt_packetsize = endpoint->wMaxPacketSize;
                    hub_info->hub_interrupt_millis = endpoint->bInterval;
                    hub_info->hub_configuration_id = config->bConfigurationvalue;
                }
                ptr += *ptr;
                break;
            case 0x21: // HID descriptor
                hid = (USB_HID_DESCRIPTOR*) ptr;
                printf ("      HID Descriptor:\n");
                printf ("        bLength\t\t\t%d\n",hid->bLength);
                printf ("        bDescriptorType\t\t%d\n",hid->bDescriptorType);
                printf ("        bcdHID\t\t\t%d.%d\n",(hid->hid_version & 0xff00)>>8, hid->hid_version & 0xff);
                printf ("        bCountryCode\t\t%d\n",hid->country_code);
                printf ("        bNumDescriptors\t\t%d\n",hid->num_descriptors);
                printf ("        bDescriptorType\t\t%d\n",hid->descriptor_type);
                printf ("        wDescriptorLength\t%d\n",hid->descriptor_length);
                ptr += *ptr;
                break;
            case 0x24: // CDC ECM / CS_INTERFACE
                printf ("    CDC ECM ");
                switch (*(ptr+2)) // subtype
                {
                    case 0x00: // header
                        printf ("header\n");
                        header = (USB_ETHERNET_HEADER_DESCRIPTOR*) ptr;
                        printf ("      bcdCDC\t\t\t%d.%d\n",(header->bcdCDC & 0xff00)>>8, header->bcdCDC & 0xff);
                        break;
                    case 0x06: // union
                        printf ("union\n");
                        unione = (USB_ETHERNET_UNION_DESCRIPTOR*) ptr;
                        printf ("      bControlInterface\t\t%d\n",unione->bControlInterface);
                        printf ("      bSubordinateInterface0\t%d\n",unione->bSubordinateInterface0);
                        break;
                    case 0x0f: // ethernet
                        printf ("ethernet\n");
                        ethernet = (USB_ETHERNET_DESCRIPTOR*) ptr;
                        printf ("      MAC\t\t\t%s\n",get_string2(device_address, ethernet->iMACAddress).c_str());
                        printf ("      wMaxSegmentSize\t\t%d\n",ethernet->wMaxSegmentSize);
                        break;
                    default:
                        printf ("unknown %02x\n",*(ptr+2));
                }
                ptr += *(ptr);
                break;
            case 0x29: // HUB
                hub = (USB_HUB_DESCRIPTOR*) ptr;
                printf ("  Hub Descriptor:\n");
                printf ("    bLength\t\t\t%d\n",hub->bLength);
                printf ("    bDescriptorType\t\t%x\n",hub->bDescriptorType);
                printf ("    bNrPorts\t\t\t%d\n",hub->bNrPorts);
                printf ("    wHubCharacteristics\t\t%x\n",hub->wHubCharacteristics);
                printf ("    bPwrOn2PwrGood\t\t%d\n",hub->bPwrOn2PwrGood);
                printf ("    bHubContrCurrent\t\t%d\n",hub->bHubContrCurrent);
                ptr += *(ptr);
                break;
            default:
                printf ("Unknown type: %02x\n",*(ptr+1));
                ptr += *ptr;
                break;
        }
    }
}
void do_hid (uint8_t device_address);
void do_ethernet (uint8_t device_address);
void do_hub (uint8_t device_address);

void init_device (uint8_t device_address)
{
    uint8_t buffer[1024];
    int buffer_length;
    hid_info=NULL;
    hub_info=NULL;
    ethernet_info=NULL;

    memset (buffer, 0, 1024);
    bool result = get_descriptors (device_address,buffer, buffer_length);
    if (!result)
        error("unable to get descriptors");

    print_descriptors (device_address, buffer, buffer_length);
    
    if (hid_info)
        do_hid (device_address);
    if (ethernet_info)
        do_ethernet (device_address);
    if (hub_info)
        do_hub (device_address);
}
void do_hid (uint8_t device_address)
{
    // USB HID?
    if (hid_info && hid_info->hid_configuration_id>0)
    {
        printf ("\nWe found an USB HID device, lets start using it.\n");
        if (hid_info->mouse_interface!=255 && hid_info->keyboard_interface!=255)
        {
            // enable device configuration
            if (!set_configuration2(device_address,hid_info->hid_configuration_id))
                error ("error setting HID configuration");
            // set boot protocol for mouse and read it
            //if (!set_protocol2 (device_address,BOOT_PROTOCOL,hid_info->mouse_interface)) // select boot protocol for our device
            //    error ("error setting boot protocol for mouse");
            //set_idle2(device_address, 0x0, 0,mouse_interface); // wait on change
            // set boot protocol for keyboard and read it
            if (!set_protocol2 (device_address,BOOT_PROTOCOL,hid_info->keyboard_interface)) // select boot protocol for our device
                error ("error setting boot protocol for keyboard");
            set_idle2(device_address, 0x80, 0,hid_info->keyboard_interface); // scan every ~500ms
            
            read_boot_keyboard (device_address,hid_info->keyboard_endpoint,hid_info->keyboard_millis,hid_info->keyboard_packetsize); // assume endpoint 2
            //read_boot_mouse (device_address,mouse_endpoint,mouse_millis,mouse_packetsize); // assume endpoint 2
        }
    }
}
void do_ethernet (uint8_t device_address)
{
    if (ethernet_info && ethernet_info->ethernet_configuration_id>0)
    {
        printf ("\nWe found an USB CDC ECM device, lets start using it.\n");
        if (ethernet_info->ethernet_control_interface==255 || ethernet_info->ethernet_data_interface==255)
            return; 
        // enable device configuration
        if (!set_configuration2(device_address,ethernet_info->ethernet_configuration_id))
            error ("error setting Ethernet configuration");
        // set alternate interface
        if (!set_interface2 (device_address,ethernet_info->ethernet_data_interface,ethernet_info->ethernet_alternate_setting))
            error ("error setting alternate interface");

        // check if the network cable is connected
        bool result = check_network_connection (device_address,ethernet_info->ethernet_interrupt_endpoint,ethernet_info->ethernet_interrupt_millis,ethernet_info->ethernet_interrupt_packetsize);
        if (!result)
            error ("please connect a network cable");
        else
        {
            printf ("network cable connected\n");
            const uint8_t PACKET_TYPE_MULTICAST =       0b00010000;
            const uint8_t PACKET_TYPE_BROADCAST =       0b00001000;
            const uint8_t PACKET_TYPE_DIRECTED =        0b00000100;
            const uint8_t PACKET_TYPE_ALL_MULTICAST =   0b00000010;
            const uint8_t PACKET_TYPE_PROMISCUOUS =     0b00000001;
            if (!set_packet_filter2 (device_address,ethernet_info->ethernet_control_interface,PACKET_TYPE_BROADCAST|PACKET_TYPE_DIRECTED))
                error ("error setting packet filter");
            dump_in_packets (device_address,ethernet_info->ethernet_bulk_in_endpoint,ethernet_info->ethernet_bulk_in_packetsize);
        }
    }
}

void do_hub (uint8_t device_address)
{
    if (hub_info && hub_info->hub_configuration_id>0)
    {
        if (hub_info->hub_interface==255)
            return;
        // enable device configuration
        if (!set_configuration2(device_address,hub_info->hub_configuration_id))
            error ("error setting Hub configuration");

        int length;
        USB_HUB_DESCRIPTOR* hub = new USB_HUB_DESCRIPTOR();
        bool result = get_hub_descriptor2(device_address,(uint8_t*) hub,length);
        if (!result)
            error("unable to get hub descriptor");
        printf ("  Hub Descriptor:\n");
        printf ("    bLength\t\t\t%d\n",hub->bLength);
        printf ("    bDescriptorType\t\t%x\n",hub->bDescriptorType);
        printf ("    bNrPorts\t\t\t%d\n",hub->bNrPorts);
        printf ("    wHubCharacteristics\t\t%x\n",hub->wHubCharacteristics);
        printf ("    bPwrOn2PwrGood\t\t%d\n",hub->bPwrOn2PwrGood);
        printf ("    bHubContrCurrent\t\t%d\n",hub->bHubContrCurrent);
        printf ("    DeviceRemovable\t\t%x\n",hub->DeviceRemovable);
        printf ("    PortPwrCtrlMask\t\t%x\n",hub->PortPwrCtrlMask);

        uint32_t status;
        const int PORT_POWER = 8;
        const int PORT_RESET = 4;
        const int PORT_LOW_SPEED = 9;
        const int C_PORT_RESET = 0x14;
        const int C_PORT_CONNECTION = 0x10;
        const int HUB_PORT_NUMBER = 4;
        const uint8_t HUB_PORT_BITMASK = (1 << (HUB_PORT_NUMBER));
        for (int i=1;i<=hub->bNrPorts;i++)
        {
            if (!set_hub_port_feature2 (device_address,PORT_POWER,i,0)) // power all ports
                error ("error powering Hub port");
        }   
        //if (!set_hub_port_feature2 (device_address,PORT_POWER,HUB_PORT_NUMBER,0)) // power all ports
        //        error ("error powering Hub port");
        delete hub;

        // wait something happens on any of the powered ports
        while (!(get_hub_change (device_address,hub_info->hub_interrupt_endpoint,hub_info->hub_interrupt_millis,hub_info->hub_interrupt_packetsize)&HUB_PORT_BITMASK));
        if (!clear_hub_port_feature2 (device_address,C_PORT_CONNECTION,HUB_PORT_NUMBER,0))// clear C_PORT_CONNECTION feature
            error ("error clearing feature Hub port");
        usleep (100000); // wait 100msec

        // check if low speed
        status = get_hub_portstatus (device_address,HUB_PORT_NUMBER)&0xffff;
        if (status&0x0200) // low speed
        {
            // no idea to handle this...
        }

        if (!set_hub_port_feature2 (device_address,PORT_RESET,HUB_PORT_NUMBER,0)) // reset port 1
            error ("error resetting Hub port 1");
        // wait for change of port 1 => bit 2
        while (!(get_hub_change (device_address,hub_info->hub_interrupt_endpoint,hub_info->hub_interrupt_millis,hub_info->hub_interrupt_packetsize)&HUB_PORT_BITMASK));
        status = get_hub_portstatus (device_address,HUB_PORT_NUMBER);
        if (!clear_hub_port_feature2 (device_address,C_PORT_RESET,HUB_PORT_NUMBER,0))// clear C_PORT_RESET feature
            error ("error clearing feature Hub port");
        
        // endpoint 0 should now be equal to the resetted device
        init_device (device_address+1);
    }
}
void connect_disk ()
{
    writeCommand (CH376_CMD_DISK_CONNECT);
    if (waitInterrupt ()!=CH375_USB_INT_SUCCESS)
        error ("disk not connected");
}
void mount_disk ()
{
    int status;
    int count = 5;
    while (count--)
    {
        writeCommand (CH376_CMD_DISK_MOUNT);
        sleep (1);
        status = waitInterrupt ();
        if ((status==CH375_USB_INT_CONNECT))
            connect_disk();
        if ((status==CH375_USB_INT_SUCCESS))
            break;
    }
    if (count==-1)
        error ("disk not mounted");
}
void abort_nak ()
{
    writeCommand (CH375_CMD_ABORT_NAK);
}
void open_file (char* name)
{
    int status;

    writeCommand (CH376_CMD_SET_FILE_NAME);
    writeDataMultiple ((uint8_t*) name,strlen(name));

    writeCommand (CH376_CMD_OPEN_FILE);
    if ((status=waitInterrupt ())!=CH375_USB_INT_SUCCESS)
        error ("file not opened");
    
    writeCommand (CH376_CMD_FILE_CLOSE);
    writeData (0);
    if ((status=waitInterrupt ())!=CH375_USB_INT_SUCCESS)
        error ("file not closed");
}
int main(int argc, const char * argv[]) 
{
    init_serial();
    reset_all();
    sleep (1);
    check_exists();

    // set reset bus and set host mode
    bool result;
    //result = set_usb_host_mode(CH375_USB_MODE_HOST_RESET);
    //sleep (1);
    if (!(result=set_usb_host_mode(CH375_USB_MODE_HOST)))
        error ("host mode not succeeded\n");
    usleep (250000);
    // first try some high-level stuff
    connect_disk ();
    usleep (500000);
    mount_disk ();
    open_file ("\\NEXTOR.DSK");

    result = set_usb_host_mode(CH375_USB_MODE_HOST_RESET);
    sleep (1);
    if (!(result=set_usb_host_mode(CH375_USB_MODE_HOST)))
        error ("host mode not succeeded\n");
    usleep (250000);

    // then do the low-level things
    init_device (DEV_ADDRESS);
        
    return 0;
}
