;
; usbhost.ASM - UNAPI compliant MSX USB driver
; Copyright (c) 2020 Mario Smit (S0urceror)
; 
; This program is free software: you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation, version 3.
;
; This program is distributed in the hope that it will be useful, but 
; WITHOUT ANY WARRANTY; without even the implied warranty of 
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
; General Public License for more details.
;
; You should have received a copy of the GNU General Public License 
; along with this program. If not, see <http://www.gnu.org/licenses/>.
;

; major and minor version number of MSXUSB UNAPI
UNAPI_P:  equ  0
UNAPI_S:  equ  1
; S0urceror's CH376s driver, major and minor version
IMPLEMENTATION_P:  equ  0
IMPLEMENTATION_S:  equ  1

USB_DEVICE_ADDRESS EQU 1

HIMEM equ 0FC4Ah
BOTTOM equ 0FC48h
MEMSIZ equ 0F672h
USRTAB equ 0F39Ah
RAMAD3 equ 0F344h

USBHOST_INIT:
    ; copy USB descriptors to WRKAREA
    ld bc, WRKAREA.USB_DESCRIPTORS
    call WRKAREAPTR
    ld hl, USB_DESCRIPTORS_START
    ld de, ix
    ld bc, USB_DESCRIPTORS_END - USB_DESCRIPTORS_START
    ldir
    ; INIT finished
    ld hl, TXT_UNAPI_INIT
    call PRINT
	ret

; return USB descriptors stored in RAM to enable modification
GET_SCRATCH:
    push ix, bc
    ld bc, WRKAREA.USB_DESCRIPTORS
    call WRKAREAPTR
    pop bc
    ld hl, ix
    add hl, bc
    pop ix
    ret 

; --------------------------------------
; CH_GET_DEVICE_DESCRIPTOR
;
; Input: HL=pointer to memory to receive device descriptor
; Output: Cy=0 no error, Cy=1 error
;         A  = USB error code
;         BC = Amount of data actually transferred (if IN transfer and no error)
CH_GET_DEVICE_DESCRIPTOR:
    push ix,hl,de,bc
    ld de, hl ; Address of the input or output data buffer

    ; return USB descriptor stored in scratch-area pointed to by SLTWRK+5 in HL for this ROM page
    ld bc, CMD_GET_DEVICE_DESCRIPTOR-CMD_GET_DEVICE_DESCRIPTOR ; Address of the command: 0x80,6,0,1,0,0,18,0
    call GET_SCRATCH

    ld a, 0 ; device address
    ld b, 8 ; length in bytes
    call HW_CONTROL_TRANSFER
    pop bc,de,hl,ix
    cp CH_USB_INT_SUCCESS
    ret z ; no error
    scf ; error
    ret

; --------------------------------------
; CH_GET_CONFIG_DESCRIPTOR
;
; Input: HL=pointer to memory to receive config descriptor
;        A=configuration index starting with 0 to DEVICE_DESCRIPTOR.bNumConfigurations
;        B=max_packetsize
;        C=config_descriptor_size
;        D=device address 
; Output: Cy=0 no error, Cy=1 error
CH_GET_CONFIG_DESCRIPTOR:
    push iy,ix,hl,de,bc
    ld iy, hl ; Address of the input or output data buffer

    ; get SLTWRK in HL for this ROM page
    push bc
    ld bc, CMD_GET_CONFIG_DESCRIPTOR-CMD_GET_DEVICE_DESCRIPTOR ; Address of the command: 0x80,6,configuration_id,2,0,0,config_descriptor_size,0
    call GET_SCRATCH
    pop bc
    
    ld ix, hl
    ld (ix+2), a
    ld (ix+6), c
    ld a, d ; device address
    ld de, iy ; Address of the input or output data buffer
    call HW_CONTROL_TRANSFER
    pop bc,de,hl,ix,iy
    cp CH_USB_INT_SUCCESS
    ret z ; no error
    scf ; error
    ret

; --------------------------------------
; CH_SET_CONFIGURATION
;
; Input: A=configuration id
;        B=packetsize
;        D=device address 
; Output: Cy=0 no error, Cy=1 error
CH_SET_CONFIGURATION:
    push ix,hl
    
    ; get SLTWRK in HL for this ROM page
    push bc
    ld bc, CMD_SET_CONFIGURATION-CMD_GET_DEVICE_DESCRIPTOR ; Address of the command: 0x00,0x09,configuration_id,0,0,0,0,0
    call GET_SCRATCH
    pop bc

    ld ix, hl
    ld (ix+2),a
    ld a, d ; device address
    call HW_CONTROL_TRANSFER
    pop hl,ix
    cp CH_USB_INT_SUCCESS
    ret z ; no error
    scf ; error
    ret

