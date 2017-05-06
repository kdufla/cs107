#ifndef _BANK_H
#define _BANK_H

#include <semaphore.h>

typedef struct Bank {
  unsigned int numberBranches;
  struct       Branch  *branches;
  struct       Report  *report;
  int numberWorkersHasToFinish;
  sem_t forCheck;
  sem_t letMeStartNextDay;
  sem_t lockForReportTransfer;
} Bank;

#include "account.h"

int Bank_Balance(Bank *bank, AccountAmount *balance);

Bank *Bank_Init(int numBranches, int numAccounts, AccountAmount initAmount,
                AccountAmount reportingAmount,
                int numWorkers);

int Bank_Validate(Bank *bank);
int Bank_Compare(Bank *bank1, Bank *bank2);
void free_all(Bank *bank);


#endif /* _BANK_H */
