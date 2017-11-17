#include "wrapper.h"

#include <atomic>
#include <functional>


// Acceptor constructor
Acceptor::Acceptor(std::shared_ptr<Hive> hive)
    : hive_{hive}
    , acceptor_{hive->get_service()}
    , io_strand_{hive->get_service()}
    , timer_{hive->get_service()}
    , timer_interval_{1000}
    , error_state_{0}
{}

// Acceptor destructor
Acceptor::~Acceptor()
{}

// Acceptor::StartTimer definition
void Acceptor::start_timer()
{
    last_time_ = std::chrono::steady_clock::now();
    timer_.expires_from_now(std::chrono::milliseconds{timer_interval_});
    timer_.async_wait(io_strand_.wrap(std::bind(
        &Acceptor::handle_timer, shared_from_this(), std::placeholders::_1)));
}

// Acceptor::start_error definition
void Acceptor::start_error(std::error_code const& error)
{
    // if (boost::interprocess::ipcdetail::atomic_cas32(
    //     &m_error_state, 1, 0) == 0)
    uint32_t expected{0};
    if (error_state_.compare_exchange_strong(expected, 1)) {
        std::error_code ec;
        acceptor_.cancel(ec);
        acceptor_.close(ec);
        timer_.cancel(ec);
        on_error(error);
    }
}

// Acceptor::dispatch_accept definition
void Acceptor::dispatch_accept(std::shared_ptr<Connection> connection)
{
    acceptor_.async_accept(
        connection->get_socket(),
        connection->get_strand().wrap(std::bind(
            &Acceptor::handle_accept,
            shared_from_this(),
            std::placeholders::_1,
            connection)));
}

// Acceptor::handle_timer definition
void Acceptor::handle_timer(std::error_code const& error)
{
    if (error || has_error() || hive_->has_stopped()) {
        start_error(error);
    } else {
        on_timer(std::chrono::steady_clock::now() - last_time_);
        start_timer();
    }
}

// Accpetor::handle_accept definition
void Acceptor::handle_accept(
    std::error_code const& error, std::shared_ptr<Connection> connection)
{
    if (error || has_error() || hive_->has_stopped()) {
        connection->start_error(error);
    } else {
        if (connection->get_socket().is_open()) {
            connection->start_timer();
            if (on_accept(
                connection,
                connection->get_socket()
                    .remote_endpoint().address().to_string(),
                connection->get_socket().remote_endpoint().port())) {

                connection->on_accept(
                    acceptor_.local_endpoint().address().to_string(),
                    acceptor_.local_endpoint().port());
            }
        } else {
            start_error(error);
        }
    }
}

// Acceptor::stop definition
void Acceptor::stop()
{
    io_strand_.post(std::bind(
        &Acceptor::handle_timer,
        shared_from_this(),
        asio::error::connection_reset));
}

// Acceptor::accept definition
void Acceptor::accept(std::shared_ptr<Connection> connection)
{
    io_strand_.post(
        std::bind(&Acceptor::dispatch_accept, shared_from_this(), connection));
}

// Acceptor::listen definition
void Acceptor::listen(std::string const& host, uint16_t const &port)
{
    asio::ip::tcp::resolver resolver{hive_->get_service()};
    asio::ip::tcp::resolver::query query{host, std::to_string(port)};
    asio::ip::tcp::endpoint endpoint{*resolver.resolve(query)};
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address{false});
    acceptor_.bind(endpoint);
    acceptor_.listen(asio::socket_base::max_connections);
    start_timer();
}

// Acceptor::get_hive definition
std::shared_ptr<Hive> Acceptor::get_hive()
{
    return hive_;
}

// Acceptor::get_acceptor definition
asio::ip::tcp::acceptor& Acceptor::get_acceptor()
{
    return acceptor_;
}

// Acceptor::get_strand definition
asio::io_service::strand& Acceptor::get_strand()
{
    return io_strand_;
}

// Acceptor::get_timer_interval definition
int32_t Acceptor::get_timer_interval() const
{
    return timer_interval_;
}

// Acceptor::set_timer_interval definition
void Acceptor::set_timer_interval(int32_t timer_interval_ms)
{
    timer_interval_ = timer_interval_ms;
}

