import struct

from os import path
from serial import Serial
from collections import namedtuple

# entry size 19, 4 and finally 13 for the headers

LOG_HEADER_SIZE = 13
STATUS_LOG_ENTRY_SIZE = 4
DATA_LOG_ENTRY_SIZE = 19

STATUS_FILE_NAME = "STAT.TXT"

_data_buffer = []

TIME_ELEMENTS_KEYS = [
    "Second",
    "Minute",
    "Hour",
    "Wday",
    "Day",
    "Month",
    "Year"
]

TIME_ELEMENTS = namedtuple("TIME_ELEMENTS", TIME_ELEMENTS_KEYS)

LOG_HEADER_KEYS = [
    "flag",
    "n_entries",
    "entry_size",
    "max_entries"
]

LOG_HEADER = namedtuple("LOG_HEADER", LOG_HEADER_KEYS)

STATUS_LOG_ENTRY_KEYS = [
    "cur_log_file_prefix",
    "data_log_header_index"
]

STATUS_LOG_ENTRY = namedtuple("STATUS_LOG_ENTRY", STATUS_LOG_ENTRY_KEYS)

DATA_LOG_ENTRY_KEYS = [
    "moisture",
    "brightness",
    "humidity",
    "temperature",
    "log_time"
]

DATA_LOG_ENTRY = namedtuple("DATA_LOG_ENTRY", DATA_LOG_ENTRY_KEYS)


def _load_time_elements(bytes):
    global TIME_ELEMENTS
    time = TIME_ELEMENTS._make(
        struct.unpack('<BBBBBBB', bytes)
    )
    return time


def _load_log_header(bytes):
    global LOG_HEADER
    header = LOG_HEADER._make(
        struct.unpack('<BIII', bytes)
    )
    return header


def _load_status_log_entry(bytes):
    global STATUS_LOG_ENTRY
    entry = STATUS_LOG_ENTRY._make(
        struct.unpack('<HH', bytes)
    )
    return entry


def _load_data_log_entry(bytes):
    global DATA_LOG_ENTRY, TIME_ELEMENTS
    entry = DATA_LOG_ENTRY._make(
        struct.unpack('<hhff7s', bytes)
    )

    time = _load_time_elements(entry.log_time)
    entry = entry._replace(log_time=time)
    return entry


def _get_status_log_entry(fs_path):
    entry = None

    abs_path = path.abspath(fs_path)
    status_file_path = abs_path + "/" + STATUS_FILE_NAME

    with open(status_file_path, "rb+") as status_file:
        content = status_file.read()

        header = _load_log_header(content[0:LOG_HEADER_SIZE])

        print("--------------------")
        print(header.flag)
        print(header.n_entries)
        print(header.entry_size)
        print(header.max_entries)
        print("--------------------")

        if (header.n_entries > 0):
            entry = _load_status_log_entry(
                content[LOG_HEADER_SIZE:LOG_HEADER_SIZE + STATUS_LOG_ENTRY_SIZE])

            print("--------------------")
            print(entry.cur_log_file_prefix)
            print(entry.data_log_header_index)
            print("--------------------")

    return entry


def serial_read(dev_file):
    print("not yet implemented")


def fs_read(fs_path):
    status_entry = _get_status_log_entry(fs_path)

    if status_entry is None or status_entry.cur_log_file_prefix < 0:
        return None
    
    abs_path = path.abspath(fs_path)

    for i1 in range(0, status_entry.cur_log_file_prefix + 1):
        log_file_path = abs_path + "/D" + \
            str(status_entry.cur_log_file_prefix) + ".TXT"

        with open(log_file_path, "rb+") as log_file:
            content = log_file.read()

            header = _load_log_header(content[0:LOG_HEADER_SIZE])

            print("---------- " + str(i1) + " ----------")
            print(header.flag)
            print(header.n_entries)
            print(header.entry_size)
            print(header.max_entries)
            print("--------------------")

            for i2 in range(LOG_HEADER_SIZE, len(content), DATA_LOG_ENTRY_SIZE):
                entry = _load_data_log_entry(
                    content[i2:i2 + DATA_LOG_ENTRY_SIZE])

                _data_buffer.append(entry)

                # print("---------- " + str(i1) + " : " + str(i2) + " ----------")
                # print("moisture: \t" + str(entry.moisture))
                # print("brightness: \t" + str(entry.brightness))
                # print("humidity: \t" + str(entry.humidity))
                # print("temperature: \t" + str(entry.temperature))
                # print("Second: " + str(entry.log_time.Second))
                # print("Minute: " + str(entry.log_time.Minute))
                # print("Hour: \t" + str(entry.log_time.Hour))
                # print("Wday: \t" + str(entry.log_time.Wday))
                # print("Day: \t" + str(entry.log_time.Day))
                # print("Month: \t" + str(entry.log_time.Month))
                # print("Year: \t" + str(entry.log_time.Year))
                # # print("--------------------")

    return _data_buffer
