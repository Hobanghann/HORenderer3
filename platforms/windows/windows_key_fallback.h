#pragma once
#include <windows.h>

// ==========================================================
// Safe Windows VK_* Macro Definitions (Fallback Completer)
// ==========================================================

// Mouse Buttons
#ifndef VK_LBUTTON
#define VK_LBUTTON 0x01
#endif
#ifndef VK_RBUTTON
#define VK_RBUTTON 0x02
#endif
#ifndef VK_CANCEL
#define VK_CANCEL 0x03
#endif
#ifndef VK_MBUTTON
#define VK_MBUTTON 0x04
#endif
#ifndef VK_XBUTTON1
#define VK_XBUTTON1 0x05
#endif
#ifndef VK_XBUTTON2
#define VK_XBUTTON2 0x06
#endif

// Control Keys
#ifndef VK_BACK
#define VK_BACK 0x08
#endif
#ifndef VK_TAB
#define VK_TAB 0x09
#endif
#ifndef VK_CLEAR
#define VK_CLEAR 0x0C
#endif
#ifndef VK_RETURN
#define VK_RETURN 0x0D
#endif
#ifndef VK_SHIFT
#define VK_SHIFT 0x10
#endif
#ifndef VK_CONTROL
#define VK_CONTROL 0x11
#endif
#ifndef VK_MENU
#define VK_MENU 0x12
#endif
#ifndef VK_PAUSE
#define VK_PAUSE 0x13
#endif
#ifndef VK_CAPITAL
#define VK_CAPITAL 0x14
#endif
#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif
#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif
#ifndef VK_PRIOR
#define VK_PRIOR 0x21
#endif
#ifndef VK_NEXT
#define VK_NEXT 0x22
#endif
#ifndef VK_END
#define VK_END 0x23
#endif
#ifndef VK_HOME
#define VK_HOME 0x24
#endif
#ifndef VK_LEFT
#define VK_LEFT 0x25
#endif
#ifndef VK_UP
#define VK_UP 0x26
#endif
#ifndef VK_RIGHT
#define VK_RIGHT 0x27
#endif
#ifndef VK_DOWN
#define VK_DOWN 0x28
#endif
#ifndef VK_SELECT
#define VK_SELECT 0x29
#endif
#ifndef VK_PRINT
#define VK_PRINT 0x2A
#endif
#ifndef VK_EXECUTE
#define VK_EXECUTE 0x2B
#endif
#ifndef VK_SNAPSHOT
#define VK_SNAPSHOT 0x2C
#endif
#ifndef VK_INSERT
#define VK_INSERT 0x2D
#endif
#ifndef VK_DELETE
#define VK_DELETE 0x2E
#endif
#ifndef VK_HELP
#define VK_HELP 0x2F
#endif

// Numbers (Top Row)
#ifndef VK_NUM_0
#define VK_NUM_0 0x30
#endif
#ifndef VK_NUM_1
#define VK_NUM_1 0x31
#endif
#ifndef VK_NUM_2
#define VK_NUM_2 0x32
#endif
#ifndef VK_NUM_3
#define VK_NUM_3 0x33
#endif
#ifndef VK_NUM_4
#define VK_NUM_4 0x34
#endif
#ifndef VK_NUM_5
#define VK_NUM_5 0x35
#endif
#ifndef VK_NUM_6
#define VK_NUM_6 0x36
#endif
#ifndef VK_NUM_7
#define VK_NUM_7 0x37
#endif
#ifndef VK_NUM_8
#define VK_NUM_8 0x38
#endif
#ifndef VK_NUM_9
#define VK_NUM_9 0x39
#endif

