#ifndef PSX_H
#define PSX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define API __declspec(dllexport)
#define ABI __stdcall


#define KB 1024
#define MB 1024*KB
#define GB 1024*MB


typedef uint64_t u64;
typedef int64_t  s64;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint8_t  u8;
typedef int8_t   s8;



/* ************************************************************************* */
/* MIPS R3400 CPU Registers                                                  */
/* ************************************************************************* */
#define R0     0x00
#define R1     0x01
#define R2     0x02
#define R3     0x03
#define R4     0x04
#define R5     0x05
#define R6     0x06
#define R7     0x07
#define R8     0x08
#define R9     0x09
#define R10    0x0A
#define R11    0x0B
#define R12    0x0C
#define R13    0x0D
#define R14    0x0E
#define R15    0x0F
#define R16    0x10
#define R17    0x11
#define R18    0x12
#define R19    0x13
#define R20    0x14
#define R21    0x15
#define R22    0x16
#define R23    0x17
#define R24    0x18
#define R25    0x19
#define R26    0x1A
#define R27    0x1B
#define R28    0x1C
#define R29    0x1D
#define R30    0x1E
#define R31    0x1F
#define LO     0x20
#define HI     0x21
#define PC     0x22
#define DELAY  0x23


#define BADA   0x08
#define SR     0x0C
#define CAUSE  0x0D



/* ************************************************************************* */
/* CPU.C Exceptions                                                          */
/* ************************************************************************* */
#define ILLEGAL_OPCODE 0x0001
#define OVERFLOW       0x0002
#define BAD_ADDR       0x0003
#define INTERRUPT      0x0004



/* ************************************************************************* */
/* BUS.C                                                                     */
/* ************************************************************************* */
#define BUS_RAM      (0x00000000)           //  00000000h 80000000h A0000000h  2048K  Main RAM (first 64K reserved for BIOS)
#define BUS_EX1      (BUS_RAM     + 2*MB)   //  1F000000h 9F000000h BF000000h  8192K  Expansion Region 1 (ROM/RAM)
#define BUS_EX3      (BUS_EX1     + 8*MB)   //  1FA00000h 9FA00000h BFA00000h  2048K  Expansion Region 3 (whatever purpose)
#define BUS_BIOS     (BUS_EX3     + 2*MB)   //  1FC00000h 9FC00000h BFC00000h  512K   BIOS ROM (Kernel) (4096K max)
#define BUS_IO       (BUS_BIOS    + 512*KB) //  1F801000h 9F801000h BF801000h  8K     I/O Ports
#define BUS_EX2      (BUS_IO      + 4*KB)   //  1F802000h 9F802000h BF802000h  8K     Expansion Region 2 (I/O Ports)
#define BUS_SCRATCH  (BUS_EX2     + 8*KB)   //  1F800000h 9F800000h    --      1K     Scratchpad (D-Cache used as Fast RAM)
#define BUS_KSEG2    (BUS_SCRATCH + 1*KB)   //  FFFE0000h (KSEG2)              0.5K   I/O Ports (Cache Control)

API int ABI BUS_Reset(void);
API u8 *ABI BUS_TranslateVirt2Phys(u32 ptr);
API int ABI BUS_Get32(u32 ptr);
API int ABI BUS_Get16(u32 ptr);
API int ABI BUS_Get8(u32 ptr);
API int ABI BUS_Set32(u32 ptr, u32 val);
API int ABI BUS_Set16(u32 ptr, u16 val);
API int ABI BUS_Set8(u32 ptr, u8 val);
API int ABI BUS_GetCpu(u32 ptr, u8 *buf, u32 len);
API int ABI BUS_SetCpu(u32 ptr, u8 *buf, u32 len);
API int ABI BUS_GetGpu(u32 ptr, u8 *buf, u32 len);
API int ABI BUS_SetGpu(u32 ptr, u8 *buf, u32 len);
API int ABI BUS_GetSpu(u32 ptr, u8 *buf, u32 len);
API int ABI BUS_SetSpu(u32 ptr, u8 *buf, u32 len);
API u32 ABI BUS_GetReg(u32 reg);
API int ABI BUS_SetReg(u32 reg, u32 val);
API int ABI BUS_LoadState(u8 *buf, u32 len);
API int ABI BUS_DumpState(u8 *buf, u32 len);



/* ************************************************************************* */
/* BIOS.C                                                                    */
/* ************************************************************************* */
API int ABI BIOS_InsertRom(char *path);



/* ************************************************************************* */
/* CARD.C                                                                    */
/* ************************************************************************* */
API int ABI CARD_InsertCard(int slot, char *path);



/* ************************************************************************* */
/* CPU.C                                                                     */
/* ************************************************************************* */
API int ABI CPU_Reset(void);
API int ABI CPU_Step(void);
API int ABI CPU_Goto(u32 ptr);



/* ************************************************************************* */
/* DBG.C                                                                     */
/* ************************************************************************* */
API int ABI DBG_Halt(void);
API int ABI DBG_Run(void);
API int ABI DBG_StepFrame(void);
API int ABI DBG_StepInto(void);
API int ABI DBG_StepOver(void);
API int ABI DBG_StepOut(void);
API int ABI DBG_InsertBreakPoint(u32 ptr, u32 len, u32 rwx);
API int ABI DBG_RemoveBreakPoint(u32 ptr, u32 len, u32 rwx);



/* ************************************************************************* */
/* CDROM.C                                                                   */
/* ************************************************************************* */
API int ABI CDROM_InsertDisc(char *path);
API int ABI CDROM_ReadSector(u32 lba, u8 ptr);
API int ABI CDROM_SeekSector(u32 lba);



/* ************************************************************************* */
/* LOG.C                                                                     */
/* ************************************************************************* */
API int ABI LOG_Save(char *path);
API int ABI LOG_Enable(int enable);
API int ABI LOG_Filter(char *module, int show);
API int ABI LOG_Info(char *module, char *message);
API int ABI LOG_Pass(char *module, char *message);
API int ABI LOG_Warn(char *module, char *message);
API int ABI LOG_Fail(char *module, char *message);



/* ************************************************************************* */
/* OS.C - Platform dependent code                                            */
/* ************************************************************************* */
API int ABI OS_OpenFile(char *path);
API int ABI OS_CloseFile(int fd);
API int ABI OS_SeekFile(int fd, int pos);
API int ABI OS_ReadFile(int fd, char *buf, int len);
API int ABI OS_WriteFile(int fd, char *buf, int len);



#ifdef __cplusplus
}
#endif
#endif // PSX_H
