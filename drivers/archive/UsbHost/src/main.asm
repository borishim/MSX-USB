;
; main.ASM - UNAPI compliant MSX USB driver
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

CHPUT   EQU     #A2

    org  4000h
    db  "AB"
    dw  INIT
    ds  12

INIT:
    ; HOOK EXTBIO
    call HOOK_EXTBIO
    ; create a scratch space by moving HIMEM down
    call ALLOC_SCRATCH
    ; INIT finished
    ld hl, TXT_INIT
    call PRINT

	ret	; Back to slots scanning

ALLOC_SCRATCH:
    push af, bc, de, hl

    ; get SLTWRK in HL for this ROM page
    call GETSLT
    call GETWRK
    ; skip 5 bytes old EXTBIO contents in SLTWRK
    inc hl
    inc hl
    inc hl
    inc hl
    inc hl
    ; calculate new HIMEM
    ld de,(HIMEM)
    ex hl, de
    ld bc, USB_DESCRIPTORS_END - USB_DESCRIPTORS_START
    or a
    sbc hl, bc
    ex hl, de
    ; store updated HIMEM in SLT WRK
    ld (hl),e
    inc hl
    ld (hl),d
    ex hl, de
    ; update HIMEM
    ld (HIMEM),hl
    ; update MEMSIZ
    push hl,de
    ld hl,(MEMSIZ)
    or a
    sbc hl,bc
    ld (MEMSIZ),hl
    pop de,hl

    ; copy USB commands to scratch area
    ex de,hl
    ld hl, USB_DESCRIPTORS_START
    ld bc, USB_DESCRIPTORS_END - USB_DESCRIPTORS_START
    ldir

    pop hl,de,bc,af
    ret 

; return USB descriptor stored in scratch-area pointed to by SLTWRK+5 in HL for this ROM page
GET_SCRATCH:
    push de, af
    push bc
    call GETSLT
    call GETWRK
    ld de, 5
    add hl, de
    ld e, (hl)
    inc hl
    ld d, (hl)
    ex de, hl
    pop bc
    add hl, bc
    pop af, de
    ret 

;       Subroutine      Print nul-terminated text with the BIOS routine
;       Inputs          HL - pointer to text to print
;       Outputs         -------------------------------
PRINT:
    push af,hl
_PRINT_MORE:
    ld a,(hl)
    and a
    jr z, _PRINT_DONE
    call CHPUT
    inc hl
    jr _PRINT_MORE
_PRINT_DONE:
    pop hl,af
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
MAX_FN equ 4

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
	include "ch376s.asm"
    include "unapi.asm"

TXT_INIT DB "\r\nUNAPI MSX USB Driver started\r\n\r\n",0
UNAPI_ID DB "MSXUSB",0
UNAPI_INFO: db "MSXUSB driver by Sourceror",0

    DS 0C000h-$,0