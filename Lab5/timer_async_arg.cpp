#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

void Print(const system::error_code& /*e*/,
        asio::deadline_timer* t,
        int* count) {
    if (*count < 5) {
        cout << *count << endl;
        ++(*count);

        t->expires_at(t->expires_at() + posix_time::seconds(1));
        t->async_wait(bind(Print, asio::placeholders::error, t, count));
    }
}

int main(void) {
    asio::io_service io;

    int count = 0;
    asio::deadline_timer t(io, posix_time::seconds(1));
    t.async_wait(bind(Print, asio::placeholders::error, &t, &count));

    io.run();
    cout << "Final count is " << count << endl;

    return 0;
}
