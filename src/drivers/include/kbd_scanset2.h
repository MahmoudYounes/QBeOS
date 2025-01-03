#ifndef KBD_SCANSET2
#define KBD_SCANSET2

#include <include/common.h>

const uint8_t SC2_BREAK = 0xf0;
const uint8_t SC2_ERR = '\e';

enum SC2_KBDKEY {
 _SC2_F9_PKEY = 0x01,
 _SC2_F5_PKEY = 0x03,
 _SC2_F3_PKEY = 0x04,
 _SC2_F1_PKEY = 0x05,
 _SC2_F2_PKEY = 0x06,
 _SC2_F12_PKEY = 0x07,
 _SC2_F10_PKEY = 0x09,
 _SC2_F8_PKEY = 0x0A,
 _SC2_F6_PKEY = 0x0B,
 _SC2_F4_PKEY = 0x0C,
 _SC2_TAB_PKEY = 0x0D,
 _SC2_BTIK_PKEY = 0x0E,
 _SC2_LALT_PKEY = 0x11,
 _SC2_LSHFT_PKEY = 0x12,
 _SC2_LCTRL_PKEY = 0x14,
 _SC2_Q_PKEY = 0x15,
 _SC2_1_PKEY = 0x16,
 _SC2_Z_PKEY = 0x1A,
 _SC2_S_PKEY = 0x1B,
 _SC2_A_PKEY = 0x1C,
 _SC2_W_PKEY = 0x1D,
 _SC2_2_PKEY = 0x1E,
 _SC2_C_PKEY = 0x21,
 _SC2_X_PKEY = 0x22,
 _SC2_D_PKEY = 0x23,
 _SC2_E_PKEY = 0x24,
 _SC2_4_PKEY = 0x25,
 _SC2_3_PKEY = 0x26,
 _SC2_SPC_PKEY = 0x29,
 _SC2_V_PKEY = 0x2A,
 _SC2_F_PKEY = 0x2B,
 _SC2_T_PKEY = 0x2C,
 _SC2_R_PKEY = 0x2D,
 _SC2_5_PKEY = 0x2E,
 _SC2_N_PKEY = 0x31,
 _SC2_B_PKEY = 0x32,
 _SC2_H_PKEY = 0x33,
 _SC2_G_PKEY = 0x34,
 _SC2_Y_PKEY = 0x35,
 _SC2_6_PKEY = 0x36,
 _SC2_M_PKEY = 0x3A,
 _SC2_J_PKEY = 0x3B,
 _SC2_U_PKEY = 0x3C,
 _SC2_7_PKEY = 0x3D,
 _SC2_8_PKEY = 0x3E,
 _SC2_COMA_PKEY = 0x41,
 _SC2_K_PKEY = 0x42,
 _SC2_I_PKEY = 0x43,
 _SC2_O_PKEY = 0x44,
 _SC2_0_PKEY = 0x45,
 _SC2_9_PKEY = 0x46,
 _SC2_DOT_PKEY = 0x49,
 _SC2_FWDSLSH_PKEY = 0x4A,
 _SC2_L_PKEY = 0x4B,
 _SC2_SMICOLN_PKEY = 0x4C,
 _SC2_P_PKEY = 0x4D,
 _SC2_HYPHN_PKEY = 0x4E,
 _SC2_SQOT_PKEY = 0x52,
 _SC2_SQBRKTO_PKEY = 0x54,
 _SC2_EQ_PKEY = 0x55,
 _SC2_CPSLCK_PKEY = 0x58,
 _SC2_RSHFT_PKEY = 0x59,
 _SC2_ENTR_PKEY = 0x5A,
 _SC2_SQBRKTC_PKEY = 0x5B,
 _SC2_BKSLSH_PKEY = 0x5D,
 _SC2_BKSPC_PKEY = 0x66,
 _SC2_ESC_PKEY = 0x76,
 _SC2_NUMLCK_PKEY = 0x77,
 _SC2_F11_PKEY = 0x78,
 _SC2_SCRLLCK_PKEY = 0x7E,
 _SC2_F7_PKEY = 0x83,
};

inline bool SC2_isKeyboardCommand(uint8_t data){
  switch (data){
    case _SC2_NUMLCK_PKEY:
    case _SC2_SCRLLCK_PKEY:
    case _SC2_CPSLCK_PKEY:
      return true;
    default:
      return false;
  }
}

inline char SC2_toASCII(uint8_t data){
  switch (data) {
    case _SC2_0_PKEY:
      return '0';
    case _SC2_1_PKEY:
      return '1';
    case _SC2_2_PKEY:
      return '2';
    case _SC2_3_PKEY:
      return '3';
    case _SC2_4_PKEY:
      return '4';
    case _SC2_5_PKEY:
      return '5';
    case _SC2_6_PKEY:
      return '6';
    case _SC2_7_PKEY:
      return '7';
    case _SC2_8_PKEY:
      return '8';
    case _SC2_9_PKEY:
      return '9';
    case _SC2_A_PKEY:
      return 'a';
    case _SC2_B_PKEY:
      return 'b';
    case _SC2_C_PKEY:
      return 'c';
    case _SC2_D_PKEY:
      return 'd';
    case _SC2_E_PKEY:
      return 'e';
    case _SC2_F_PKEY:
      return 'f';
    case _SC2_G_PKEY:
      return 'g';
    case _SC2_H_PKEY:
      return 'h';
    case _SC2_I_PKEY:
      return 'i';
    case _SC2_J_PKEY:
      return 'j';
    case _SC2_K_PKEY:
      return 'k';
    case _SC2_L_PKEY:
      return 'l';
    case _SC2_M_PKEY:
      return 'm';
    case _SC2_N_PKEY:
      return 'n';
    case _SC2_O_PKEY:
      return 'o';
    case _SC2_P_PKEY:
      return 'p';
    case _SC2_Q_PKEY:
      return 'q';
    case _SC2_R_PKEY:
      return 'r';
    case _SC2_S_PKEY:
      return 's';
    case _SC2_T_PKEY:
      return 't';
    case _SC2_U_PKEY:
      return 'u';
    case _SC2_V_PKEY:
      return 'v';
    case _SC2_W_PKEY:
      return 'w';
    case _SC2_X_PKEY:
      return 'x';
    case _SC2_Y_PKEY:
      return 'y';
    case _SC2_Z_PKEY:
      return 'z';
    case _SC2_SPC_PKEY:
      return ' ';
    default:
      return SC2_ERR;
  }
}

#endif
