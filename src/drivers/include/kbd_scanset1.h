#ifndef KBD_SCANSET1
#define KBD_SCANSET1

#include <include/common.h>

// PKEY is pressed key, RKEY is released key
enum KBDKEY {
  _ESC_PKEY = 0x01,
  _1_PKEY = 0x02,
  _2_PKEY = 0x03,
  _3_PKEY = 0x04,
  _4_PKEY = 0x05,
  _5_PKEY = 0x06,
  _6_PKEY = 0x07,
  _7_PKEY = 0x08,
  _8_PKEY = 0x09,
  _9_PKEY = 0x0A,
  _0_PKEY = 0x0B,
  _HYPHN_PKEY = 0x0C,
  _EQ_PKEY = 0x0D,
  _BKSPC_PKEY = 0x0E,
  _TAB_PKEY = 0x0F,
  _Q_PKEY = 0x10,
  _W_PKEY = 0x11,
  _E_PKEY = 0x12,
  _R_PKEY = 0x13,
  _T_PKEY = 0x14,
  _Y_PKEY = 0x15,
  _U_PKEY = 0x16,
  _I_PKEY = 0x17,
  _O_PKEY = 0x18,
  _P_PKEY = 0x19,
  _SQBRKTO_PKEY = 0x1A,
  _SQBRKTC_PKEY = 0x1B,
  _ENTR_PKEY = 0x1C,
  _LCTRL_PKEY = 0x1D,
  _A_PKEY = 0x1E,
  _S_PKEY = 0x1F,
  _D_PKEY = 0x20,
  _F_PKEY = 0x21,
  _G_PKEY = 0x22,
  _H_PKEY = 0x23,
  _J_PKEY = 0x24,
  _K_PKEY = 0x25,
  _L_PKEY = 0x26,
  _SMICOLN_PKEY = 0x27,
  _SQOT_PKEY = 0x28,
  _BKTIK_PKEY = 0x29,
  _LSHFT_PKEY = 0x2A,
  _BKSLSH_PKEY = 0x2B,
  _Z_PKEY = 0x2C,
  _X_PKEY = 0x2D,
  _C_PKEY = 0x2E,
  _V_PKEY = 0x2F,
  _B_PKEY = 0x30,
  _N_PKEY = 0x31,
  _M_PKEY = 0x32,
  _COMA_PKEY = 0x33,
  _DOT_PKEY = 0x34,
  _FRWDSLSH_PKEY = 0x35,
  _RSHIFT_PKEY = 0x36,
  _STR_PKEY = 0x37,
  _LALT_PKEY = 0x38,
  _SPC_PKEY = 0x39,
  _CPS_PKEY = 0x3A,
  _F1_PKEY = 0x3B,
  _F2_PKEY = 0x3C,
  _F3_PKEY = 0x3D,
  _F4_PKEY = 0x3E,
  _F5_PKEY = 0x3F,
  _F6_PKEY = 0x40,
  _F7_PKEY = 0x41,
  _F8_PKEY = 0x42,
  _F9_PKEY = 0x43,
  _F10_PKEY = 0x44,
  _NUMLCK_PKEY = 0x45,
  _SCRLLCK_PKEY = 0x46,
  _F11_PKEY = 0x57,
  _F12_PKEY = 0x58,

  _ESC_RKEY = 0x81,
  _1_RKEY = 0x82,
  _2_RKEY = 0x83,
  _3_RKEY = 0x84,
  _4_RKEY = 0x85,
  _5_RKEY = 0x86,
  _6_RKEY = 0x87,
  _7_RKEY = 0x88,
  _8_RKEY = 0x89,
  _9_RKEY = 0x8A,
  _0_RKEY = 0x8B,
  _HYPHN_RKEY = 0x8C,
  _EQ_RKEY = 0x8D,
  _BKSPC_RKEY = 0x8E,
  _TAB_RKEY = 0x8F,
  _Q_RKEY = 0x90,
  _W_RKEY = 0x91,
  _E_RKEY = 0x92,
  _R_RKEY = 0x93,
  _T_RKEY = 0x94,
  _Y_RKEY = 0x95,
  _U_RKEY = 0x96,
  _I_RKEY = 0x97,
  _O_RKEY = 0x98,
  _P_RKEY = 0x99,
  _SQBRKTO_RKEY = 0x9A,
  _SQBRKTC_RKEY = 0x9B,
  _ENTR_RKEY = 0x9C,
  _LCTRL_RKEY = 0x9D,
  _A_RKEY = 0x9E,
  _S_RKEY = 0x9F,
  _D_RKEY = 0xA0,
  _F_RKEY = 0xA1,
  _G_RKEY = 0xA2,
  _H_RKEY = 0xA3,
  _J_RKEY = 0xA4,
  _K_RKEY = 0xA5,
  _L_RKEY = 0xA6,
  _SMICOLN_RKEY = 0xA7,
  _SQOT_RKEY = 0xA8,
  _BKTIK_RKEY = 0xA9,
  _LSHFT_RKEY = 0xAA,
  _BKSLSH_RKEY = 0xAB,
  _Z_RKEY = 0xAC,
  _X_RKEY = 0xAD,
  _C_RKEY = 0xAE,
  _V_RKEY = 0xAF,
  _B_RKEY = 0xB0,
  _N_RKEY = 0xB1,
  _M_RKEYz0xB2,
  _COMA_RKEY = 0xB3,
  _DOT_RKEY = 0xB4,
  _FWDSLSH_RKEY = 0xB5,
  _RSHFT_RKEY = 0xB6,
  _STR_RKEY = 0xB7,
  _LALT_RKEY = 0xB8,
  _SPC_RKEY = 0xB9,
  _CPS_RKEY = 0xBA,
  _F1_RKEY = 0xBB,
  _F2_RKEY = 0xBC,
  _F3_RKEY = 0xBD,
  _F4_RKEY = 0xBE,
  _F5_RKEY = 0xBF,
  _F6_RKEY = 0xC0,
  _F7_RKEY = 0xC1,
  _F8_RKEY = 0xC2,
  _F9_RKEY = 0xC3,
  _F10_RKEY = 0xC4,
  _NUMLCK_RKEY = 0xC5,
  _SCRLLCK_RKEY = 0xC6,
  _F11_RKEY = 0xD7,
  _F12_RKEY = 0xD8,
};

