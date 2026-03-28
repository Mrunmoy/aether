#include <gtest/gtest.h>
#include "AudioDsp.h"
#include "ClientBase.h"

#include <chrono>
#include <cmath>
#include <cstring>
#include <mutex>
#include <thread>

using namespace aether::ipc;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
#if defined(_WIN32)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
#endif
}

// ── Test server ─────────────────────────────────────────────────────

class TestDsp : public AudioDsp
{
public:
    using AudioDsp::AudioDsp;

    using AudioDsp::notifyClipDetected;
    using AudioDsp::notifyLevelUpdate;

    static constexpr uint8_t kMaxSlots = 8;

protected:
    int handleAddEffect(uint8_t slotIndex, EffectType type,
                        const char *name, uint8_t *success) override
    {
        if (slotIndex >= kMaxSlots)
            return IPC_ERR_INVALID_ARGUMENT;

        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_slots[slotIndex].enabled)
        {
            *success = 0;
            return IPC_SUCCESS;
        }

        m_slots[slotIndex].slotIndex = slotIndex;
        m_slots[slotIndex].type = type;
        m_slots[slotIndex].enabled = 1;
        m_slots[slotIndex].param1 = 0.0f;
        m_slots[slotIndex].param2 = 0.0f;
        m_slots[slotIndex].param3 = 0.0f;
        std::strncpy(m_slots[slotIndex].name, name, 32);
        m_slots[slotIndex].name[32] = '\0';
        *success = 1;
        return IPC_SUCCESS;
    }

    int handleRemoveEffect(uint8_t slotIndex, uint8_t *success) override
    {
        if (slotIndex >= kMaxSlots)
            return IPC_ERR_INVALID_ARGUMENT;

        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_slots[slotIndex].enabled)
        {
            *success = 0;
            return IPC_SUCCESS;
        }

        m_slots[slotIndex] = {};
        *success = 1;
        return IPC_SUCCESS;
    }

    int handleSetParameter(uint8_t slotIndex, uint8_t paramIndex,
                           float value) override
    {
        if (slotIndex >= kMaxSlots)
            return IPC_ERR_INVALID_ARGUMENT;

        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_slots[slotIndex].enabled)
            return IPC_ERR_INVALID_ARGUMENT;

        switch (paramIndex)
        {
        case 0: m_slots[slotIndex].param1 = value; break;
        case 1: m_slots[slotIndex].param2 = value; break;
        case 2: m_slots[slotIndex].param3 = value; break;
        default: return IPC_ERR_INVALID_ARGUMENT;
        }
        return IPC_SUCCESS;
    }

    int handleGetEffectSlot(uint8_t slotIndex, EffectSlot *slot) override
    {
        if (slotIndex >= kMaxSlots)
            return IPC_ERR_INVALID_ARGUMENT;

        std::lock_guard<std::mutex> lock(m_mutex);
        *slot = m_slots[slotIndex];
        return IPC_SUCCESS;
    }

    int handleGetSignalLevel(SignalLevel *level) override
    {
        *level = {-6.2f, -12.4f, 0};
        return IPC_SUCCESS;
    }

    int handleGetEffectCount(uint8_t *count) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        uint8_t n = 0;
        for (uint8_t i = 0; i < kMaxSlots; ++i)
        {
            if (m_slots[i].enabled)
                ++n;
        }
        *count = n;
        return IPC_SUCCESS;
    }

private:
    std::mutex m_mutex;
    EffectSlot m_slots[kMaxSlots]{};
};

// ── Notification client ─────────────────────────────────────────────

class NotifyClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    std::mutex mtx;
    std::condition_variable cv;
    uint32_t lastServiceId = 0;
    uint32_t lastMessageId = 0;
    std::vector<uint8_t> lastPayload;
    int notifyCount = 0;

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        lastServiceId = serviceId;
        lastMessageId = messageId;
        lastPayload = payload;
        notifyCount++;
        cv.notify_all();
    }
};

// ── Request helpers ─────────────────────────────────────────────────

static std::vector<uint8_t> makeAddEffectReq(uint8_t slot, EffectType type,
                                             const char *name)
{
    std::vector<uint8_t> req(sizeof(uint8_t) + sizeof(EffectType) + 33);
    std::memcpy(req.data(), &slot, sizeof(slot));
    std::memcpy(req.data() + sizeof(uint8_t), &type, sizeof(type));
    char buf[33] = {};
    std::strncpy(buf, name, 32);
    std::memcpy(req.data() + sizeof(uint8_t) + sizeof(EffectType), buf, 33);
    return req;
}

