
#include "i2c-gpio.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>

#define CHECK(instr) do { int ___res___ = instr; if (___res___ < 0) return ___res___; } while (0)

typedef enum {
    ERR_SEEK_FAILED = -1,
    ERR_READ_FAILED = -2,
    ERR_BOGUS_VALUE = -3,
    ERR_WRITE_FAILED = -4,
    ERR_SCL_HI_TIMEOUT = -5,
    ERR_SCL_NOT_LO = -6,
    ERR_OPEN_FAILED = -7,
    ERR_EXPORT_FAILED = -8,
    ERR_UNEXPORT_FAILED = -9
} GpioError;

static inline void debug(const char *fmt, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
#endif
}

static char errbuf[1024] = {0};

static void err(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(errbuf, sizeof(errbuf), fmt, args);
    va_end(args);
}

static inline void __usleep(long usec) {
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = usec * 1000;
	nanosleep(&ts, NULL);
}

static inline void DELAY(float usec) {
    long u = (long) (usec * DELAY_STRETCH);
    __usleep(u);
}

static int readval(int fd, char *desc) {
	char val[3] = {0};
    if (lseek(fd, 0, SEEK_SET) != 0) {
        err("Failed to seek %s to 0", desc);
        return ERR_SEEK_FAILED;
    }
	int res = read(fd, val, sizeof(val));
	if (res < 0) {
        err("Failed to read %s", desc);
        return ERR_READ_FAILED;
	}
	if (val[0] == '1') {
        debug("Read %s: 1\n", desc);
		return 1;
	} else if (val[0] == '0') {
        debug("Read %s: 0\n", desc);
		return 0;
	} else {
        err("Bogus %s value: %s", desc, val);
		return ERR_BOGUS_VALUE;
	}
}

static inline int setval(int fd, BOOL val, char *desc) {
    int res;
    debug("Writ %s: %i\n", desc, val);
    if (val)
        res = write(fd, "in", 2);
    else
        res = write(fd, "low", 3);
    if (res < 0) {
        err("Writing %s to %i failed", desc, val);
        return ERR_WRITE_FAILED;
    }
    return 0;
}

static inline int sdalo(GpioI2C bus) {
    return setval(bus->sdaDir, 0, "sda(low)");
}

static inline int sdahi(GpioI2C bus) {
    return setval(bus->sdaDir, 1, "sda(high)");
}

static inline int scllo(GpioI2C bus) {
    return setval(bus->sclDir, 0, "scl(low)");
}

static inline int sclhi(GpioI2C bus) {
    return setval(bus->sclDir, 1, "scl(high)");
}

static int getsda(GpioI2C bus) {
    return readval(bus->sdaVal, "sda(in)");
}

static int getscl(GpioI2C bus) {
    return readval(bus->sclVal, "scl(in)");
}

static int wait_scl_hi(GpioI2C bus) {
    CHECK(sclhi(bus));
    DELAY(1);
    for(int i = 0; i < TIMEOUT_RETRIES; i++) {
        int res = getscl(bus);
        if (res < 0) return res;
        if (res == 1) return 0;
        __usleep(TIMEOUT_SLEEP_MICRO);
    }
    err("Waiting for scl high timed out");
    return ERR_SCL_HI_TIMEOUT;
}

int i2c_gpio_start(GpioI2C bus) {
    debug(" == START\n");
    CHECK(wait_scl_hi(bus));
    // TODO: ensure sda hi?
    CHECK(sdalo(bus));
	DELAY(2);
    CHECK(scllo(bus));
	DELAY(1);
    return 0;
}

int i2c_gpio_repstart(GpioI2C bus) {
    debug(" == REPEATED START\n");
    CHECK(sdahi(bus));
	DELAY(1.5);
    return i2c_gpio_start(bus);
}

int i2c_gpio_stop(GpioI2C bus) {
    debug(" == STOP\n");
	CHECK(sdalo(bus));
	DELAY(1.5);
    int res = wait_scl_hi(bus); // Must release sda even if this fails
	DELAY(2);
    CHECK(sdahi(bus));
	DELAY(2);
    return res;
}

static int sendbit(GpioI2C bus, int bit) {
    debug(" - Sending bit: %i\n", bit);
    if (bit)
        CHECK(sdahi(bus));
    else
        CHECK(sdalo(bus));
    DELAY(1.5);
    CHECK(wait_scl_hi(bus));
    DELAY(2);
    CHECK(scllo(bus));
    DELAY(1);
    return 0;
}

static int receivebit(GpioI2C bus) {
    CHECK(wait_scl_hi(bus));
    DELAY(5);
	int bit = getsda(bus);
    if (bit < 0) return bit;
    debug(" - received: %i\n", bit);
	CHECK(scllo(bus));
	DELAY(1);
    return bit;
}

static int readack(GpioI2C bus) {
    debug(" - Reading ack\n");
    int bit = receivebit(bus);
    if (bit < 0) return bit;
    return !bit;
}

static int check_scl_lo(GpioI2C bus) {
    int res = getscl(bus);
    switch (res) {
        case 0: return 0;
        case 1: 
            err(""); // Clear error buffer
            return ERR_SCL_NOT_LO;
        default: return res;
    }
}

int i2c_gpio_write(GpioI2C bus, unsigned char c) {
    debug(" == Sending byte: %02x\n", c);
    CHECK(check_scl_lo(bus)); // Must be low from START condition
	for (int i = 7; i >= 0; i--) {
		int sb = (c >> i) & 1;
        int res = sendbit(bus, sb);
        if (res < 0) return res;
	}
	CHECK(sdahi(bus));
	DELAY(1.5);
    return readack(bus);
}

