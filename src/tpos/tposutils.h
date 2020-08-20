#ifndef TPOSUTILS_H
#define TPOSUTILS_H

#include <string>
#include <memory>
#include <amount.h>
#include <script/standard.h>
#include <pubkey.h>
#include <key_io.h>

class CWallet;
class CWalletTx;
class CMutableTransaction;
class CReserveKey;
class CValidationState;

class TPoSContract
{
public:
    static const uint16_t CURRENT_VERSION = 2;

public:
    uint16_t nVersion{CURRENT_VERSION};                    // message version
    uint16_t nType{0};                                     // only 0 supported for now
    uint16_t nOperatorReward { 0 };                        // operator reward in % from 0 to 100
    CScript scriptTPoSAddress;
    CScript scriptMerchantAddress;
    std::vector<unsigned char> vchSig;

    CTransactionRef txContract; // memonly

    TPoSContract() = default;
    TPoSContract(CTransactionRef tx,
                 CTxDestination merchantAddress,
                 CTxDestination tposAddress,
                 uint16_t nOperatorReward,
                 std::vector<unsigned char> vchSignature);

    bool IsValid() const;

    static TPoSContract FromTPoSContractTx(const CTransactionRef tx);
};

class TPoSUtils
{
public:
    TPoSUtils() = delete;
    ~TPoSUtils() = delete;

    static std::string PrepareTPoSExportBlock(std::string content);
    static std::string ParseTPoSExportBlock(std::string block);

    static bool IsTPoSContract(const CTransactionRef &tx);

#ifdef ENABLE_WALLET
    static bool GetTPoSPayments(const CWallet *wallet,
                                const CTransactionRef &tx,
                                CAmount &stakeAmount,
                                CAmount &commissionAmount,
                                CTxDestination &tposAddress, CTxDestination &merchantAddress);

    static bool IsTPoSOwnerContract(CWallet *wallet, const CTransactionRef &tx);
    static bool IsTPoSMerchantContract(CWallet *wallet, const CTransactionRef &tx);

    static bool CreateTPoSTransaction(CWallet *wallet,
                                      CTransactionRef &transactionOut,
                                      CReserveKey &reserveKey,
                                      const CBitcoinAddress &tposAddress,
                                      const CBitcoinAddress &merchantAddress,
                                      int merchantCommission,
                                      std::string &strError);

    static bool CreateCancelContractTransaction(CWallet *wallet,
                                                CTransactionRef &txOut,
                                                CReserveKey &reserveKey,
                                                const TPoSContract &contract,
                                                std::string &strError);

    static COutPoint GetContractCollateralOutpoint(const TPoSContract &contract);
    static bool CheckContract(const uint256 &hashContractTx, TPoSContract &contract, int nBlockHeight, bool fCheckSignature, bool fCheckContractOutpoint, std::string &strError);
    static bool CheckContract(const CTransactionRef &txContract, TPoSContract &contract, int nBlockHeight, bool fCheckSignature, bool fCheckContractOutpoint, std::string &strError);
    static bool IsMerchantPaymentValid(CValidationState &state, const CBlock &block, int nBlockHeight, CAmount expectedReward, CAmount actualReward);

#endif

};

#endif // TPOSUTILS_H
