#ifndef KBD_SCANSET1
#define KBD_SCANSET1

#include <include/common.h>

// PKEY is pressed key, RKEY is released key
enum SC1_KBDKEY {
 SC1_ESC_PKEY = 0x01,
 SC1_1_PKEY = 0x02,
 SC1_2_PKEY = 0x03,
 SC1_3_PKEY = 0x04,
 SC1_4_PKEY = 0x05,
 SC1_5_PKEY = 0x06,
 SC1_6_PKEY = 0x07,
 SC1_7_PKEY = 0x08,
 SC1_8_PKEY = 0x09,
 SC1_9_PKEY = 0x0A,
 SC1_0_PKEY = 0x0B,
 SC1_HYPHN_PKEY = 0x0C,
 SC1_EQ_PKEY = 0x0D,
 SC1_BKSPC_PKEY = 0x0E,
 SC1_TAB_PKEY = 0x0F,
 SC1_Q_PKEY = 0x10,
 SC1_W_PKEY = 0x11,
 SC1_E_PKEY = 0x12,
 SC1_R_PKEY = 0x13,
 SC1_T_PKEY = 0x14,
 SC1_Y_PKEY = 0x15,
 SC1_U_PKEY = 0x16,
 SC1_I_PKEY = 0x17,
 SC1_O_PKEY = 0x18,
 SC1_P_PKEY = 0x19,
 SC1_SQBRKTO_PKEY = 0x1A,
 SC1_SQBRKTC_PKEY = 0x1B,
 SC1_ENTR_PKEY = 0x1C,
 SC1_LCTRL_PKEY = 0x1D,
 SC1_A_PKEY = 0x1E,
 SC1_S_PKEY = 0x1F,
 SC1_D_PKEY = 0x20,
 SC1_F_PKEY = 0x21,
 SC1_G_PKEY = 0x22,
 SC1_H_PKEY = 0x23,
 SC1_J_PKEY = 0x24,
 SC1_K_PKEY = 0x25,
 SC1_L_PKEY = 0x26,
 SC1_SMICOLN_PKEY = 0x27,
 SC1_SQOT_PKEY = 0x28,
 SC1_BKTIK_PKEY = 0x29,
 SC1_LSHFT_PKEY = 0x2A,
 SC1_BKSLSH_PKEY = 0x2B,
 SC1_Z_PKEY = 0x2C,
 SC1_X_PKEY = 0x2D,
 SC1_C_PKEY = 0x2E,
 SC1_V_PKEY = 0x2F,
 SC1_B_PKEY = 0x30,
 SC1_N_PKEY = 0x31,
 SC1_M_PKEY = 0x32,
 SC1_COMA_PKEY = 0x33,
 SC1_DOT_PKEY = 0x34,
 SC1_FRWDSLSH_PKEY = 0x35,
 SC1_RSHIFT_PKEY = 0x36,
 SC1_STR_PKEY = 0x37,
 SC1_LALT_PKEY = 0x38,
 SC1_SPC_PKEY = 0x39,
 SC1_CPS_PKEY = 0x3A,
 SC1_F1_PKEY = 0x3B,
 SC1_F2_PKEY = 0x3C,
 SC1_F3_PKEY = 0x3D,
 SC1_F4_PKEY = 0x3E,
 SC1_F5_PKEY = 0x3F,
 SC1_F6_PKEY = 0x40,
 SC1_F7_PKEY = 0x41,
 SC1_F8_PKEY = 0x42,
 SC1_F9_PKEY = 0x43,
 SC1_F10_PKEY = 0x44,
 SC1_NUMLCK_PKEY = 0x45,
 SC1_SCRLLCK_PKEY = 0x46,
 SC1_F11_PKEY = 0x57,
 SC1_F12_PKEY = 0x58,
 SC1_ESC_RKEY = 0x81,
 SC1_1_RKEY = 0x82,
 SC1_2_RKEY = 0x83,
 SC1_3_RKEY = 0x84,
 SC1_4_RKEY = 0x85,
 SC1_5_RKEY = 0x86,
 SC1_6_RKEY = 0x87,
 SC1_7_RKEY = 0x88,
 SC1_8_RKEY = 0x89,
 SC1_9_RKEY = 0x8A,
 SC1_0_RKEY = 0x8B,
 SC1_HYPHN_RKEY = 0x8C,
 SC1_EQ_RKEY = 0x8D,
 SC1_BKSPC_RKEY = 0x8E,
 SC1_TAB_RKEY = 0x8F,
 SC1_Q_RKEY = 0x90,
 SC1_W_RKEY = 0x91,
 SC1_E_RKEY = 0x92,
 SC1_R_RKEY = 0x93,
 SC1_T_RKEY = 0x94,
 SC1_Y_RKEY = 0x95,
 SC1_U_RKEY = 0x96,
 SC1_I_RKEY = 0x97,
 SC1_O_RKEY = 0x98,
 SC1_P_RKEY = 0x99,
 SC1_SQBRKTO_RKEY = 0x9A,
 SC1_SQBRKTC_RKEY = 0x9B,
 SC1_ENTR_RKEY = 0x9C,
 SC1_LCTRL_RKEY = 0x9D,
 SC1_A_RKEY = 0x9E,
 SC1_S_RKEY = 0x9F,
 SC1_D_RKEY = 0xA0,
 SC1_F_RKEY = 0xA1,
 SC1_G_RKEY = 0xA2,
 SC1_H_RKEY = 0xA3,
 SC1_J_RKEY = 0xA4,
 SC1_K_RKEY = 0xA5,
 SC1_L_RKEY = 0xA6,
 SC1_SMICOLN_RKEY = 0xA7,
 SC1_SQOT_RKEY = 0xA8,
 SC1_BKTIK_RKEY = 0xA9,
 SC1_LSHFT_RKEY = 0xAA,
 SC1_BKSLSH_RKEY = 0xAB,
 SC1_Z_RKEY = 0xAC,
 SC1_X_RKEY = 0xAD,
 SC1_C_RKEY = 0xAE,
 SC1_V_RKEY = 0xAF,
 SC1_B_RKEY = 0xB0,
 SC1_N_RKEY = 0xB1,
 SC1_M_RKEYz0xB2,
 SC1_COMA_RKEY = 0xB3,
 SC1_DOT_RKEY = 0xB4,
 SC1_FWDSLSH_RKEY = 0xB5,
 SC1_RSHFT_RKEY = 0xB6,
 SC1_STR_RKEY = 0xB7,
 SC1_LALT_RKEY = 0xB8,
 SC1_SPC_RKEY = 0xB9,
 SC1_CPS_RKEY = 0xBA,
 SC1_F1_RKEY = 0xBB,
 SC1_F2_RKEY = 0xBC,
 SC1_F3_RKEY = 0xBD,
 SC1_F4_RKEY = 0xBE,
 SC1_F5_RKEY = 0xBF,
 SC1_F6_RKEY = 0xC0,
 SC1_F7_RKEY = 0xC1,
 SC1_F8_RKEY = 0xC2,
 SC1_F9_RKEY = 0xC3,
 SC1_F10_RKEY = 0xC4,
 SC1_NUMLCK_RKEY = 0xC5,
 SC1_SCRLLCK_RKEY = 0xC6,
 SC1_F11_RKEY = 0xD7,
 SC1_F12_RKEY = 0xD8,
};

