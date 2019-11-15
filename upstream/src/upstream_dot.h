#pragma once

#include <utility>
#include <upstream.h>
#include <event2/event.h>
#include <ldns/net.h>
#include "dns_framed.h"
#include "bootstrapper.h"

namespace ag {

/**
 * Pool of TLS connections
 */
class tls_pool : public dns_framed_pool {
public:
    /**
     * Create TLS pool
     * @param loop Event loop
     * @param bootstrapper Bootstrapper (used to resolve original address)
     */
    tls_pool(event_loop_ptr loop, bootstrapper_ptr &&bootstrapper)
            : dns_framed_pool(std::move(loop)), m_bootstrapper(std::move(bootstrapper)) {
    }

    get_result get() override;

    /**
     * @return Bootstrapper for server address
     */
    bootstrapper_ptr bootstrapper();
private:
    /** Bootstrapper for server address */
    bootstrapper_ptr m_bootstrapper;

    get_result create();
};

/**
 * DNS-over-TLS upstream
 */
class dns_over_tls : public ag::upstream {
public:
    /** Default port for DoT */
    static constexpr auto DEFAULT_PORT = 853;

    /**
     * Create DNS-over-TLS upstream
     * @param bootstrapper Bootstrapper for server address
     * @param timeout Timeout of requests
     */
    dns_over_tls(bootstrapper_ptr bootstrapper, std::chrono::milliseconds timeout);

    ~dns_over_tls() override = default;

    std::string address() override;

    std::pair<ldns_pkt_ptr, err_string> exchange(ldns_pkt *request_pkt) override;

private:
    /** TLS connection pool */
    tls_pool m_pool;
    /** Timeout */
    std::chrono::milliseconds m_timeout;
};

} // namespace ag