char SC1_ToASCII(uint8_t data) {
  switch (data) {
  case _1_PKEY:
    return '1';
  case _2_PKEY:
    return '2';
  case _3_PKEY:
    return '3';
  case _4_PKEY:
    return '4';
  case _5_PKEY:
    return '5';
  case _6_PKEY:
    return '6';
  case _7_PKEY:
    return '7';
  case _8_PKEY:
    return '8';
  case _9_PKEY:
    return '9';
  case _0_PKEY:
    return '0';
  case _HYPHN_PKEY:
    return '-';
  case _EQ_PKEY:
    return '=';
  case _BKSPC_PKEY:
    return '\r';
  case _TAB_PKEY:
    return '\t';
  case _Q_PKEY:
    return 'Q';
  case _W_PKEY:
  case _E_PKEY:
  case _R_PKEY:
  case _T_PKEY:
  case _Y_PKEY:
  case _U_PKEY:
  case _I_PKEY:
  case _O_PKEY:
  case _P_PKEY:
  case _SQBRKTO_PKEY:
  case _SQBRKTC_PKEY:
  case _ENTR_PKEY:
  case _LCTRL_PKEY:
  case _A_PKEY:
  case _S_PKEY:
  case _D_PKEY:
  case _F_PKEY:
  case _G_PKEY:
  case _H_PKEY:
  case _J_PKEY:
  case _K_PKEY:
  case _L_PKEY:
  case _SMICOLN_PKEY:
  case _SQOT_PKEY:
  case _BKTIK_PKEY:
  case _LSHFT_PKEY:
  case _BKSLSH_PKEY:
  case _Z_PKEY:
  case _X_PKEY:
  case _C_PKEY:
  case _V_PKEY:
  case _B_PKEY:
  case _N_PKEY:
  case _M_PKEY:
  case _COMA_PKEY:
  case _DOT_PKEY:
  case _FRWDSLSH_PKEY:
  case _RSHIFT_PKEY:
  case _STR_PKEY:
  case _LALT_PKEY:
  case _SPC_PKEY:
  case _CPS_PKEY:
  case _F1_PKEY:
  case _F2_PKEY:
  case _F3_PKEY:
  case _F4_PKEY:
  case _F5_PKEY:
  case _F6_PKEY:
  case _F7_PKEY:
  case _F8_PKEY:
  case _F9_PKEY:
  case _F10_PKEY:
  case _NUMLCK_PKEY:
  case _SCRLLCK_PKEY:
  case _F11_PKEY:
  case _F12_PKEY:
  case _ESC_RKEY:
  case _1_RKEY:
  case _2_RKEY:
  case _3_RKEY:
  case _4_RKEY:
  case _5_RKEY:
  case _6_RKEY:
  case _7_RKEY:
  case _8_RKEY:
  case _9_RKEY:
  case _0_RKEY:
  case _HYPHN_RKEY:
  case _EQ_RKEY:
  case _BKSPC_RKEY:
  case _TAB_RKEY:
  case _Q_RKEY:
  case _W_RKEY:
  case _E_RKEY:
  case _R_RKEY:
  case _T_RKEY:
  case _Y_RKEY:
  case _U_RKEY:
  case _I_RKEY:
  case _O_RKEY:
  case _P_RKEY:
  case _SQBRKTO_RKEY:
  case _SQBRKTC_RKEY:
  case _ENTR_RKEY:
  case _LCTRL_RKEY:
  case _A_RKEY:
  case _S_RKEY:
  case _D_RKEY:
  case _F_RKEY:
  case _G_RKEY:
  case _H_RKEY:
  case _J_RKEY:
  case _K_RKEY:
  case _L_RKEY:
  case _SMICOLN_RKEY:
  case _SQOT_RKEY:
  case _BKTIK_RKEY:
  case _LSHFT_RKEY:
  case _BKSLSH_RKEY:
  case _Z_RKEY:
  case _X_RKEY:
  case _C_RKEY:
  case _V_RKEY:
  case _B_RKEY:
  case _N_RKEY:
  case _M_RKEYz0xB2:
  case _COMA_RKEY:
  case _DOT_RKEY:
  case _FWDSLSH_RKEY:
  case _RSHFT_RKEY:
  case _STR_RKEY:
  case _LALT_RKEY:
  case _SPC_RKEY:
  case _CPS_RKEY:
  case _F1_RKEY:
  case _F2_RKEY:
  case _F3_RKEY:
  case _F4_RKEY:
  case _F5_RKEY:
  case _F6_RKEY:
  case _F7_RKEY:
  case _F8_RKEY:
  case _F9_RKEY:
  case _F10_RKEY:
  case _NUMLCK_RKEY:
  case _SCRLLCK_RKEY:
  case _F11_RKEY:
  case _F12_RKEY:
    break;
  }
}

#endif