inline char SC1_ToASCII(uint8_t data) {
  switch (data) {
  case SC1_1_PKEY:
    return '1';
  case SC1_2_PKEY:
    return '2';
  case SC1_3_PKEY:
    return '3';
  case SC1_4_PKEY:
    return '4';
  case SC1_5_PKEY:
    return '5';
  case SC1_6_PKEY:
    return '6';
  case SC1_7_PKEY:
    return '7';
  case SC1_8_PKEY:
    return '8';
  case SC1_9_PKEY:
    return '9';
  case SC1_0_PKEY:
    return '0';
  case SC1_HYPHN_PKEY:
    return '-';
  case SC1_EQ_PKEY:
    return '=';
  case SC1_BKSPC_PKEY:
    return '\r';
  case SC1_TAB_PKEY:
    return '\t';
  case SC1_Q_PKEY:
    return 'Q';
  case SC1_W_PKEY:
    return 'W';
  case SC1_E_PKEY:
    return 'E';
  case SC1_R_PKEY:
    return 'R';
  case SC1_T_PKEY:
    return 'T';
  case SC1_Y_PKEY:
    return 'Y';
  case SC1_U_PKEY:
    return 'U';
  case SC1_I_PKEY:
    return 'I';
  case SC1_O_PKEY:
    return 'O';
  case SC1_P_PKEY:
    return 'P';
  case SC1_SQBRKTO_PKEY:
    return '0';
  case SC1_SQBRKTC_PKEY:
    return '0';
  case SC1_ENTR_PKEY:
    return 'E';
  case SC1_LCTRL_PKEY:
    return 'L';
  case SC1_A_PKEY:
    return 'A';
  case SC1_S_PKEY:
    return 'S';
  case SC1_D_PKEY:
    return 'D';
  case SC1_F_PKEY:
    return 'F';
  case SC1_G_PKEY:
    return 'G';
  case SC1_H_PKEY:
    return 'H';
  case SC1_J_PKEY:
    return 'J';
  case SC1_K_PKEY:
    return 'K';
  case SC1_L_PKEY:
    return 'L';
  case SC1_SMICOLN_PKEY:
  case SC1_SQOT_PKEY:
  case SC1_BKTIK_PKEY:
  case SC1_LSHFT_PKEY:
  case SC1_BKSLSH_PKEY:
  case SC1_Z_PKEY:
    return 'Z';
  case SC1_X_PKEY:
    return 'X';
  case SC1_C_PKEY:
    return 'C';
  case SC1_V_PKEY:
    return 'V';
  case SC1_B_PKEY:
    return 'B';
  case SC1_N_PKEY:
    return 'N';
  case SC1_M_PKEY:
    return 'M';
  case SC1_COMA_PKEY:
    return ',';
  case SC1_DOT_PKEY:
    return '.';
  case SC1_FRWDSLSH_PKEY:
  case SC1_RSHIFT_PKEY:
  case SC1_STR_PKEY:
  case SC1_LALT_PKEY:
  case SC1_SPC_PKEY:
  case SC1_CPS_PKEY:
  case SC1_F1_PKEY:
  case SC1_F2_PKEY:
  case SC1_F3_PKEY:
  case SC1_F4_PKEY:
  case SC1_F5_PKEY:
  case SC1_F6_PKEY:
  case SC1_F7_PKEY:
  case SC1_F8_PKEY:
  case SC1_F9_PKEY:
  case SC1_F10_PKEY:
  case SC1_NUMLCK_PKEY:
  case SC1_SCRLLCK_PKEY:
  case SC1_F11_PKEY:
  case SC1_F12_PKEY:
  case SC1_ESC_RKEY:
  case SC1_1_RKEY:
    return '1';
  case SC1_2_RKEY:
    return '2';
  case SC1_3_RKEY:
    return '3';
  case SC1_4_RKEY:
    return '4';
  case SC1_5_RKEY:
    return '5';
  case SC1_6_RKEY:
    return '6';
  case SC1_7_RKEY:
    return '7';
  case SC1_8_RKEY:
    return '8';
  case SC1_9_RKEY:
    return '9';
  case SC1_0_RKEY:
    return '0';
  case SC1_HYPHN_RKEY:
  case SC1_EQ_RKEY:
  case SC1_BKSPC_RKEY:
  case SC1_TAB_RKEY:
  case SC1_Q_RKEY:
  case SC1_W_RKEY:
  case SC1_E_RKEY:
  case SC1_R_RKEY:
  case SC1_T_RKEY:
  case SC1_Y_RKEY:
  case SC1_U_RKEY:
  case SC1_I_RKEY:
  case SC1_O_RKEY:
  case SC1_P_RKEY:
  case SC1_SQBRKTO_RKEY:
  case SC1_SQBRKTC_RKEY:
  case SC1_ENTR_RKEY:
  case SC1_LCTRL_RKEY:
  case SC1_A_RKEY:
  case SC1_S_RKEY:
  case SC1_D_RKEY:
  case SC1_F_RKEY:
  case SC1_G_RKEY:
  case SC1_H_RKEY:
  case SC1_J_RKEY:
  case SC1_K_RKEY:
  case SC1_L_RKEY:
  case SC1_SMICOLN_RKEY:
  case SC1_SQOT_RKEY:
  case SC1_BKTIK_RKEY:
  case SC1_LSHFT_RKEY:
  case SC1_BKSLSH_RKEY:
  case SC1_Z_RKEY:
  case SC1_X_RKEY:
  case SC1_C_RKEY:
  case SC1_V_RKEY:
  case SC1_B_RKEY:
  case SC1_N_RKEY:
  case SC1_M_RKEYz0xB2:
  case SC1_COMA_RKEY:
  case SC1_DOT_RKEY:
  case SC1_FWDSLSH_RKEY:
  case SC1_RSHFT_RKEY:
  case SC1_STR_RKEY:
  case SC1_LALT_RKEY:
  case SC1_SPC_RKEY:
  case SC1_CPS_RKEY:
  case SC1_F1_RKEY:
  case SC1_F2_RKEY:
  case SC1_F3_RKEY:
  case SC1_F4_RKEY:
  case SC1_F5_RKEY:
  case SC1_F6_RKEY:
  case SC1_F7_RKEY:
  case SC1_F8_RKEY:
  case SC1_F9_RKEY:
  case SC1_F10_RKEY:
  case SC1_NUMLCK_RKEY:
  case SC1_SCRLLCK_RKEY:
  case SC1_F11_RKEY:
  case SC1_F12_RKEY:
    return 'R';
  default:
    return 'N';
  }
}

#endif
