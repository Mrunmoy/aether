/*
 * CApi.cpp — C ABI wrapper for the Aether IPC library.
 *
 * Implements the stable extern "C" API declared in aether_ipc.h.
 * Internally subclasses ServiceBase and ClientBase to route callbacks
 * through C function pointers.
 */

#include "aether_ipc.h"
#include "ClientBase.h"
#include "ServiceBase.h"

#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>

using namespace aether::ipc;

/* ── Server wrapper ────────────────────────────────────────────────── */

class CServiceWrapper : public ServiceBase
{
public:
    CServiceWrapper(const char *name,
                    aether_request_cb cb,
                    void *userData)
        : ServiceBase(name)
        , m_cb(cb)
        , m_userData(userData)
    {
    }

    // Expose protected sendNotify through a public method for the C API
    int notify(uint32_t serviceId, uint32_t messageId,
               const uint8_t *payload, uint32_t payloadBytes)
    {
        return sendNotify(serviceId, messageId, payload, payloadBytes);
    }

protected:
    int onRequest(uint32_t messageId,
                  const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        if (!m_cb)
        {
            return AETHER_ERR_INVALID_METHOD;
        }

        uint8_t *respBuf = nullptr;
        uint32_t respLen = 0;

        // serviceId is not provided by ServiceBase::onRequest — pass 0.
        // Generated skeletons already dispatch by serviceId before reaching
        // the handler; the C API exposes a flat request callback.
        int rc = m_cb(0, messageId,
                      request.data(),
                      static_cast<uint32_t>(request.size()),
                      &respBuf, &respLen,
                      m_userData);

        if (respBuf && respLen > 0)
        {
            response->assign(respBuf, respBuf + respLen);
            std::free(respBuf);
        }

        return rc;
    }

private:
    aether_request_cb m_cb;
    void *m_userData;
};

/* ── Client wrapper ────────────────────────────────────────────────── */

class CClientWrapper : public ClientBase
{
public:
    CClientWrapper(const char *name,
                   aether_notify_cb cb,
                   void *userData)
        : ClientBase(name)
        , m_cb(cb)
        , m_userData(userData)
    {
    }

protected:
    void onNotification(uint32_t serviceId,
                        uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        if (m_cb)
        {
            m_cb(serviceId, messageId,
                 payload.data(),
                 static_cast<uint32_t>(payload.size()),
                 m_userData);
        }
    }

private:
    aether_notify_cb m_cb;
    void *m_userData;
};

/* ── Server API ────────────────────────────────────────────────────── */

extern "C"
{

int aether_service_create(const char *name,
                          aether_request_cb on_request,
                          void *user_data,
                          aether_service_t *out)
{
    if (!name || !on_request || !out)
    {
        return AETHER_ERR_INVALID_ARGUMENT;
    }

    try
    {
        auto *svc = new CServiceWrapper(name, on_request, user_data);
        *out = reinterpret_cast<aether_service_t>(svc);
        return AETHER_SUCCESS;
    }
    catch (...)
    {
        return AETHER_ERR_INVALID_ARGUMENT;
    }
}

int aether_service_start(aether_service_t svc)
{
    if (!svc)
    {
        return AETHER_ERR_INVALID_ARGUMENT;
    }
    auto *impl = reinterpret_cast<CServiceWrapper *>(svc);
    return impl->start() ? AETHER_SUCCESS : AETHER_ERR_STOPPED;
}

void aether_service_stop(aether_service_t svc)
{
    if (!svc)
    {
        return;
    }
    auto *impl = reinterpret_cast<CServiceWrapper *>(svc);
    impl->stop();
}

int aether_service_is_running(aether_service_t svc)
{
    if (!svc)
    {
        return 0;
    }
    auto *impl = reinterpret_cast<CServiceWrapper *>(svc);
    return impl->isRunning() ? 1 : 0;
}

void aether_service_destroy(aether_service_t svc)
{
    if (!svc)
    {
        return;
    }
    auto *impl = reinterpret_cast<CServiceWrapper *>(svc);
    impl->stop();
    delete impl;
}

int aether_service_notify(aether_service_t svc,
                          uint32_t service_id,
                          uint32_t message_id,
                          const uint8_t *payload, uint32_t len)
{
    if (!svc)
    {
        return AETHER_ERR_INVALID_ARGUMENT;
    }
    auto *impl = reinterpret_cast<CServiceWrapper *>(svc);
    return impl->notify(service_id, message_id, payload, len);
}

/* ── Client API ────────────────────────────────────────────────────── */

int aether_client_create(const char *name,
                         aether_notify_cb on_notify,
                         void *user_data,
                         aether_client_t *out)
{
    if (!name || !out)
    {
        return AETHER_ERR_INVALID_ARGUMENT;
    }

    try
    {
        auto *cli = new CClientWrapper(name, on_notify, user_data);
        *out = reinterpret_cast<aether_client_t>(cli);
        return AETHER_SUCCESS;
    }
    catch (...)
    {
        return AETHER_ERR_INVALID_ARGUMENT;
    }
}

int aether_client_connect(aether_client_t cli)
{
    if (!cli)
    {
        return AETHER_ERR_INVALID_ARGUMENT;
    }
    auto *impl = reinterpret_cast<CClientWrapper *>(cli);
    return impl->connect() ? AETHER_SUCCESS : AETHER_ERR_DISCONNECTED;
}

void aether_client_disconnect(aether_client_t cli)
{
    if (!cli)
    {
        return;
    }
    auto *impl = reinterpret_cast<CClientWrapper *>(cli);
    impl->disconnect();
}

int aether_client_is_connected(aether_client_t cli)
{
    if (!cli)
    {
        return 0;
    }
    auto *impl = reinterpret_cast<CClientWrapper *>(cli);
    return impl->isConnected() ? 1 : 0;
}

void aether_client_destroy(aether_client_t cli)
{
    if (!cli)
    {
        return;
    }
    auto *impl = reinterpret_cast<CClientWrapper *>(cli);
    impl->disconnect();
    delete impl;
}

int aether_client_call(aether_client_t cli,
                       uint32_t service_id,
                       uint32_t message_id,
                       const uint8_t *req, uint32_t req_len,
                       uint8_t *resp, uint32_t resp_cap,
                       uint32_t *resp_len,
                       uint32_t timeout_ms)
{
    if (!cli)
    {
        return AETHER_ERR_INVALID_ARGUMENT;
    }

    auto *impl = reinterpret_cast<CClientWrapper *>(cli);

    std::vector<uint8_t> reqVec;
    if (req && req_len > 0)
    {
        reqVec.assign(req, req + req_len);
    }

    std::vector<uint8_t> respVec;
    uint32_t timeout = timeout_ms > 0 ? timeout_ms : 2000;

    int rc = impl->call(service_id, message_id, reqVec, &respVec, timeout);

    if (rc == AETHER_SUCCESS && resp && resp_cap > 0)
    {
        uint32_t copyLen = static_cast<uint32_t>(respVec.size());
        if (copyLen > resp_cap)
        {
            copyLen = resp_cap;
        }
        if (copyLen > 0)
        {
            std::memcpy(resp, respVec.data(), copyLen);
        }
        if (resp_len)
        {
            *resp_len = copyLen;
        }
    }
    else if (resp_len)
    {
        *resp_len = 0;
    }

    return rc;
}

/* ── Memory ────────────────────────────────────────────────────────── */

void aether_free(void *ptr)
{
    std::free(ptr);
}

} /* extern "C" */