// Acceptor::has_error definition
bool Acceptor::has_error()
{
    // return (boost::interprocess::ipcdetail::atomic_cas32(
    //     &m_error_state, 1, 1) == 1);
    uint32_t expected{1};
    return error_state_.compare_exchange_strong(expected, 1);
}


// Connection constructor
Connection::Connection(std::shared_ptr<Hive> hive)
    : hive_(hive)
    , socket_{hive->get_service()}
    , io_strand_{hive->get_service()}
    , timer_{hive->get_service()}
    , receive_buffer_size_{4096}
    , timer_interval_{1000}
    , error_state_{0}
{}

// Connection destructor
Connection::~Connection()
{}

// Connection::bind definition
void Connection::bind(std::string const& ip, uint16_t port)
{
    asio::ip::tcp::endpoint endpoint{asio::ip::address::from_string(ip), port};
    socket_.open(endpoint.protocol());
    socket_.set_option(asio::ip::tcp::acceptor::reuse_address{false});
    socket_.bind(endpoint);
}

// Connection::start_send definition
void Connection::start_send()
{
    if (!pending_sends_.empty()) {
        asio::async_write(
            socket_,
            asio::buffer(pending_sends_.front()),
            io_strand_.wrap(std::bind(
                &Connection::handle_send,
                shared_from_this(),
                std::placeholders::_1,
                pending_sends_.begin())));
    }
}

// Connection:start_receive() definition
void Connection::start_recv(int32_t total_bytes)
{
    if (0 < total_bytes) {
        recv_buffer_.resize(total_bytes);
        asio::async_read(
            socket_,
            asio::buffer(recv_buffer_),
            io_strand_.wrap(std::bind(
                &Connection::handle_recv,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2)));
    } else {
        recv_buffer_.resize(receive_buffer_size_);
        socket_.async_read_some(
            asio::buffer(recv_buffer_),
            io_strand_.wrap(std::bind(
                &Connection::handle_recv,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2)));
    }
}

// Connection::start_timer definition
void Connection::start_timer()
{
    last_time_ = std::chrono::steady_clock::now();
    timer_.expires_from_now(std::chrono::milliseconds{timer_interval_});
    timer_.async_wait(io_strand_.wrap(std::bind(
        &Connection::dispatch_timer,
        shared_from_this(),
        std::placeholders::_1)));
}

// Connnection::start_error definition
void Connection::start_error(std::error_code const& error)
{
    // if (boost::interprocess::ipcdetail::atomic_cas32(
    //     &m_error_state, 1, 0) == 0)
    uint32_t expected{0};
    if (error_state_.compare_exchange_strong(expected, 1)) {
        std::error_code ec;
        socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        socket_.close(ec);
        timer_.cancel(ec);
        on_error(error);
    }
}

// Connection::handle_connect definition
void Connection::handle_connect(std::error_code const& error)
{
    if (error || has_error() || hive_->has_stopped()) {
        start_error(error);
    } else {
        if (socket_.is_open())
            on_connect(
                socket_.remote_endpoint().address().to_string(),
                socket_.remote_endpoint().port());
        else
            start_error(error);
    }
}

// Connection::handle_send definition
void Connection::handle_send(
    std::error_code const& error,
    std::list<std::vector<uint8_t>>::iterator iter)
{
    if (error || has_error() || hive_->has_stopped()) {
        start_error(error);
    } else {
        on_send(*iter);
        pending_sends_.erase(iter);
        start_send();
    }
}

// Connection::handle_recv definition
void Connection::handle_recv(
    std::error_code const& error, size_t actual_bytes)
{
    if (error || has_error() || hive_->has_stopped()) {
        start_error(error);
    } else {
        recv_buffer_.resize(actual_bytes);
        on_recv(recv_buffer_);
        pending_recvs_.pop_front();
        if (!pending_recvs_.empty())
            start_recv(pending_recvs_.front());
    }
}

// Connection::handle_timer definition
void Connection::handle_timer(std::error_code const& error)
{
    if (error || has_error() || hive_->has_stopped()) {
        start_error(error);
    } else {
        on_timer(std::chrono::steady_clock::now() - last_time_);
        start_timer();
    }
}

//Connection::dispatch_send definition
void Connection::dispatch_send(std::vector<uint8_t> buffer)
{
    auto should_start_send{pending_sends_.empty()};
    pending_sends_.push_back(buffer);
    if (should_start_send)
        start_send();
}

