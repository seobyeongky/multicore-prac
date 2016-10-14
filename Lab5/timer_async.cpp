#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

void Print(const system::error_code& e) {
    cout << "Hello, world!" << endl;
}

int main(void) {
    asio::io_service io;

    asio::deadline_timer t(io, posix_time::seconds(2));
    t.async_wait(&Print);
    cout << "after async_wait" << endl;

    io.run();
    cout << "after io.run()" << endl;

    return 0;
}
