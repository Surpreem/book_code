#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

#include "asio.hpp"


// Class declaration
class Acceptor;
class Connection;
class Hive;


// Class Acceptor definition and its members declaration
class Acceptor : public std::enable_shared_from_this<Acceptor> {
    friend class Hive;

public:
    // Returns the Hive object.
    std::shared_ptr<Hive> get_hive();

    // Returns the acceptor object.
    asio::ip::tcp::acceptor& get_acceptor();

    // Returns the strand object.
    asio::io_service::strand& get_strand();

    // Sets the time interval of the object. The interval is changed
    // after the next update is called. The default value is 1000ms.
    void set_timer_interval(int32_t timer_interval_ms);

    // Returns the timer interval of the object.
    int32_t get_timer_interval() const;

    // Returns true if this object has an error associated with it.
    bool has_error();

    // Begin listening on the specific network interface.
    void listen(std::string const& host, uint16_t const& port);

    // Posts the connection to the listening interface. The next client
    // that connections will be given this connection. If multiple calls
    // to accept() are called at a time, then they are accepted in a FIFO
    // order.
    void accept(std::shared_ptr<Connection> connection);

    // Stop the Acceptor from listening.
    void stop();

protected:
    Acceptor(std::shared_ptr<Hive> hive);
    virtual ~Acceptor();

private:
    Acceptor(Acceptor const& rhs);
    Acceptor& operator=(Acceptor const& rhs);

    void start_timer();
    void start_error(std::error_code const& error);
    void dispatch_accept(std::shared_ptr<Connection> connection);
    void handle_timer(std::error_code const& error);
    void handle_accept(
        std::error_code const& error, std::shared_ptr<Connection> connection);

    // Called when a connection has connected to the server. This function
    // should return true to invoke the connection's on_accept function if
    // the connection will be kept. If the connection will not be kept, the
    // connection's disconnect function should be called and the function
    // should return false.
    virtual bool on_accept(
        std::shared_ptr<Connection> connection,
        std::string const& host,
        uint16_t port) = 0;

    // Called on each timer event.
    virtual void on_timer(std::chrono::duration<double> const& delta) = 0;

    // Called when an error is encountered. Most typically, this is when the
    // acceptor is being closed via the stop function or if the listen is
    // called on an address that is not available.
    virtual void on_error(std::error_code const& error) = 0;

    std::shared_ptr<Hive> hive_;
    asio::ip::tcp::acceptor acceptor_;
    asio::io_service::strand io_strand_;
    asio::steady_timer timer_;
    std::chrono::steady_clock::time_point last_time_;
    int32_t timer_interval_;
    // volatile uint32_t error_state_;
    std::atomic<uint32_t> error_state_;
};

// Class Connection definition and its members declaration
class Connection : public std::enable_shared_from_this<Connection> {
    friend class Acceptor;
    friend class Hive;

public:
    // Returns the Hive object.
    std::shared_ptr<Hive> get_hive();

    // Returns the socket object.
    asio::ip::tcp::socket& get_socket();

    // Returns the strand object.
    asio::io_service::strand& get_strand();

    // Sets the application specific receive buffer size used. For stream
    // based protocols such as HTTP, you want this to be pretty large, like
    // 64Kb. For packet based protocols, then it will be much smaller,
    // usually 512b - 8Kb depending on the protocol. The default value is
    // 4Kb.
    void set_receive_buffer_size(int32_t size);

    // Returns the size of the receive buffer size of the current object.
    int32_t get_receive_buffer_size() const;

    // Sets the timer interval of the object. The interval is changed after
    // the next update is called.
    void set_timer_interval(int32_t timer_interval_ms);

    // Returns the timer interval of the object.
    int32_t get_timer_interval() const;

    // Returns true if this object has an error associated with it.
    bool has_error();

    // Binds the socket to the specified interface.
    void bind(std::string const& ip, uint16_t port);

    // Starts an a/synchronous connect.
    void connect(std::string const& host, uint16_t port);

    // Posts data to be sent to the connection.
    void send(std::vector<uint8_t> const& buffer);

    // Posts a recv for the connection to process. If total_bytes is 0, then
    // as many bytes as possible up to get_receive_buffer_size() will be
    // waited for. If recv() is not 0, then the connection will wait for
    // exactly total_bytes before invoking on_recv().
    void recv(int32_t total_bytes = 0);

    // Posts an asynchronous disconnect event for the object to process.
    void disconnect();

protected:
    Connection(std::shared_ptr<Hive> hive);
    virtual ~Connection();

private:
    Connection(Connection const& rhs);
    Connection& operator=(Connection const& rhs);

    void start_send();
    void start_recv(int32_t total_bytes);
    void start_timer();
    void start_error(std::error_code const& error);
    void dispatch_send(std::vector<uint8_t> buffer);
    void dispatch_recv(int32_t total_bytes);
    void dispatch_timer(std::error_code const& error);
    void handle_connect(std::error_code const& error);
    void handle_send(
        std::error_code const& error,
        std::list<std::vector<uint8_t>>::iterator iter);
    void handle_recv(std::error_code const& error, size_t actual_bytes);
    void handle_timer(std::error_code const& error);

    // Called when the connection has successfully connected to the local
    // host.
    virtual void on_accept(std::string const& host, uint16_t port) = 0;

    // Called when the connection has successfully connected to the remote
    // host.
    virtual void on_connect(std::string const& host, uint16_t port) = 0;

    // Called when data has been sent by the connection.
    virtual void on_send(std::vector<uint8_t> const& buffer) = 0;

    // Called when data has been received by the connection.
    virtual void on_recv(std::vector<uint8_t> const& buffer) = 0;

    // Called on each timer event.
    virtual void on_timer(std::chrono::duration<double> const& delta) = 0;

    // Called when an error is encountered.
    virtual void on_error(std::error_code const& error) = 0;

    std::shared_ptr<Hive> hive_;
    asio::ip::tcp::socket socket_;
    asio::io_service::strand io_strand_;
    asio::steady_timer timer_;
    std::chrono::steady_clock::time_point last_time_;
    std::vector<uint8_t> recv_buffer_;
    std::list<int32_t> pending_recvs_;
    std::list<std::vector<uint8_t>> pending_sends_;
    int32_t receive_buffer_size_;
    int32_t timer_interval_;
    // volatile uint32_t error_state_;
    std::atomic<uint32_t> error_state_;
};

// Class Hive definition and its members declaration
class Hive : public std::enable_shared_from_this<Hive> {
public:
    Hive();
    virtual ~Hive();

    // Return the io_service of this object.
    asio::io_service& get_service();

    // Return true if the Stop function has been called.
    bool has_stopped();

    // Polls the networking subsystem once from the current thread
    // and returns.
    void poll();

    // Runs the networking system on the current thread. This function
    // blocks until the networking system is stopped, so do not call on
    // a single threaded application with no other means of being able
    // to call stop() unless you code in such logic.
    void run();

    // Stops the networking system. All work is finished and no more
    // networking interaction will be possible afterwards until reset()
    // is called.
    void stop();

    // Restarts the networking system after stop() as been called. A new
    // work object is created and the shutdown flag is cleared.
    void reset();

private:
    Hive(Hive const& rhs);
    Hive& operator=(Hive const& rhs);

    asio::io_service io_service_;
    std::shared_ptr<asio::io_service::work> work_ptr_;
    //volatile uint32_t shutdown_;
    std::atomic<uint32_t> shutdown_;
};
