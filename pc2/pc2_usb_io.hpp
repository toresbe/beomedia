#include <vector>
#include <cstdint>
#include <mutex>
#include <thread>
#include <queue>
#include <future>

#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include <libusb-1.0/libusb.h>

typedef std::vector<uint8_t> PC2Message;

/**! \brief Low-level USB I/O with PC2 device
 * 
 * Implements only very basic read and write methods.
 */
class PC2DeviceIO {
    libusb_context *usb_ctx;                ///< libusb context variable
    libusb_device *pc2_dev;                 ///< Reference to PC2 device
    libusb_device_handle *pc2_handle;       ///< File descriptor/device handle of PC2 device

    libusb_transfer *transfer_out = NULL;   ///< Outgoing transfer struct
    std::timed_mutex transfer_out_mutex;    ///< Mutex for outgoing transfer struct

    libusb_transfer *transfer_in = NULL;    ///< Incoming transfer struct
    uint8_t input_buffer[1024];             ///< Incoming transfer data buffer

    bool keep_running = true;               ///< If this is set to 0, the USB event thread will terminate after libusb_handle_event returns

    std::thread * usb_thread;               ///< USB event handling loop thread

    void send_next();
    void usb_loop();

    std::mutex message_promises_mutex;
    std::queue<std::promise<PC2Message>> message_promises;

    public:
    void open();
    static void read_callback(struct libusb_transfer *transfer);
    static void write_callback(struct libusb_transfer *transfer);
    //TODO: Replace with the following
    //static void read_callback(struct libusb_transfer *transfer, PC2DeviceIO * instance);
    //static void write_callback(struct libusb_transfer *transfer, PC2DeviceIO * instance);
    PC2DeviceIO();
    ~PC2DeviceIO();
    void write(const PC2Message &message);
    std::shared_future<PC2Message> read();
};
