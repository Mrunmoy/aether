/*
 * CApiTest.cpp — Google Test suite for the C API (aether_ipc.h).
 *
 * Validates the extern "C" wrapper: create/destroy, start/stop,
 * connect/call/disconnect, notifications, and error paths.
 */

#include <gtest/gtest.h>

#include "aether_ipc.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <thread>

// Unique per-test service name to avoid UDS collisions
#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

/* ── Null-argument guards ──────────────────────────────────────────── */

TEST(CApiTest, CreateServiceNullArgs)
{
    aether_service_t svc = nullptr;
    EXPECT_EQ(AETHER_ERR_INVALID_ARGUMENT, aether_service_create(nullptr, nullptr, nullptr, &svc));
    EXPECT_EQ(nullptr, svc);
}

TEST(CApiTest, CreateClientNullArgs)
{
    aether_client_t cli = nullptr;
    EXPECT_EQ(AETHER_ERR_INVALID_ARGUMENT, aether_client_create(nullptr, nullptr, nullptr, &cli));
    EXPECT_EQ(nullptr, cli);
}

TEST(CApiTest, DestroyNullIsNoOp)
{
    aether_service_destroy(nullptr);
    aether_client_destroy(nullptr);
}

TEST(CApiTest, StopNullIsNoOp)
{
    aether_service_stop(nullptr);
    aether_client_disconnect(nullptr);
}

TEST(CApiTest, IsRunningNullReturnsFalse)
{
    EXPECT_EQ(0, aether_service_is_running(nullptr));
    EXPECT_EQ(0, aether_client_is_connected(nullptr));
}

/* ── Basic echo ────────────────────────────────────────────────────── */

static int echoHandler(uint32_t /*serviceId*/, uint32_t /*messageId*/,
                       const uint8_t *req, uint32_t reqLen,
                       uint8_t **resp, uint32_t *respLen,
                       void * /*userData*/)
{
    if (reqLen > 0)
    {
        *resp = static_cast<uint8_t *>(std::malloc(reqLen));
        std::memcpy(*resp, req, reqLen);
        *respLen = reqLen;
    }
    else
    {
        *resp = nullptr;
        *respLen = 0;
    }
    return AETHER_SUCCESS;
}

TEST(CApiTest, EchoRoundTrip)
{
    aether_service_t svc = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_service_create(SVC_NAME, echoHandler, nullptr, &svc));
    ASSERT_NE(nullptr, svc);
    ASSERT_EQ(AETHER_SUCCESS, aether_service_start(svc));
    EXPECT_EQ(1, aether_service_is_running(svc));
    settle();

    aether_client_t cli = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_client_create(SVC_NAME, nullptr, nullptr, &cli));
    ASSERT_EQ(AETHER_SUCCESS, aether_client_connect(cli));
    EXPECT_EQ(1, aether_client_is_connected(cli));
    settle();

    const uint8_t msg[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t resp[16] = {};
    uint32_t respLen = 0;

    int rc = aether_client_call(cli, 0, 1, msg, sizeof(msg),
                                resp, sizeof(resp), &respLen, 2000);
    EXPECT_EQ(AETHER_SUCCESS, rc);
    EXPECT_EQ(sizeof(msg), respLen);
    EXPECT_EQ(0, std::memcmp(msg, resp, sizeof(msg)));

    aether_client_destroy(cli);
    aether_service_destroy(svc);
}

/* ── Empty payload ─────────────────────────────────────────────────── */

TEST(CApiTest, EmptyPayload)
{
    aether_service_t svc = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_service_create(SVC_NAME, echoHandler, nullptr, &svc));
    ASSERT_EQ(AETHER_SUCCESS, aether_service_start(svc));
    settle();

    aether_client_t cli = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_client_create(SVC_NAME, nullptr, nullptr, &cli));
    ASSERT_EQ(AETHER_SUCCESS, aether_client_connect(cli));
    settle();

    uint8_t resp[4] = {};
    uint32_t respLen = 99;

    int rc = aether_client_call(cli, 0, 1, nullptr, 0,
                                resp, sizeof(resp), &respLen, 2000);
    EXPECT_EQ(AETHER_SUCCESS, rc);
    EXPECT_EQ(0u, respLen);

    aether_client_destroy(cli);
    aether_service_destroy(svc);
}

/* ── User data forwarding ──────────────────────────────────────────── */

static int userDataHandler(uint32_t /*serviceId*/, uint32_t /*messageId*/,
                           const uint8_t * /*req*/, uint32_t /*reqLen*/,
                           uint8_t **resp, uint32_t *respLen,
                           void *userData)
{
    auto *counter = static_cast<std::atomic<int> *>(userData);
    counter->fetch_add(1);
    *resp = nullptr;
    *respLen = 0;
    return AETHER_SUCCESS;
}