; --------------------------------------
; CH_SET_PROTOCOL
;
; Input: A=protocol id (0=BOOT)
;        B=packetsize
;        D=device address 
;        E=interface id
; Output: Cy=0 no error, Cy=1 error
CH_SET_PROTOCOL:
    push ix,hl
    
    ; get SLTWRK in HL for this ROM page
    push bc
    ld bc, CMD_SET_PROTOCOL-CMD_GET_DEVICE_DESCRIPTOR ; Address of the command: 0x21,0x0B,protocol_id,0,interface_id,0,0,0
    call GET_SCRATCH
    pop bc

    ld ix, hl
    ld (ix+2),a
    ld (ix+4),e
    ld a, d ; device address
    call HW_CONTROL_TRANSFER
    pop hl,ix
    cp CH_USB_INT_SUCCESS
    ret z ; no error
    scf ; error
    ret

; --------------------------------------
; CH_SET_IDLE
;
; Input: A=idle value
;        B=packetsize
;        C=report id
;        D=device address 
;        E=interface id
; Output: Cy=0 no error, Cy=1 error
CH_SET_IDLE:
    push ix,hl
    
    ; get SLTWRK in HL for this ROM page
    push bc
    ld bc, CMD_SET_IDLE-CMD_GET_DEVICE_DESCRIPTOR ; Address of the command: 0x21,0x0A,report_id,duration,interface_id,0,0,0
    call GET_SCRATCH
    pop bc

    ld ix, hl
    ld (ix+2),c
    ld (ix+3),a
    ld (ix+4),e
    ld a, d ; device address
    call HW_CONTROL_TRANSFER
    pop hl,ix
    cp CH_USB_INT_SUCCESS
    ret z ; no error
    scf ; error
    ret

; --------------------------------------
; CH_SET_ADDRESS
;
; Input: A=address to assign to connected USB device
;        B=packetsize
; Output: Cy=0 no error, Cy=1 error
CH_SET_ADDRESS:
    push ix,hl,de
    ld de, hl ; Address of the input or output data buffer

    ; get SLTWRK in HL for this ROM page
    push bc
    ld bc, CMD_SET_ADDRESS-CMD_GET_DEVICE_DESCRIPTOR ; Address of the command: 0x00,0x05,target_device_address,0,0,0,0,0
    call GET_SCRATCH
    pop bc
    
    ld ix, hl
    ld (ix+2),a
    ld a, 0 ; device address
    call HW_CONTROL_TRANSFER
    pop de,hl,ix
    cp CH_USB_INT_SUCCESS
    ret z ; no error
    scf ; error
    ret

;***********************************
;***  FUNCTIONS ADDRESSES TABLE  ***
;***********************************

;--- Standard routines addresses table
FN_TABLE:
FN_0:  dw  FN_INFO
FN_1:  dw  FN_JUMP_TABLE
FN_2:  dw  FN_CHECK
FN_3:  dw  FN_CONNECT
FN_4:  dw  FN_GETDESCRIPTORS
MAX_FN equ 5

JUMP_TABLE:
JN_0:
    rst 30h
    db 0 ; to be replaced with current slot id
    dw FN_CHECK
    ret
    nop 
    nop 
    nop
JN_1:
    rst 30h
    db 0 ; to be replaced with current slot id
    dw FN_CONNECT
    ret
    nop 
    nop 
    nop
JN_2:
    rst 30h
    db 0 ; to be replaced with current slot id
    dw FN_GETDESCRIPTORS
    ret
    nop 
    nop 
    nop
JN_3:
    rst 30h
    db 0 ; to be replaced with current slot id
    dw HW_CONTROL_TRANSFER
    ret
    nop 
    nop 
    nop
JN_4:
    rst 30h
    db 0 ; to be replaced with current slot id
    dw HW_DATA_IN_TRANSFER
    ret
    nop 
    nop 
    nop
JN_5:
    rst 30h
    db 0 ; to be replaced with current slot id
    dw HW_DATA_OUT_TRANSFER
    ret
    nop 
    nop 
    nop
JN_6:
    rst 30h
    db 0 ; to be replaced with current slot id
    dw GET_SCRATCH
    ret
    nop 
    nop 
    nop
JN_7:
    rst 30h
    db 0 ; to be replaced with current slot id
    dw HW_CONFIGURE_NAK_RETRY_2
    ret
    nop 
    nop 
    nop
END_JUMP_TABLE: DB 0

