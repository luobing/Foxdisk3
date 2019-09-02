#ifndef __PARTWORK_H_
#define __PARTWORK_H_


#define PRIMARY_PARTN       1
#define EXTENDED_PARTN      2
#define LOGIC_PARTN         3

extern void Create_Partn (PARTN *partnArr, int ndx, unsigned long begin, unsigned long end,
                   int kind, int find_sw);
extern void Del_Partn (PARTN *partnArr, int ndx); 
extern void Set_Active_Partn (PARTN *partnArr, PARTN *ptr, int bToggle);  
extern void ReloadLogicPartn (PARTN *partnArr);
extern int ChkValidLogicPartn (PARTN *ptr);
extern int Partn_Save_Process (void *pArg,unsigned long mbr);                
#endif  /* __PARTWORK_H_ */