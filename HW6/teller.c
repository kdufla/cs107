#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "teller.h"
#include "account.h"
#include "account.c"
#include "error.h"
#include "branch.h"
#include "debug.h"

void acc_branch(int func(sem_t *__sem), Bank* bank, Account* account, int branchID){
  func(&(account->accLock));
  func(&(bank->branches[branchID].branchLock));
}

void acc_acc(int func(sem_t *__sem), Account* acc1, Account* acc2){
  func(&(acc1->accLock));
  func(&(acc2->accLock));
}

void branch_branch(int func(sem_t *__sem), Bank* bank, int branchID1, int branchID2){
  func(&(bank->branches[branchID1].branchLock));
  func(&(bank->branches[branchID2].branchLock));
}

/*
 * deposit money into an account
 */
int
Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);
  

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  sem_wait(&(account->accLock));
  int branchID = AccountNum_GetBranchID(accountNum);
  sem_wait(&(bank->branches[branchID].branchLock));

  

  Account_Adjust(bank,account, amount, 1);
  acc_branch(sem_post, bank, account, branchID);
  return ERROR_SUCCESS;
}

/*
 * withdraw money from an account
 */
int
Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);
  int branchID = AccountNum_GetBranchID(accountNum);

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  sem_wait(&(account->accLock));
  
  if (amount > Account_Balance(account)) {
    sem_post(&(account->accLock));
    return ERROR_INSUFFICIENT_FUNDS;
  }
  sem_wait(&(bank->branches[branchID].branchLock));


  Account_Adjust(bank,account, -amount, 1);
  acc_branch(sem_post, bank, account, branchID);
  return ERROR_SUCCESS;
}

/*
 * do a tranfer from one account to another account
 */
int
Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum,
                  AccountNumber dstAccountNum,
                  AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));
  
  if(srcAccountNum==dstAccountNum)return ERROR_SUCCESS;
  
  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  int  srcBranchID = AccountNum_GetBranchID(srcAccountNum);
  if (srcAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  int  dstBranchID = AccountNum_GetBranchID(dstAccountNum);
  if (dstAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  /*
   * If we are doing a transfer within the branch, we tell the Account module to
   * not bother updating the branch balance since the net change for the
   * branch is 0.
   */
  int updateBranch = !Account_IsSameBranch(srcAccountNum, dstAccountNum);


  if (updateBranch){
    if(srcBranchID < dstBranchID) {
      acc_acc(sem_wait,srcAccount,dstAccount);
      branch_branch(sem_wait,bank,srcBranchID,dstBranchID);
    } else {
      acc_acc(sem_wait,dstAccount,srcAccount);
      branch_branch(sem_wait,bank,dstBranchID,srcBranchID);
    }

    if (amount > Account_Balance(srcAccount)) {
      acc_acc(sem_post,srcAccount,dstAccount);
      branch_branch(sem_post, bank, srcBranchID, dstBranchID);
      
      return ERROR_INSUFFICIENT_FUNDS;
    }

    Account_Adjust(bank, srcAccount, -amount, updateBranch);
    Account_Adjust(bank, dstAccount, amount, updateBranch);
    acc_acc(sem_post,srcAccount,dstAccount);
    branch_branch(sem_post, bank, srcBranchID, dstBranchID);
   
    return ERROR_SUCCESS;

  }else{
    if(srcAccount->accountNumber < dstAccount->accountNumber) {
      acc_acc(sem_wait,srcAccount,dstAccount);
    } else {
      acc_acc(sem_wait,dstAccount,srcAccount);
    }

    if (amount > Account_Balance(srcAccount)) {
      acc_acc(sem_post, srcAccount, dstAccount);
      return ERROR_INSUFFICIENT_FUNDS;
    }

    Account_Adjust(bank, srcAccount, -amount, updateBranch);
    Account_Adjust(bank, dstAccount, amount, updateBranch);

    acc_acc(sem_post, srcAccount, dstAccount);

    return ERROR_SUCCESS;
  }
}