int i2c_gpio_read(GpioI2C bus) {
    debug(" == Receiving byte...\n");
	unsigned char indata = 0;
    CHECK(check_scl_lo(bus)); // Must be low from START condition
	CHECK(sdahi(bus));
	DELAY(1.5);
	for (int i = 0; i < 8; i++) {
        int bit = receivebit(bus);
        if (bit < 0) return bit;
        indata *= 2;
		if (bit == 1)
			indata |= 0x01;
	}
    // Send ACK
    CHECK(sendbit(bus, 0));
    debug(" == Received: %02x\n", indata);
	return (int) indata;
}

int i2c_gpio_try_address(GpioI2C bus, unsigned char addr) {
    CHECK(i2c_gpio_start(bus));
    int writeRes = i2c_gpio_write(bus, addr);
    int stopRes = i2c_gpio_stop(bus);
    if (stopRes < 0) return stopRes;
    return writeRes;
}

int i2c_gpio_retry_address(GpioI2C bus, unsigned char addr, int retries) {
    int res = 0;
	for (int i = 0; i < retries && res != 1; i++) {
        res = i2c_gpio_try_address(bus, addr);
        DELAY(2);
	}
	return res;
}

static int gpio_export(char *pin)  {
    int fd = open(GPIO_PATH_EXPORT, O_WRONLY);
    if (fd < 0) {
        err("Failed to open %s for exporting %s", GPIO_PATH_EXPORT, pin);
        return ERR_EXPORT_FAILED;
    } else {
        int res = write(fd, pin, strlen(pin));
        if (res < 0) {
            close(fd);
            err("Failed to export %s", pin);
            return ERR_EXPORT_FAILED;
        }
    }
    close(fd);
    return 0;
}

static int gpio_unexport(char *pin)  {
    int fd = open(GPIO_PATH_UNEXPORT, O_WRONLY);
    if (fd < 0) {
        err("Failed to open %s for unexporting %s", GPIO_PATH_UNEXPORT, pin);
        return ERR_UNEXPORT_FAILED;
    } else {
        int res = write(fd, pin, strlen(pin));
        if (res < 0) {
            close(fd);
            err("Failed to unexport %s", pin);
            return ERR_UNEXPORT_FAILED;
        }
    }
    close(fd);
    return 0;
}

int i2c_gpio_destroy(GpioI2C bus) {
    int res1 = gpio_unexport(bus->sdaPinNum);
    int res2 = gpio_unexport(bus->sclPinNum);
    if (bus->sclVal > 0) close(bus->sclVal);
    if (bus->sdaVal > 0) close(bus->sdaVal);
    if (bus->sclDir > 0) close(bus->sclDir);
    if (bus->sdaDir > 0) close(bus->sdaDir);
    return res2 < 0 ? res2 : res1;
}

static int open_failed_err(char *file_name) {
    err("Failed to open %s", file_name);
    return ERR_OPEN_FAILED;
}

int i2c_gpio_init(GpioI2C bus) {
    CHECK(gpio_export(bus->sdaPinNum));
    CHECK(gpio_export(bus->sclPinNum));
    
	char file_name[512];
	
	snprintf(file_name, sizeof(file_name), "%s%s%s", GPIO_PATH, bus->sdaPinNum, GPIO_VAL);
	bus->sdaVal = open(file_name, O_RDWR);
	if (bus->sdaVal < 0) return open_failed_err(file_name);

	snprintf(file_name, sizeof(file_name), "%s%s%s", GPIO_PATH, bus->sclPinNum, GPIO_VAL);
	bus->sclVal = open(file_name, O_RDWR);
	if (bus->sclVal < 0) return open_failed_err(file_name);

	snprintf(file_name, sizeof(file_name), "%s%s%s", GPIO_PATH, bus->sdaPinNum, GPIO_DIR);
	bus->sdaDir = open(file_name, O_RDWR);
	if (bus->sdaDir < 0) return open_failed_err(file_name);
	
	snprintf(file_name, sizeof(file_name), "%s%s%s", GPIO_PATH, bus->sclPinNum, GPIO_DIR);
	bus->sclDir = open(file_name, O_RDWR);
	if (bus->sclDir < 0) return open_failed_err(file_name);

    return 0;
}

char *i2c_gpio_errstring(int code) {
    static char ext_errbuf[2048];
    char *msg;
    switch ((GpioError) code) {
        case ERR_SEEK_FAILED:
            msg = "Seek failed"; break;
        case ERR_READ_FAILED:
            msg = "Read failed"; break;
        case ERR_BOGUS_VALUE:
            msg = "Bogus input value"; break;
        case ERR_WRITE_FAILED:
            msg = "Write failed"; break;
        case ERR_SCL_HI_TIMEOUT:
            msg = "SCL high timeout"; break;
        case ERR_SCL_NOT_LO:
            msg = "SCL must be low (execute START first)"; break;
        case ERR_OPEN_FAILED:
            msg = "Open failed"; break;
        case ERR_EXPORT_FAILED:
            msg = "Export failed"; break;
        case ERR_UNEXPORT_FAILED:
            msg = "Unexport failed"; break;
        default:
            msg = "Unknown error code"; break;
    }
    int n = snprintf(ext_errbuf, sizeof(ext_errbuf), "%s: %s", msg, errbuf);
    if (errno != 0)
        snprintf(ext_errbuf + n, sizeof(ext_errbuf) - n, " (%s)", strerror(errno));
    return ext_errbuf;
}