static std::vector<uint8_t> makeSlotReq(uint8_t slot)
{
    std::vector<uint8_t> req(sizeof(slot));
    std::memcpy(req.data(), &slot, sizeof(slot));
    return req;
}

static std::vector<uint8_t> makeSetParamReq(uint8_t slot, uint8_t paramIdx,
                                            float value)
{
    std::vector<uint8_t> req(sizeof(uint8_t) + sizeof(uint8_t) + sizeof(float));
    std::memcpy(req.data(), &slot, sizeof(slot));
    std::memcpy(req.data() + sizeof(uint8_t), &paramIdx, sizeof(paramIdx));
    std::memcpy(req.data() + sizeof(uint8_t) + sizeof(uint8_t), &value, sizeof(value));
    return req;
}

static uint8_t parseSuccess(const std::vector<uint8_t> &resp)
{
    uint8_t s = 0;
    if (resp.size() >= sizeof(s))
        std::memcpy(&s, resp.data(), sizeof(s));
    return s;
}

// ═════════════════════════════════════════════════════════════════════
// AddEffect_Succeeds — add EQ to slot 0, returns success=1
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, AddEffect_Succeeds)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto req = makeAddEffectReq(0, EQ, "Main EQ");
    std::vector<uint8_t> resp;
    int rc = client.call(AudioDsp::kServiceId, AudioDsp::kAddEffect, req, &resp);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(parseSuccess(resp), 1u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// AddEffect_OccupiedSlot — add to already occupied slot, returns success=0
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, AddEffect_OccupiedSlot)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto req1 = makeAddEffectReq(0, EQ, "EQ1");
    std::vector<uint8_t> resp;
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kAddEffect, req1, &resp), IPC_SUCCESS);
    ASSERT_EQ(parseSuccess(resp), 1u);

    auto req2 = makeAddEffectReq(0, Compressor, "Comp");
    resp.clear();
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kAddEffect, req2, &resp), IPC_SUCCESS);
    EXPECT_EQ(parseSuccess(resp), 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RemoveEffect_Succeeds — add then remove, returns success=1
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, RemoveEffect_Succeeds)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto addReq = makeAddEffectReq(3, Reverb, "Hall");
    std::vector<uint8_t> resp;
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kAddEffect, addReq, &resp), IPC_SUCCESS);
    ASSERT_EQ(parseSuccess(resp), 1u);

    auto rmReq = makeSlotReq(3);
    resp.clear();
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kRemoveEffect, rmReq, &resp), IPC_SUCCESS);
    EXPECT_EQ(parseSuccess(resp), 1u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RemoveEffect_EmptySlot — remove from empty slot, returns success=0
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, RemoveEffect_EmptySlot)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto req = makeSlotReq(5);
    std::vector<uint8_t> resp;
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kRemoveEffect, req, &resp), IPC_SUCCESS);
    EXPECT_EQ(parseSuccess(resp), 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetEffectSlot_ValidSlot — verify returned data matches what was added
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, GetEffectSlot_ValidSlot)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto addReq = makeAddEffectReq(2, Compressor, "Bus Comp");
    std::vector<uint8_t> resp;
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kAddEffect, addReq, &resp), IPC_SUCCESS);
    ASSERT_EQ(parseSuccess(resp), 1u);

    auto getReq = makeSlotReq(2);
    resp.clear();
    int rc = client.call(AudioDsp::kServiceId, AudioDsp::kGetEffectSlot, getReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(EffectSlot));

    EffectSlot slot;
    std::memcpy(&slot, resp.data(), sizeof(slot));
    EXPECT_EQ(slot.slotIndex, 2u);
    EXPECT_EQ(slot.type, Compressor);
    EXPECT_EQ(slot.enabled, 1u);
    EXPECT_STREQ(slot.name, "Bus Comp");

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetEffectSlot_EmptySlot — verify enabled=0
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, GetEffectSlot_EmptySlot)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto req = makeSlotReq(7);
    std::vector<uint8_t> resp;
    int rc = client.call(AudioDsp::kServiceId, AudioDsp::kGetEffectSlot, req, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(EffectSlot));

    EffectSlot slot;
    std::memcpy(&slot, resp.data(), sizeof(slot));
    EXPECT_EQ(slot.enabled, 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// SetParameter_UpdatesValue — set param, get slot, verify param changed
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, SetParameter_UpdatesValue)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto addReq = makeAddEffectReq(1, EQ, "Para EQ");
    std::vector<uint8_t> resp;
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kAddEffect, addReq, &resp), IPC_SUCCESS);

    auto paramReq = makeSetParamReq(1, 0, 2.5f);
    resp.clear();
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kSetParameter, paramReq, &resp), IPC_SUCCESS);

    auto getReq = makeSlotReq(1);
    resp.clear();
    ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kGetEffectSlot, getReq, &resp), IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(EffectSlot));

    EffectSlot slot;
    std::memcpy(&slot, resp.data(), sizeof(slot));
    EXPECT_FLOAT_EQ(slot.param1, 2.5f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetEffectCount_TracksCorrectly — add 3, verify 3, remove 1, verify 2
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, GetEffectCount_TracksCorrectly)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Add 3 effects
    for (uint8_t i = 0; i < 3; ++i)
    {
        auto req = makeAddEffectReq(i, EQ, "EQ");
        std::vector<uint8_t> resp;
        ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kAddEffect, req, &resp), IPC_SUCCESS);
    }

    // Verify count = 3
    {
        std::vector<uint8_t> req;
        std::vector<uint8_t> resp;
        ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kGetEffectCount, req, &resp), IPC_SUCCESS);
        ASSERT_EQ(resp.size(), sizeof(uint8_t));
        uint8_t count;
        std::memcpy(&count, resp.data(), sizeof(count));
        EXPECT_EQ(count, 3u);
    }

    // Remove slot 1
    {
        auto req = makeSlotReq(1);
        std::vector<uint8_t> resp;
        ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kRemoveEffect, req, &resp), IPC_SUCCESS);
    }

    // Verify count = 2
    {
        std::vector<uint8_t> req;
        std::vector<uint8_t> resp;
        ASSERT_EQ(client.call(AudioDsp::kServiceId, AudioDsp::kGetEffectCount, req, &resp), IPC_SUCCESS);
        ASSERT_EQ(resp.size(), sizeof(uint8_t));
        uint8_t count;
        std::memcpy(&count, resp.data(), sizeof(count));
        EXPECT_EQ(count, 2u);
    }

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetSignalLevel_ReturnsFiniteValues — peak and rms are finite
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, GetSignalLevel_ReturnsFiniteValues)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> req;
    std::vector<uint8_t> resp;
    int rc = client.call(AudioDsp::kServiceId, AudioDsp::kGetSignalLevel, req, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(SignalLevel));

    SignalLevel level;
    std::memcpy(&level, resp.data(), sizeof(level));
    EXPECT_TRUE(std::isfinite(level.peakDb));
    EXPECT_TRUE(std::isfinite(level.rmsDb));

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// InvalidSlot_ReturnsError — slot 99 returns error
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, InvalidSlot_ReturnsError)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto req = makeSlotReq(99);
    std::vector<uint8_t> resp;
    int rc = client.call(AudioDsp::kServiceId, AudioDsp::kGetEffectSlot, req, &resp);
    EXPECT_EQ(rc, IPC_ERR_INVALID_ARGUMENT);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// ClipDetected_NotificationFires — verify ClipDetected notification
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, ClipDetected_NotificationFires)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.notifyClipDetected(-0.5f), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, AudioDsp::kServiceId);
    EXPECT_EQ(client.lastMessageId, AudioDsp::kClipDetected);
    ASSERT_GE(client.lastPayload.size(), sizeof(float));

    float peakDb;
    std::memcpy(&peakDb, client.lastPayload.data(), sizeof(peakDb));
    EXPECT_FLOAT_EQ(peakDb, -0.5f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// LevelUpdate_NotificationFires — verify LevelUpdate notification
// ═════════════════════════════════════════════════════════════════════

TEST(AudioDspTest, LevelUpdate_NotificationFires)
{
    TestDsp svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    SignalLevel sent{-5.0f, -11.0f, 1};
    ASSERT_EQ(svc.notifyLevelUpdate(sent), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, AudioDsp::kServiceId);
    EXPECT_EQ(client.lastMessageId, AudioDsp::kLevelUpdate);
    ASSERT_GE(client.lastPayload.size(), sizeof(SignalLevel));

    SignalLevel received;
    std::memcpy(&received, client.lastPayload.data(), sizeof(received));
    EXPECT_FLOAT_EQ(received.peakDb, -5.0f);
    EXPECT_FLOAT_EQ(received.rmsDb, -11.0f);
    EXPECT_EQ(received.clipping, 1u);

    client.disconnect();
    svc.stop();
}
