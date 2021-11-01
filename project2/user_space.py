# /usr/bin/python

import sys


def main():
    try:
        args = sys.argv
        check_valid_input(args)

        if len(args) == 2:
            cleanup_packets_status()
        else:
            show_packets_status()

    except TypeError as type_e:
        print(type_e)
    except ValueError as val_e:
        print(val_e)
    except RuntimeError as e:
        print(e)

def check_valid_input(args):
    # no cleanup argument
    if len(args) == 1:
        pass
    elif len(args) == 2:
        cleanup_arg = int(args[1])
        if type(cleanup_arg) != int:
            raise TypeError("Invalid type [{0}] not int".format(args[1]))
        elif cleanup_arg != 0:
            raise ValueError("Invalid value [{0}] not zero".format(args[1]))
    else:  # too many arguments on cli
        raise RuntimeError("Too many arguments [{0}] in cli not program name and optional cleanup argument".format(len(args)))


def cleanup_packets_status():
    with open("/sys/class/Sysfs_class/sysfs_class_sysfs_Device") as fd:
        lst_values = fd.read()
        values = lst_values.split("\n")
        accpet_number = values[0]
        drop_number = values[1]
        total = values[0] + values[1]

        print_packets_status(accpet_number, drop_number, total)


def show_packets_status():
    print("In show_packets_status()")


def print_packets_status(accpet_number, drop_number, total):
    print("Firewall Packets Summary:")
    print("Number of accepted packets:", accept_number)
    print("Number of dropped packets:", drop_number)
    print("Total number of packets:", total)


if __name__ == "__main__":
    main()