// Alphabet Keys
#ifndef VK_A
#define VK_A 0x41
#endif
#ifndef VK_B
#define VK_B 0x42
#endif
#ifndef VK_C
#define VK_C 0x43
#endif
#ifndef VK_D
#define VK_D 0x44
#endif
#ifndef VK_E
#define VK_E 0x45
#endif
#ifndef VK_F
#define VK_F 0x46
#endif
#ifndef VK_G
#define VK_G 0x47
#endif
#ifndef VK_H
#define VK_H 0x48
#endif
#ifndef VK_I
#define VK_I 0x49
#endif
#ifndef VK_J
#define VK_J 0x4A
#endif
#ifndef VK_K
#define VK_K 0x4B
#endif
#ifndef VK_L
#define VK_L 0x4C
#endif
#ifndef VK_M
#define VK_M 0x4D
#endif
#ifndef VK_N
#define VK_N 0x4E
#endif
#ifndef VK_O
#define VK_O 0x4F
#endif
#ifndef VK_P
#define VK_P 0x50
#endif
#ifndef VK_Q
#define VK_Q 0x51
#endif
#ifndef VK_R
#define VK_R 0x52
#endif
#ifndef VK_S
#define VK_S 0x53
#endif
#ifndef VK_T
#define VK_T 0x54
#endif
#ifndef VK_U
#define VK_U 0x55
#endif
#ifndef VK_V
#define VK_V 0x56
#endif
#ifndef VK_W
#define VK_W 0x57
#endif
#ifndef VK_X
#define VK_X 0x58
#endif
#ifndef VK_Y
#define VK_Y 0x59
#endif
#ifndef VK_Z
#define VK_Z 0x5A
#endif

// System Keys
#ifndef VK_LWIN
#define VK_LWIN 0x5B
#endif
#ifndef VK_RWIN
#define VK_RWIN 0x5C
#endif
#ifndef VK_APPS
#define VK_APPS 0x5D
#endif
#ifndef VK_SLEEP
#define VK_SLEEP 0x5F
#endif

// Numpad
#ifndef VK_NUMPAD0
#define VK_NUMPAD0 0x60
#endif
#ifndef VK_NUMPAD1
#define VK_NUMPAD1 0x61
#endif
#ifndef VK_NUMPAD2
#define VK_NUMPAD2 0x62
#endif
#ifndef VK_NUMPAD3
#define VK_NUMPAD3 0x63
#endif
#ifndef VK_NUMPAD4
#define VK_NUMPAD4 0x64
#endif
#ifndef VK_NUMPAD5
#define VK_NUMPAD5 0x65
#endif
#ifndef VK_NUMPAD6
#define VK_NUMPAD6 0x66
#endif
#ifndef VK_NUMPAD7
#define VK_NUMPAD7 0x67
#endif
#ifndef VK_NUMPAD8
#define VK_NUMPAD8 0x68
#endif
#ifndef VK_NUMPAD9
#define VK_NUMPAD9 0x69
#endif
#ifndef VK_MULTIPLY
#define VK_MULTIPLY 0x6A
#endif
#ifndef VK_ADD
#define VK_ADD 0x6B
#endif
#ifndef VK_SEPARATOR
#define VK_SEPARATOR 0x6C
#endif
#ifndef VK_SUBTRACT
#define VK_SUBTRACT 0x6D
#endif
#ifndef VK_DECIMAL
#define VK_DECIMAL 0x6E
#endif
#ifndef VK_DIVIDE
#define VK_DIVIDE 0x6F
#endif

// Function Keys
#ifndef VK_F1
#define VK_F1 0x70
#endif
#ifndef VK_F2
#define VK_F2 0x71
#endif
#ifndef VK_F3
#define VK_F3 0x72
#endif
#ifndef VK_F4
#define VK_F4 0x73
#endif
#ifndef VK_F5
#define VK_F5 0x74
#endif
#ifndef VK_F6
#define VK_F6 0x75
#endif
#ifndef VK_F7
#define VK_F7 0x76
#endif
#ifndef VK_F8
#define VK_F8 0x77
#endif
#ifndef VK_F9
#define VK_F9 0x78
#endif
#ifndef VK_F10
#define VK_F10 0x79
#endif
#ifndef VK_F11
#define VK_F11 0x7A
#endif
#ifndef VK_F12
#define VK_F12 0x7B
#endif
#ifndef VK_F13
#define VK_F13 0x7C
#endif
#ifndef VK_F14
#define VK_F14 0x7D
#endif
#ifndef VK_F15
#define VK_F15 0x7E
#endif
#ifndef VK_F16
#define VK_F16 0x7F
#endif
#ifndef VK_F17
#define VK_F17 0x80
#endif
#ifndef VK_F18
#define VK_F18 0x81
#endif
#ifndef VK_F19
#define VK_F19 0x82
#endif
#ifndef VK_F20
#define VK_F20 0x83
#endif
#ifndef VK_F21
#define VK_F21 0x84
#endif
#ifndef VK_F22
#define VK_F22 0x85
#endif
#ifndef VK_F23
#define VK_F23 0x86
#endif
#ifndef VK_F24
#define VK_F24 0x87
#endif

