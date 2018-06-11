
#ifndef cppkit_ck_socket_address_h
#define cppkit_ck_socket_address_h

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace cppkit
{

/// String constant that can be used with XSocketAddress to represent the IPv4 INADDR_ANY enumeration.
const std::string ip4_addr_any("INADDR_ANY");

/// String constant that can be used with XSocketAddress to represent the IPv6 IN6ADDR_ANY enumeration.
const std::string ip6_addr_any("IN6ADDR_ANY");

/// This class provides a common way to deal with IPv4 and IPv6 addresses.
class ck_socket_address final
{
public:
    ck_socket_address(int port, const std::string& address = ip4_addr_any);

    ck_socket_address(const struct sockaddr* addr, const socklen_t len);

    ck_socket_address(ck_socket_address&& obj) noexcept;

    ck_socket_address(const ck_socket_address& obj);

    ~ck_socket_address() noexcept;

    ck_socket_address& operator = (ck_socket_address&& obj) noexcept;

    ck_socket_address& operator = (const ck_socket_address& obj);

    int port() const { return _port; }
    void set_port_num(int port); // Can't be SetPort b/c of stupid Window macro of same name

    const std::string& address() const { return _addr; }

    void set_address(const std::string& addr, int port = -1);

    unsigned int address_family() const { return _sockaddr.ss_family; }

    bool operator==(const ck_socket_address&) const;
    bool operator==(const struct sockaddr*) const;
    bool operator==(const struct sockaddr_storage&) const;
    bool operator!=(const ck_socket_address&) const;
    bool operator!=(const struct sockaddr*) const;
    bool operator!=(const struct sockaddr_storage&) const;

    struct sockaddr* get_sock_addr() const { return (struct sockaddr*)&_sockaddr; }

    socklen_t sock_addr_size() const { return sock_addr_size(_sockaddr.ss_family); }

    bool is_ipv4() const;
    bool is_ipv6() const;
    bool is_multicast() const;

    bool is_ipv4_mapped_to_ipv6(std::string* unmapped=0) const;

    bool is_wildcard_address() const;

    static unsigned int get_address_family(const std::string& address, struct sockaddr* saddr=0);

    static socklen_t sock_addr_size(unsigned int addrFamily);

    static std::string address_to_string(const struct sockaddr* addr, const socklen_t len);

    static void string_to_address(const std::string& saddr,
                                  struct sockaddr* addr,
                                  const socklen_t len);

    static std::string isolate_address(const std::string& addr);

    static bool is_hostname(const std::string& addr);

    static bool is_ipv4(const std::string& addr);
    static bool is_ipv4(const struct sockaddr* addr, const socklen_t len);
    static bool is_ipv6(const std::string& addr);
    static bool is_ipv6(const struct sockaddr* addr, const socklen_t len);
    static bool is_multicast(const std::string& addr);
    static bool is_multicast(const struct sockaddr* addr, const socklen_t len);
    static bool is_wildcard_address(const std::string& addr);
    static bool is_wildcard_address(const struct sockaddr* addr, const socklen_t len);

    static bool is_ipv4_mapped_to_ipv6(const std::string& addr, std::string* unmapped=0);

    static bool is_ipv4_mapped_to_ipv6(const struct sockaddr* addr,
                                       const socklen_t len,
                                       std::string* unmapped=0);

private:
    int _port;
    std::string _addr;
    struct sockaddr_storage _sockaddr;
};

}

#endif
