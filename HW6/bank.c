#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>


#include "error.h"

#include "bank.h"
#include "branch.h"
#include "account.h"
#include "report.h"

/*
 * allocate the bank structure and initialize the branches.
 */
Bank*
Bank_Init(int numBranches, int numAccounts, AccountAmount initalAmount,
          AccountAmount reportingAmount,
          int numWorkers)
{
	Bank *bank = malloc(sizeof(Bank));
  bank-> numberWorkersHasToFinish = numWorkers;
  sem_init(&(bank->forCheck), 0, 1);
  sem_init(&(bank->letMeStartNextDay), 0, 0);
  sem_init(&(bank->lockForReportTransfer), 0, 1);

  if (bank == NULL) {
    return bank;
  }

  Branch_Init(bank, numBranches, numAccounts, initalAmount);
  Report_Init(bank, reportingAmount, numWorkers);

  return bank;
}

/*
 * get the balance of the entire bank by adding up all the balances in
 * each branch.
 */
int
Bank_Balance(Bank *bank, AccountAmount *balance)
{
  for (size_t i = 0; i < bank->numberBranches; i++)
    sem_wait(&(bank->branches[i].branchLock));
  assert(bank->branches);

  AccountAmount bankTotal = 0;
  for (unsigned int branch = 0; branch < bank->numberBranches; branch++) {
    AccountAmount branchBalance;
    int err = Branch_Balance(bank,bank->branches[branch].branchID, &branchBalance);
    if (err < 0) {
      for (size_t i = 0; i < bank->numberBranches; i++)
        sem_post(&(bank->branches[i].branchLock));
      return err;
    }
    bankTotal += branchBalance;
  }

  *balance = bankTotal;
  for (size_t i = 0; i < bank->numberBranches; i++)
    sem_post(&(bank->branches[i].branchLock));
  return 0;
}

/*
 * tranverse and validate each branch.
 *
int
Bank_Validate(Bank *bank)
{
  assert(bank->branches);
  int err = 0;

  for (unsigned int branch = 0; branch < bank->numberBranches; branch++) {
    int berr = Branch_Validate(bank,bank->branches[branch].branchID);
    if (berr < 0) {
      err = berr;
    }
  }
  return err;
}

*
 * compare the data inside two banks and see they are exactly the same;
 * it is called in BankTest.
 */
int
Bank_Compare(Bank *bank1, Bank *bank2)
{
  int err = 0;
  if (bank1->numberBranches != bank2->numberBranches) {
    fprintf(stderr, "Bank num branches mismatch\n");
    return -1;
  }

  for (unsigned int branch = 0; branch < bank1->numberBranches; branch++) {
    int berr = Branch_Compare(&bank1->branches[branch],
                              &bank2->branches[branch]);
    if (berr < 0) {
      err = berr;
    }
  }

  int cerr = Report_Compare(bank1, bank2);
  if (cerr < 0)
    err = cerr;

  return err;

}

void free_all(Bank *bank){
  for (unsigned int i = 0; i < bank->numberBranches; i++) {
    Branch *branch=&bank->branches[i];
    for (int j = 0; j < branch->numberAccounts; j++) {
      Account *account=&branch->accounts[j];
      sem_destroy(&account->accLock);
    }
    sem_destroy(&branch->branchLock);
    free(branch->accounts);
  }
  sem_destroy(&bank->forCheck);
  sem_destroy(&bank->letMeStartNextDay);
  sem_destroy(&bank->lockForReportTransfer);
  free(bank->report);
  free(bank->branches);
  free(bank);
}
