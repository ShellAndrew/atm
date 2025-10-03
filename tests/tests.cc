#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}


TEST("BankTests", "RegisterAccount_Success") {
    Bank bank;
    bank.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
    // Should not throw, account should exist
    EXPECT_NO_THROW(bank.RegisterAccount(98765432, 4321, "John Doe", 1000.0));
}

TEST("BankTests", "RegisterAccount_DuplicateThrows") {
    Bank bank;
    bank.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
    EXPECT_THROW(bank.RegisterAccount(12345678, 1234, "Duplicate", 500.0), std::invalid_argument);
}

TEST("BankTests", "WithdrawCash_Success") {
    Bank bank;
    bank.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
    EXPECT_NO_THROW(bank.WithdrawCash(12345678, 1234, 200.0));
}

TEST("BankTests", "WithdrawCash_InsufficientFundsThrows") {
    Bank bank;
    bank.RegisterAccount(12345678, 1234, "Sam Sepiol", 100.0);
    EXPECT_THROW(bank.WithdrawCash(12345678, 1234, 200.0), std::runtime_error);
}

TEST("BankTests", "WithdrawCash_InvalidAccountThrows") {
    Bank bank;
    EXPECT_THROW(bank.WithdrawCash(11111111, 9999, 50.0), std::invalid_argument);
}

TEST("BankTests", "WithdrawCash_NegativeAmountThrows") {
    Bank bank;
    bank.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.0);
    EXPECT_THROW(bank.WithdrawCash(12345678, 1234, -50.0), std::invalid_argument);
}

TEST("BankTests", "DepositCash_Success") {
    Bank bank;
    bank.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.0);
    EXPECT_NO_THROW(bank.DepositCash(12345678, 1234, 200.0));
}

TEST("BankTests", "DepositCash_InvalidAccountThrows") {
    Bank bank;
    EXPECT_THROW(bank.DepositCash(11111111, 9999, 200.0), std::invalid_argument);
}

TEST("BankTests", "DepositCash_NegativeAmountThrows") {
    Bank bank;
    bank.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.0);
    EXPECT_THROW(bank.DepositCash(12345678, 1234, -200.0), std::invalid_argument);
}

TEST("BankTests", "PrintLedger_Success") {
    Bank bank;
    bank.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
    bank.WithdrawCash(12345678, 1234, 200.40);
    bank.DepositCash(12345678, 1234, 40000.0);
    bank.DepositCash(12345678, 1234, 32000.0);

    std::string filepath = "ledger_test.txt";
    EXPECT_NO_THROW(bank.PrintLedger(filepath, 12345678, 1234));

    // Verify file content
    std::ifstream file(filepath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("Sam Sepiol"), std::string::npos);
    EXPECT_NE(content.find("Withdrawal - Amount: $200.40"), std::string::npos);
    EXPECT_NE(content.find("Deposit - Amount: $40000.00"), std::string::npos);
    EXPECT_NE(content.find("Deposit - Amount: $32000.00"), std::string::npos);
}

TEST("BankTests", "PrintLedger_InvalidAccountThrows") {
    Bank bank;
    EXPECT_THROW(bank.PrintLedger("ledger_fail.txt", 12345678, 1234), std::invalid_argument);
}