// Lock & Modifier Keys
#ifndef VK_NUMLOCK
#define VK_NUMLOCK 0x90
#endif
#ifndef VK_SCROLL
#define VK_SCROLL 0x91
#endif
#ifndef VK_LSHIFT
#define VK_LSHIFT 0xA0
#endif
#ifndef VK_RSHIFT
#define VK_RSHIFT 0xA1
#endif
#ifndef VK_LCONTROL
#define VK_LCONTROL 0xA2
#endif
#ifndef VK_RCONTROL
#define VK_RCONTROL 0xA3
#endif
#ifndef VK_LMENU
#define VK_LMENU 0xA4
#endif
#ifndef VK_RMENU
#define VK_RMENU 0xA5
#endif

// OEM / Symbol Keys
#ifndef VK_OEM_1
#define VK_OEM_1 0xBA  // ';:'
#endif
#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS 0xBB  // '+'
#endif
#ifndef VK_OEM_COMMA
#define VK_OEM_COMMA 0xBC  // ','
#endif
#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS 0xBD  // '-'
#endif
#ifndef VK_OEM_PERIOD
#define VK_OEM_PERIOD 0xBE  // '.'
#endif
#ifndef VK_OEM_2
#define VK_OEM_2 0xBF  // '/?'
#endif
#ifndef VK_OEM_3
#define VK_OEM_3 0xC0  // '`~'
#endif
#ifndef VK_OEM_4
#define VK_OEM_4 0xDB  // '[{'
#endif
#ifndef VK_OEM_5
#define VK_OEM_5 0xDC  // '\|'
#endif
#ifndef VK_OEM_6
#define VK_OEM_6 0xDD  // ']}'
#endif
#ifndef VK_OEM_7
#define VK_OEM_7 0xDE  // ''"'
#endif
#ifndef VK_OEM_8
#define VK_OEM_8 0xDF
#endif

// Multimedia Keys (Optional)
#ifndef VK_VOLUME_MUTE
#define VK_VOLUME_MUTE 0xAD
#endif
#ifndef VK_VOLUME_DOWN
#define VK_VOLUME_DOWN 0xAE
#endif
#ifndef VK_VOLUME_UP
#define VK_VOLUME_UP 0xAF
#endif
#ifndef VK_MEDIA_NEXT_TRACK
#define VK_MEDIA_NEXT_TRACK 0xB0
#endif
#ifndef VK_MEDIA_PREV_TRACK
#define VK_MEDIA_PREV_TRACK 0xB1
#endif
#ifndef VK_MEDIA_STOP
#define VK_MEDIA_STOP 0xB2
#endif
#ifndef VK_MEDIA_PLAY_PAUSE
#define VK_MEDIA_PLAY_PAUSE 0xB3
#endif
#ifndef VK_LAUNCH_MAIL
#define VK_LAUNCH_MAIL 0xB4
#endif
#ifndef VK_LAUNCH_MEDIA_SELECT
#define VK_LAUNCH_MEDIA_SELECT 0xB5
#endif
#ifndef VK_LAUNCH_APP1
#define VK_LAUNCH_APP1 0xB6
#endif
#ifndef VK_LAUNCH_APP2
#define VK_LAUNCH_APP2 0xB7
#endif
