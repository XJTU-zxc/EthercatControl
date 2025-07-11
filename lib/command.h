#ifndef LIB_COMMAND_H_
#define LIB_COMMAND_H_


struct Compiler;
struct StringWrapper;

int Command_VSR(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_TEC(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MIT(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MRT(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command__FLS(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command__RLS(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_BGM(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MVA(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MVR(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_AIT(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_EGM(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_EGR(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_AMP(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_TCP(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_DPE(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MON(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MOF(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_TCE(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_VLI(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_DPC(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MSP(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MAC(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_CLB(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_RPB(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_SET(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_WTM(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_TER(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_TCV(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_SFL(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_SRL(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_EPG(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_CLR(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_STM(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_VIE(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_VIC(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_VCW(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_VAW(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_VSP(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_VAC(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_VTN(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi); //正切轴使能及设定缩放系数WYF
int Command_TSV(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_TFV(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_EST(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_RST(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_JOG(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_FSW(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi); //寻找回零开关 zjq
int Command_FID(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi); //寻找z脉冲 zjq
int Command_RDI(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi); //读IO输入 zjq
int Command_RDO(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi); //读IO输出 zjq
int Command_BKC(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_DEC(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_DFN(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_DRN(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_DZN(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_DIT(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_BPM(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_BSN(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_BCP(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_IPA(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_MCM(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_GOTO(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_END(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);
int Command_SEN(struct Compiler* compiler, struct StringWrapper* wrapper, bool isMdi);


#endif /* LIB_COMMAND_H_ */