;************************
;***  FUNCTIONS CODE  ***
;************************

;--- Mandatory routine 0: return API information
;    Input:  A  = 0
;    Output: HL = Descriptive string for this implementation, on this slot, zero terminated
;            DE = API version supported, D.E
;            BC = This implementation version, B.C.
;            A  = 0 and Cy = 0

FN_INFO:
    ld  bc,256*IMPLEMENTATION_P+IMPLEMENTATION_S
    ld  de,256*UNAPI_P+UNAPI_S
    ld  hl,UNAPI_INFO
    xor  a
    ret

; Check if CH376s is connected and functional
; Input: (none)
; Output: Cy = 0, everything okay, Cy = 1, not connected
FN_CHECK:
    ; check_exists
    call CH_HW_TEST
    ret

; Connect attached USB device and reset it
; Input: (none)
; Output: Cy = 0, everything okay, Cy = 1, something went wrong
FN_CONNECT:
    ; set_usb_mode (MODE_HOST_RESET)
    ld a, CH_MODE_HOST_RESET
    call CH_SET_USB_MODE
    ret c
    ; set_usb_mode (MODE_HOST)
    ld a, CH_MODE_HOST
    call CH_SET_USB_MODE
    ret

; Get both the DEVICE and full CONFIG descriptors and return it
; Input: HL = pointer to buffer
; Output: Cy = 0, everything okay, Cy = 1, not connected
FN_GETDESCRIPTORS:
    ld ix, hl
    ; get device descriptor
    call CH_GET_DEVICE_DESCRIPTOR
    jr nc, _INIT_USBHID_NEXT
    ; if not OK, set_speed (2), get device descriptor
    ld a, CH_SPEED_LOW
    call CH_SET_SPEED
    ret c
    ld hl, ix ;DEVICE_DESCRIPTOR
    call CH_GET_DEVICE_DESCRIPTOR
    ret c
_INIT_USBHID_NEXT:
    ; store number of configurations
    ld a, (ix+DEVICE_DESCRIPTOR.bNumConfigurations)
    ld iyh, a
    ; set address (1)
    ld a, USB_DEVICE_ADDRESS ; address to assign to attached USB device
    ld b, (ix+DEVICE_DESCRIPTOR.bMaxPacketSize0)
    call CH_SET_ADDRESS
    ret c
    ; from now on the device only listens to address given
    ; get config descriptor
    ld iy, ix
    ld bc, DEVICE_DESCRIPTOR ; sizeof
    add iy, bc ; config lies after device descriptor
    ld e, 0 ; first configuration
_INIT_USBHID_AGAIN:
    ld a, e
    ld b, (ix+DEVICE_DESCRIPTOR.bMaxPacketSize0)
    ld c, CONFIG_DESCRIPTOR ; sizeof
    ld d, USB_DEVICE_ADDRESS ; assigned address
    ld hl, iy 
    call CH_GET_CONFIG_DESCRIPTOR ; call first with max packet size to discover real size
    ret c
    ld a, e
    ld c, (iy+CONFIG_DESCRIPTOR.wTotalLength) ; lower 8 bits
    call CH_GET_CONFIG_DESCRIPTOR ; call again with real size
    ld b, 0
    add iy, bc
    inc e
    ld a, (ix+DEVICE_DESCRIPTOR.bNumConfigurations)
    cp e
    jr nz, _INIT_USBHID_AGAIN
    ret

FN_JUMP_TABLE:
    push hl
    ; copy jump table
    ex hl,de
    ld hl, JUMP_TABLE
    ld bc, END_JUMP_TABLE - JUMP_TABLE
    ldir
    pop ix
    ; add current slot-id
    call GETSLT
    ld (ix+JN_0-JUMP_TABLE+1),a
    ld (ix+JN_1-JUMP_TABLE+1),a
    ld (ix+JN_2-JUMP_TABLE+1),a
    ld (ix+JN_3-JUMP_TABLE+1),a
    ld (ix+JN_4-JUMP_TABLE+1),a
    ld (ix+JN_5-JUMP_TABLE+1),a
    ld (ix+JN_6-JUMP_TABLE+1),a
    ld (ix+JN_7-JUMP_TABLE+1),a
    ret 

    include "usb_descriptors.asm"
	include "unapi.asm"

TXT_UNAPI_INIT DB "+UNAPI MSXUSB base driver started\r\n",0
UNAPI_ID DB "MSXUSB",0
;UNAPI_INFO: db "MSXUSB driver by Sourceror",0
;Moved to start of unused space in Nextor kernel
UNAPI_INFO: equ 7BD0h