TEST(CApiTest, UserDataForwarded)
{
    std::atomic<int> counter{0};

    aether_service_t svc = nullptr;
    ASSERT_EQ(AETHER_SUCCESS,
              aether_service_create(SVC_NAME, userDataHandler, &counter, &svc));
    ASSERT_EQ(AETHER_SUCCESS, aether_service_start(svc));
    settle();

    aether_client_t cli = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_client_create(SVC_NAME, nullptr, nullptr, &cli));
    ASSERT_EQ(AETHER_SUCCESS, aether_client_connect(cli));
    settle();

    uint8_t dummy = 0;
    uint8_t resp[4] = {};
    uint32_t respLen = 0;

    aether_client_call(cli, 0, 1, &dummy, 1, resp, sizeof(resp), &respLen, 2000);
    aether_client_call(cli, 0, 1, &dummy, 1, resp, sizeof(resp), &respLen, 2000);

    EXPECT_EQ(2, counter.load());

    aether_client_destroy(cli);
    aether_service_destroy(svc);
}

/* ── Handler error code forwarded ──────────────────────────────────── */

static int errorHandler(uint32_t /*serviceId*/, uint32_t /*messageId*/,
                        const uint8_t * /*req*/, uint32_t /*reqLen*/,
                        uint8_t **resp, uint32_t *respLen,
                        void * /*userData*/)
{
    *resp = nullptr;
    *respLen = 0;
    return 42; // application error
}

TEST(CApiTest, HandlerErrorForwarded)
{
    aether_service_t svc = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_service_create(SVC_NAME, errorHandler, nullptr, &svc));
    ASSERT_EQ(AETHER_SUCCESS, aether_service_start(svc));
    settle();

    aether_client_t cli = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_client_create(SVC_NAME, nullptr, nullptr, &cli));
    ASSERT_EQ(AETHER_SUCCESS, aether_client_connect(cli));
    settle();

    uint8_t dummy = 0;
    uint8_t resp[4] = {};
    uint32_t respLen = 0;

    int rc = aether_client_call(cli, 0, 1, &dummy, 1, resp, sizeof(resp), &respLen, 2000);
    EXPECT_EQ(42, rc);

    aether_client_destroy(cli);
    aether_service_destroy(svc);
}

/* ── Call on disconnected client ───────────────────────────────────── */

TEST(CApiTest, CallWithoutConnect)
{
    aether_client_t cli = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_client_create(SVC_NAME, nullptr, nullptr, &cli));

    uint8_t dummy = 0;
    uint8_t resp[4] = {};
    uint32_t respLen = 0;

    int rc = aether_client_call(cli, 0, 1, &dummy, 1, resp, sizeof(resp), &respLen, 500);
    EXPECT_NE(AETHER_SUCCESS, rc);

    aether_client_destroy(cli);
}

/* ── Multiple calls ────────────────────────────────────────────────── */

TEST(CApiTest, MultipleCalls)
{
    aether_service_t svc = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_service_create(SVC_NAME, echoHandler, nullptr, &svc));
    ASSERT_EQ(AETHER_SUCCESS, aether_service_start(svc));
    settle();

    aether_client_t cli = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_client_create(SVC_NAME, nullptr, nullptr, &cli));
    ASSERT_EQ(AETHER_SUCCESS, aether_client_connect(cli));
    settle();

    for (int i = 0; i < 100; ++i)
    {
        uint8_t val = static_cast<uint8_t>(i);
        uint8_t resp[4] = {};
        uint32_t respLen = 0;

        int rc = aether_client_call(cli, 0, 1, &val, 1, resp, sizeof(resp), &respLen, 2000);
        ASSERT_EQ(AETHER_SUCCESS, rc) << "Failed on iteration " << i;
        ASSERT_EQ(1u, respLen);
        EXPECT_EQ(val, resp[0]);
    }

    aether_client_destroy(cli);
    aether_service_destroy(svc);
}

/* ── aether_free on nullptr ────────────────────────────────────────── */

TEST(CApiTest, FreeNullIsNoOp)
{
    aether_free(nullptr); // must not crash
}

/* ── Start/stop cycle ──────────────────────────────────────────────── */

TEST(CApiTest, StartStopCycle)
{
    aether_service_t svc = nullptr;
    ASSERT_EQ(AETHER_SUCCESS, aether_service_create(SVC_NAME, echoHandler, nullptr, &svc));

    EXPECT_EQ(0, aether_service_is_running(svc));
    ASSERT_EQ(AETHER_SUCCESS, aether_service_start(svc));
    EXPECT_EQ(1, aether_service_is_running(svc));

    aether_service_stop(svc);
    EXPECT_EQ(0, aether_service_is_running(svc));

    aether_service_destroy(svc);
}
