#/usr/bin/python

import sys


def main():
    """
        main checks the cli arguments valid 
        print error message if not
        and if they are valid
        cleanup if the argument is 0 
        else show 
    """
    try: # try execute the main
        args = sys.argv
        check_valid_input(args)

        if len(args) == 2: # the case that the cli is: <program name> 0
            cleanup_packets_status()
        else: # the case that the cli is: <program naem> 
            show_packets_status()

    except TypeError as type_e:
        print(type_e)
    except ValueError as val_e:
        print(val_e)
    except RuntimeError as e:
        print(e)


def check_valid_input(args):
    """//sadassad
        check that the arguments besides the program name are valid means it is nothing or the argument is 0
    """
    # show packets case
    if len(args) == 1:
        pass
    # cleanup case
    elif len(args) == 2:
        
        arg_type = type(args[1])
        try:
            cleanup_arg = int(args[1])
        except TypeError as type_e:
            raise TypeError("Invalid type [{0}] not int".format(arg_type))
           
        if cleanup_arg != 0: # argument need to be 0
            raise ValueError("Invalid value [{0}] not zero".format(args[1]))
    else:  # too many arguments on cli need to be one more other than program name as most
        raise RuntimeError("Too many arguments [{0}] in cli not program name and optional cleanup argument".format(len(args)))


def cleanup_packets_status():
    """
        cleanup the packets file
    """
    with open("/sys/class/Sysfs_class/sysfs_class_sysfs_Device/rules", "w") as fd:
        fd.truncate()
        fd.write("0\n0\n")


def show_packets_status():
    """
        show the packets on the file
    """
    with open("/sys/class/fw/rules/rules") as fd:
        lst_values = fd.read()
        values = lst_values.split("\n")
        accept_number = int(values[0]) # get the accept packets number
        drop_number = int(values[1]) # get the drop packets number
        total = accept_number + drop_number # calculate the total packets number

        print_packets_status(accept_number, drop_number, total)


def print_packets_status(accept_number, drop_number, total):
    """
        print the packets to the standart output
    """
    print("Firewall Packets Summary:")
    print("Number of accepted packets:", accept_number)
    print("Number of dropped packets:", drop_number)
    print("Total number of packets:", total)


# check if the file is executed directly and call to main function to work
if __name__ == "__main__":
    main()