// Connection::dispatch_recv definition
void Connection::dispatch_recv(int32_t total_bytes)
{
    auto should_start_receive{pending_recvs_.empty()};
    pending_recvs_.push_back(total_bytes);
    if (should_start_receive)
        start_recv(total_bytes);
}

// Connection::dispatch_timer definition
void Connection::dispatch_timer(std::error_code const& error)
{
    io_strand_.post(
        std::bind(&Connection::handle_timer, shared_from_this(), error));
}

// Connection::connect definition
void Connection::connect(std::string const& host, uint16_t port)
{
    std::error_code ec;
    asio::ip::tcp::resolver resolver(hive_->get_service());
    asio::ip::tcp::resolver::query query(host, std::to_string(port));
    asio::ip::tcp::resolver::iterator iterator{resolver.resolve(query)};
    socket_.async_connect(
        *iterator,
        io_strand_.wrap(std::bind(
            &Connection::handle_connect,
            shared_from_this(),
            std::placeholders::_1)));
    start_timer();
}

// Connection::disconnect definition
void Connection::disconnect()
{
    io_strand_.post(std::bind(
        &Connection::handle_timer,
        shared_from_this(),
        asio::error::connection_reset));
}

// Connection::recv definition
void Connection::recv(int32_t total_bytes)
{
    io_strand_.post(std::bind(
        &Connection::dispatch_recv, shared_from_this(), total_bytes));
}

// Connection::send definition
void Connection::send(std::vector<uint8_t> const& buffer)
{
    io_strand_.post(
        std::bind(&Connection::dispatch_send, shared_from_this(), buffer));
}

// Connection::get_socket definition
asio::ip::tcp::socket& Connection::get_socket()
{
    return socket_;
}

// Connection::get_strand definition
asio::io_service::strand& Connection::get_strand()
{
    return io_strand_;
}

// Connection::get_hive definition
std::shared_ptr<Hive> Connection::get_hive()
{
    return hive_;
}

// Connection::set_receive_buffer_size definition
void Connection::set_receive_buffer_size(int32_t size)
{
    receive_buffer_size_ = size;
}

// Connection::get_receive_buffer_size definition
int32_t Connection::get_receive_buffer_size() const
{
    return receive_buffer_size_;
}

// Connection::get_timer_interval definition
int32_t Connection::get_timer_interval() const
{
    return timer_interval_;
}

// Connection::set_timer_interval definition
void Connection::set_timer_interval(int32_t timer_interval_ms)
{
    timer_interval_ = timer_interval_ms;
}

// Connection::has_error definition
bool Connection::has_error()
{
    // return (boost::interprocess::ipcdetail::atomic_cas32(
    //     &m_error_state, 1, 1) == 1);
    uint32_t expected{1};
    return true == error_state_.compare_exchange_strong(expected, 1);
}


// Hive constructor
Hive::Hive() : work_ptr_{new asio::io_service::work{io_service_}}, shutdown_{0}
{}

// Hive destructor
Hive::~Hive()
{}

// Hive::get_service definition
asio::io_service& Hive::get_service()
{
    return io_service_;
}

// Hive::has_stopped definition
bool Hive::has_stopped()
{
    // return (boost::interprocess::ipcdetail::atomic_cas32(
    //     &m_shutdown, 1, 1) == 1);
    uint32_t expected{1};
    return shutdown_.compare_exchange_strong(expected, 1);
}

// Hive::poll definition
void Hive::poll()
{
    io_service_.poll();
}

// Hive::run definition
void Hive::run()
{
    io_service_.run();
}

// Hive::stop definition
void Hive::stop()
{
    // if (boost::interprocess::ipcdetail::atomic_cas32(
    //     &m_shutdown, 1, 0) == 0)
    uint32_t expected{0};
    if (shutdown_.compare_exchange_strong(expected, 1)) {
        work_ptr_.reset();
        io_service_.run();
        io_service_.stop();
    }
}

// Hive::reset definition
void Hive::reset()
{
    // if (boost::interprocess::ipcdetail::atomic_cas32(
    //     &m_shutdown, 0, 1) == 1)
    uint32_t expected{1};
    if (shutdown_.compare_exchange_strong(expected, 0)) {
        io_service_.reset();
        work_ptr_.reset(new asio::io_service::work(io_service_));
    }
}
