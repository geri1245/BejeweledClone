#pragma once

#include <string>
#include <vector>

struct CellDestructionData;

class IGameState {
public:
    virtual void UpdateScore(const CellDestructionData& datas) = 0;
    virtual std::vector<std::string> GetUIText() = 0;
    virtual std::vector<std::string> GetResult() = 0;

    virtual void Update(int deltaTime) { }
    virtual bool IsGameOver() { return false; }

    virtual ~IGameState() = default;
};

class ClassicGameState : public IGameState {
public:
    void UpdateScore(const CellDestructionData& datas) override;
    std::vector<std::string> GetUIText() override;
    std::vector<std::string> GetResult() override;

private:
    int _score;
};

class QuickDeathGameState : public IGameState {
public:
    bool IsGameOver() override;
    void Update(int deltaTime) override;

    void UpdateScore(const CellDestructionData& datas) override;
    std::vector<std::string> GetUIText() override;
    std::vector<std::string> GetResult() override;

private:
    static constexpr int InitialTimeLeft = 15000; // Start with 10 seconds

    int _timeLeft = InitialTimeLeft;
    uint64_t _timeSurvived = 0;
};
